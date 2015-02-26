/*
 * Copyright (c) 2015 Mikkel Kroman, All rights reserved.
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3.0 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.
 */

#include <cstdio>
#include <cstddef>
#include <fstream>

#include <openssl/sha.h>

#include "SQLite3/SQLite3.h"
#include "OFN/Puzzle.h"

#include "OFN/OFN.h"
#include "OFN/Image.h"
#include "OFN/Context.h"

using namespace OFN;

static void print_sqlite_trace(void* context, const char* sql)
{
    (void)context;
    auto console = spdlog::get("console");

    SPDLOG_TRACE(console, "SQL: {}", sql);
}

Context::Context() :
    conn_(std::make_shared<SQLite3::Connection>("ofn.db")),
    puzzle_(std::make_shared<Puzzle::Context>())
{
    conn_->SetTrace(print_sqlite_trace);
}

Context::~Context()
{
}

void Context::Search(const Image& image)
{
    auto console = spdlog::get("console");
    auto words = CompressWords(image.GetWords());
    auto statement = conn_->Prepare(
        "SELECT DISTINCT(signature_id) FROM words "
        "WHERE pos_and_word IN (?, ?, ?, ?, ?, ?, ?, ?, "
        "?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, "
        "?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, "
        "?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, "
        "?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, "
        "?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");

    for (auto i = 0; i < Image::MAX_WORDS; i++)
    {
        auto word = words[i];
        statement->Bind(i + 1, word.data(), word.size(), true);
    }

    auto signature_statement =
        conn_->Prepare("SELECT image_id, compressed_signature FROM "
                       "signatures WHERE(id = ?)");

    auto image_statement =
        conn_->Prepare("SELECT digest, filename FROM images WHERE(id = ?)");

    if (!signature_statement || !image_statement)
        console->error("Error when preparing statement: {}",
                       conn_->GetErrorMessage());

    // Iterate over each row.
    while (auto words_row = statement->Step())
    {
        auto signature_id = words_row.GetInt64(0);

        console->info("Got row with id {:d}", signature_id);

        // Find the associated signature by id.
        signature_statement->Bind(1, signature_id);
        
        if (auto signature_row = signature_statement->Step())
        {
            auto image_id = signature_row.GetInt64(0);
            auto signature = signature_row.GetBlob(1);

            // Find the associated image by id.
            image_statement->Bind(1, image_id);

            if (auto image_row = image_statement->Step())
            {
                auto digest = image_row.GetBlob(0);
                auto filename = image_row.GetText(1);
                auto uncompressed = Puzzle::CVecFromCompressedBuffer(
                    puzzle_, signature.data(), signature.size());
                auto distance = image.Compare(*uncompressed.get());

                console->debug("Found matching image '{}'", filename.data());

                if (distance < SimilarityThreshold)
                {
                    console->debug(
                        "The images are similar! normalized distance: {}",
                        distance);
                }
            }

            image_statement->Reset();
        }

        signature_statement->Reset();
    }
}

void Context::Commit(const Image& image)
{
    auto console = spdlog::get("console");
    int image_id, signature_id;

    if ((image_id = SaveImage(image)) == -1)
    {
        console->error("SaveImage returned -1");
        return;
    }

    if ((signature_id = SaveImageSignature(image, image_id)) == -1)
    {
        console->error("SaveImageSignature returned -1");
        return;
    }

    conn_->Execute("BEGIN TRANSACTION");

    if (!SaveImageWords(image, image_id, signature_id))
    {
        console->error("SaveImageWords failed");
    }

    conn_->Execute("END TRANSACTION");
}

int Context::SaveImage(const Image& image)
{
    auto console = spdlog::get("console");

    try
    {
        auto statement = conn_->Prepare(
            "INSERT INTO images (filename, digest) VALUES (?, ?)");

        auto digest = SHA256File(image.GetFileName());

        statement->Bind(1, image.GetFileName()); // filename
        statement->Bind(2, digest.data(), digest.size()); // digest
        statement->Step();

        if (!statement->IsDone())
        {
            console->trace("SQL Step failed: {}", conn_->GetErrorMessage());

            return -1;
        }
    }
    catch (const SQLite3::ConnectionError& error)
    {
        throw TransactionError(error.what());
    }

    return conn_->GetLastInsertRowID();
}

int Context::SaveImageSignature(const Image& image, int image_id)
{
    try
    {
        auto statement = conn_->Prepare("INSERT INTO signatures (image_id, "
                                   "compressed_signature) VALUES(?, ?)");

        auto cvec = image.GetCvec();
        auto compressed = cvec->Compress();

        statement->Bind(1, image_id);
        statement->Bind(2, compressed->GetVec(), compressed->GetSize());
        statement->Step();

        if (!statement->IsDone())
        {
            return -1;
        }
    }
    catch (const SQLite3::ConnectionError& error)
    {
        throw TransactionError(error.what());
    }

    return conn_->GetLastInsertRowID();
}

bool Context::SaveImageWords(const Image& image, int image_id, int signature_id)
{
    (void)image_id;
    auto console = spdlog::get("console");
    auto statement = conn_->Prepare(
        "INSERT INTO words (signature_id, pos_and_word) VALUES(?, ?)");

    auto words = CompressWords(image.GetWords());

    for (auto word : words)
    {
        statement->Reset();
        statement->Bind(1, signature_id);
        statement->Bind(2, word.data(), word.size());
        statement->Step();

        if (!statement->IsDone())
        {
            console->error("Failed to insert word: {}",
                           conn_->GetErrorMessage());
        }
    }

    return true;
}

using StringVector = std::vector<std::string>;
StringVector Context::CompressWords(const StringVector& words) const
{
    Puzzle::CVec cvec(puzzle_);
    std::vector<std::string> result;

    result.reserve(words.size());

    for (auto word : words)
    {
        cvec.SetVec(word);
        auto compr_cvec = cvec.Compress();

        result.emplace_back(reinterpret_cast<char*>(compr_cvec->GetVec()),
                            compr_cvec->GetSize());
    }

    // Avoid having the destructor free the vector buffer seeing as we don't
    // have ownership of it.
    cvec.SetVec(nullptr, 0);

    return result;
}

std::string Context::SHA256File(const std::string& path) const
{
    std::ifstream file(path);
    const int buffer_size = 4096;
    char digest[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;

    if (!file.good())
        return nullptr;

    spdlog::get("console")->debug("Generating digest for file '{}'", path);

    auto buffer = new char[buffer_size];

    SHA256_Init(&sha256);

    // Begin digesting bits of the file.
    while (!file.eof())
    {
        file.read(buffer, buffer_size);

        if (file.bad())
            throw RuntimeError("Read error when trying to generate digest");

        SHA256_Update(&sha256, buffer, file.gcount());
    }

    delete[] buffer;
    SHA256_Final(reinterpret_cast<unsigned char*>(digest), &sha256);

    return { digest, sizeof digest };
}
