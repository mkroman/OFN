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

#include "OFN/OFN.h"
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

void Context::Search(std::shared_ptr<Image> image)
{
    Console->info("Searching for images similar to {}", image->GetFileName());
}

void Context::Commit(std::shared_ptr<Image> image)
{
    sqlite3_int64 image_id, signature_id;

    Console->debug("Comitting image {}", image->GetFileName());

    if ((image_id = SaveImage(image)) == -1)
    {
        Console->error("SaveImage returned -1");
        return;
    }

    if ((signature_id = SaveImageSignature(image, image_id)) == -1)
    {
        Console->error("SaveImageSignature returned -1");
        return;
    }

    if (!db_->Execute("BEGIN TRANSACTION"))
        return;

    if (!SaveImageWords(image, image_id, signature_id))
    {
        Console->error("SaveImageWords failed");
    }

    db_->Execute("END TRANSACTION");
}

sqlite3_int64 Context::SaveImage(std::shared_ptr<Image> image)
{
    auto stmt = db_->PrepareStatement(
        "INSERT INTO images (filename, digest) VALUES (?, ?)");

    if (stmt == nullptr)
        return -1;

    sqlite3_bind_text(stmt, 1, image->GetFileName().c_str(),
                      image->GetFileName().size(), NULL);
    sqlite3_bind_text(stmt, 2, "111", 4, NULL);

    if (sqlite3_step(stmt) != SQLITE_DONE)
    {
        Console->error("{} failed: {}", __FUNCTION__, db_->GetErrorMessage());

        sqlite3_finalize(stmt);

        return -1;
    }

    sqlite3_finalize(stmt);

    return db_->GetLastRowID();
}

sqlite3_int64 Context::SaveImageSignature(std::shared_ptr<Image> image,
                                          sqlite3_int64 image_id)
{
    auto stmt = db_->PrepareStatement(
        "INSERT INTO signatures (image_id, compressed_signature) VALUES(?, ?)");

    if (stmt == nullptr)
        return -1;

    PuzzleCompressedCvec ccvec;
    auto source = image->GetCvec();

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

bool Context::SaveImageWords(std::shared_ptr<Image> image,
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
    PuzzleCvec cvec;
    PuzzleCompressedCvec compressed_cvec;
    std::vector<std::string> result;

    result.reserve(words.size());
    puzzle_init_cvec(puzzle_, &cvec);
    puzzle_init_compressed_cvec(puzzle_, &compressed_cvec);

    for (auto word : words)
    {
        cvec.vec =
            reinterpret_cast<signed char*>(const_cast<char*>(word.data()));
        cvec.sizeof_vec = word.size();

        assert(puzzle_compress_cvec(puzzle_, &compressed_cvec, &cvec) == 0);

        result.push_back(
            std::string(reinterpret_cast<char*>(compressed_cvec.vec),
                        compressed_cvec.sizeof_compressed_vec));

        puzzle_free_compressed_cvec(puzzle_, &compressed_cvec);
    }

    return result;
}
