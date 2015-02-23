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

#include "OFN/Puzzle.h"
#include "OFN/SQLite3.h"
#include "OFN/OFN.h"
#include "OFN/Image.h"
#include "OFN/Context.h"
#include "OFN/Database.h"

using namespace OFN;

static void print_sqlite_trace(void* data, const char* sql)
{
    auto console = spdlog::get("console");
    auto context = reinterpret_cast<Context*>(data);

    SPDLOG_TRACE(console, "SQL: {}", sql);
}

Context::Context() :
    conn_(std::make_shared<SQLite3::Connection>("ofn.db")),
    puzzle_(std::make_shared<Puzzle::Context>())
{
    conn_->SetTrace(Database::print_sqlite_trace);
}

Context::~Context()
{
}

void Context::Search(const std::shared_ptr<Image>& image)
{
    auto console = spdlog::get("console");
    auto statement = conn_->PrepareStatement(
        "SELECT DISTINCT(signature_id) FROM words "
        "WHERE pos_and_word IN (?, ?, ?, ?, ?, ?, ?, ?, "
        "?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, "
        "?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, "
        "?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, "
        "?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, "
        "?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
    auto words = CompressWords(image->GetWords());

    for (auto i = 0; i < Image::MAX_WORDS; i++)
    {
        auto word = words[i];

        statement->Bind(i + 1, word.data(), word.size(), true);
    }

    auto result = statement->Step();

    if (result == SQLITE_ERROR)
    {
        SPDLOG_TRACE(console, "Step() returned SQLITE_ERROR")
    }
    else if (result == SQLITE_ROW)
    {
        auto ptr = statement->GetStatement();
        auto signature_id = sqlite3_column_int(ptr, 0);

        console->info("Got row: {:d}", signature_id);
    }
    else if (result == SQLITE_DONE)
    {
        console->debug("No results");
    }
}

void Context::Commit(const std::shared_ptr<Image>& image)
{
    auto console = spdlog::get("console");
    int image_id, signature_id;

    if ((image_id = SaveImage(image)) == -1)
    {
        Console->error("SaveImage returned -1");
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

int Context::SaveImage(const std::shared_ptr<Image>& image)
{
    auto console = spdlog::get("console");

    try
    {
        auto stmt = conn_->PrepareStatement(
            "INSERT INTO images (filename, digest) VALUES (?, ?)");

        auto digest = SHA256File(image->GetFileName());

        stmt->Bind(1, image->GetFileName());
        stmt->Bind(2, digest.data(), digest.size());

        if (stmt->Step() != SQLITE_DONE)
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

int Context::SaveImageSignature(const std::shared_ptr<Image>& image,
                                int image_id)
{
    try
    {
        auto stmt =
            conn_->PrepareStatement("INSERT INTO signatures (image_id, "
                                    "compressed_signature) VALUES(?, ?)");

        auto cvec = image->GetCvec();
        auto compressed = cvec->Compress();

        stmt->Bind(1, image_id);
        stmt->Bind(2, compressed->GetVec(), compressed->GetSize());

        if (stmt->Step() != SQLITE_DONE)
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

bool Context::SaveImageWords(const std::shared_ptr<Image>& image, int image_id,
                             int signature_id)
{
    (void)image_id;
    auto stmt = conn_->PrepareStatement(
        "INSERT INTO words (signature_id, pos_and_word) VALUES(?, ?)");

    auto words = CompressWords(image->GetWords());

    for (auto word : words)
    {
        stmt->Reset();
        stmt->Bind(1, signature_id);
        stmt->Bind(2, word.data(), word.size());

        if (stmt->Step() != SQLITE_DONE)
        {
            Console->error("Failed to insert word: {}", conn_->GetErrorMessage());
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
    signed char* orig = cvec.GetVec();

    for (auto word : words)
    {
        cvec.SetVec(word);
        auto compr_cvec = cvec.Compress();

        result.emplace_back(reinterpret_cast<char*>(compr_cvec->GetVec()),
                            compr_cvec->GetSize());

        // puzzle_free_compressed_cvec(puzzle_->GetPuzzleContext(),
        //                             compr_cvec.GetCvec());
    }

    cvec.SetVec(orig);

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
    return std::string(digest, sizeof digest);
}
