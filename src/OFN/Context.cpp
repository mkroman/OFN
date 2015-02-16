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

extern "C" {
# include <puzzle.h>
}

#include "spdlog/spdlog.h"

#include "OFN/Image.h"
#include "OFN/Context.h"
#include "OFN/Database.h"

using namespace OFN;

Context::Context() :
    db_(std::make_shared<Database>("ofn.db"))
{
    puzzle_ = new PuzzleContext;
    puzzle_init_context(puzzle_);
}

Context::~Context()
{
    puzzle_free_context(puzzle_);
    delete puzzle_;
}

void Context::Search(Image* image)
{
    auto console = spdlog::stdout_logger_mt("console");

    console->info("Searching for images similar to {}", image->GetFileName());
}

void Context::Commit(Image* image)
{
    sqlite3_int64 image_id, signature_id;
    auto console = spdlog::stdout_logger_mt("console");

    console->debug("Comitting image {}\n", image->GetFileName());

    if ((image_id = SaveImage(*image)) == -1)
    {
        console->error("SaveImage returned -1");
        return;
    }

    if ((signature_id = SaveImageSignature(*image, image_id)) == -1)
    {
        console->error("SaveImageSignature returned -1");
        return;
    }

    if (!db_->Execute("BEGIN TRANSACTION"))
        return;

    if (!SaveImageWords(*image, image_id, signature_id))
    {
        console->error("SaveImageWords failed");
    }

    db_->Execute("END TRANSACTION");
}

sqlite3_int64 Context::SaveImage(const Image& image)
{
    auto console = spdlog::get("console");
    sqlite3_stmt* stmt = db_->PrepareStatement(
        "INSERT INTO images (filename, digest) VALUES (?, ?)");

    if (stmt == nullptr)
        return -1;

    sqlite3_bind_text(stmt, 1, image.GetFileName().c_str(),
                      image.GetFileName().size(), NULL);
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

sqlite3_int64 Context::SaveImageSignature(const Image& image, sqlite3_int64 image_id)
{
    auto console = spdlog::get("console");
    sqlite3_stmt* stmt = db_->PrepareStatement(
        "INSERT INTO signatures (image_id, compressed_signature) VALUES(?, ?)");

    if (stmt == nullptr)
        return -1;

    PuzzleCompressedCvec ccvec;
    const PuzzleCvec* source = image.GetCvec();

    puzzle_init_compressed_cvec(puzzle_, &ccvec);
    if (puzzle_compress_cvec(puzzle_, &ccvec, source) != 0)
        return -1;

    sqlite3_bind_int64(stmt, 1, image_id);
    sqlite3_bind_blob(stmt, 2, ccvec.vec, ccvec.sizeof_compressed_vec,
                      SQLITE_TRANSIENT);

    puzzle_free_compressed_cvec(puzzle_, &ccvec);

    if (sqlite3_step(stmt) != SQLITE_DONE)
    {
        sqlite3_finalize(stmt);
        return -1;
    }

    sqlite3_finalize(stmt);

    return db_->GetLastRowID();
}

bool Context::SaveImageWords(const Image& image, sqlite3_int64 image_id,
                             sqlite3_int64 signature_id)
{
    (void)image_id;
    auto console = spdlog::get("console");
    sqlite3_stmt* stmt = db_->PrepareStatement(
        "INSERT INTO words (signature_id, pos_and_word) VALUES(?, ?)");

    if (stmt == nullptr)
        return false;

    auto words = image.GetCompressedWords();
    
    for (auto word : words)
    {
        sqlite3_bind_int64(stmt, 1, signature_id);
        sqlite3_bind_blob(stmt, 2, word.word, word.size, NULL);

        if (sqlite3_step(stmt) != SQLITE_DONE)
        {
            console->error("Failed to insert word: {}", db_->GetErrorMessage());
        }
    }

    sqlite3_finalize(stmt);

    return true;
}
