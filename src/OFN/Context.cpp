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

#include "OFN/Puzzle.h"
#include "OFN/OFN.h"
#include "OFN/Image.h"
#include "OFN/Context.h"
#include "OFN/Database.h"

using namespace OFN;

Context::Context() :
    db_(std::make_shared<Database>("ofn.db")),
    puzzle_(std::make_shared<Puzzle::Context>())
{
}

Context::~Context()
{
}

void Context::Search(const std::shared_ptr<Image>& image)
{
    auto console = spdlog::get("console");

    console->info("Searching for images similar to {}", image->GetFileName());
}

void Context::Commit(const std::shared_ptr<Image>& image)
{
    auto console = spdlog::get("console");
    sqlite3_int64 image_id, signature_id;

    console->debug("Comitting image {}", image->GetFileName());

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

    if (!db_->Execute("BEGIN TRANSACTION"))
        return;

    if (!SaveImageWords(image, image_id, signature_id))
    {
        console->error("SaveImageWords failed");
    }

    db_->Execute("END TRANSACTION");
}

sqlite3_int64 Context::SaveImage(const std::shared_ptr<Image>& image)
{
    auto console = spdlog::get("console");
    auto stmt = db_->PrepareStatement(
        "INSERT INTO images (filename, digest) VALUES (?, ?)");

    if (stmt == nullptr)
        return -1;

    sqlite3_bind_text(stmt, 1, image->GetFileName().c_str(),
                      image->GetFileName().size(), NULL);
    sqlite3_bind_text(stmt, 2, "111", 4, NULL);

    if (sqlite3_step(stmt) != SQLITE_DONE)
    {
        console->error("{} failed: {}", __FUNCTION__, db_->GetErrorMessage());

        sqlite3_finalize(stmt);

        return -1;
    }

    sqlite3_finalize(stmt);

    return db_->GetLastRowID();
}

sqlite3_int64 Context::SaveImageSignature(const std::shared_ptr<Image>& image,
                                          sqlite3_int64 image_id)
{
    auto stmt = db_->PrepareStatement(
        "INSERT INTO signatures (image_id, compressed_signature) VALUES(?, ?)");

    if (stmt == nullptr)
        return -1;

    auto cvec = image->GetCvec();
    auto compressed = cvec->Compress();

    sqlite3_bind_int64(stmt, 1, image_id);
    sqlite3_bind_blob(stmt, 2, compressed->GetVec(), compressed->GetSize(),
                      SQLITE_TRANSIENT);

    if (sqlite3_step(stmt) != SQLITE_DONE)
    {
        sqlite3_finalize(stmt);
        return -1;
    }

    sqlite3_finalize(stmt);

    return db_->GetLastRowID();
}

bool Context::SaveImageWords(const std::shared_ptr<Image>& image,
                             sqlite3_int64 image_id, sqlite3_int64 signature_id)
{
    (void)image_id;
    auto stmt = db_->PrepareStatement(
        "INSERT INTO words (signature_id, pos_and_word) VALUES(?, ?)");

    if (stmt == nullptr)
        return false;

    auto words = CompressWords(image->GetWords());

    for (auto word : words)
    {
        sqlite3_reset(stmt);
        sqlite3_bind_int64(stmt, 1, signature_id);
        sqlite3_bind_blob(stmt, 2, word.data(), word.size(), NULL);

        if (sqlite3_step(stmt) != SQLITE_DONE)
        {
            Console->error("Failed to insert word: {}", db_->GetErrorMessage());
        }
    }

    sqlite3_finalize(stmt);

    return true;
}

Context::StringVector
Context::CompressWords(const Context::StringVector& words) const
{
    Puzzle::CVec cvec(puzzle_);
    Puzzle::CompressedCVec compressed_cvec(puzzle_);
    std::vector<std::string> result;

    result.reserve(words.size());

    for (auto word : words)
    {
        cvec.SetVec(word);
        compressed_cvec.Compress(cvec);

        result.emplace_back(reinterpret_cast<char*>(compressed_cvec.GetVec()),
                            compressed_cvec.GetSize());

        puzzle_free_compressed_cvec(puzzle_->GetPuzzleContext(),
                                    compressed_cvec.GetCvec());
    }

    return result;
}
