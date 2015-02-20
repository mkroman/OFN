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

#pragma once

#include <string>
#include <vector>
#include <memory>

#include <sqlite3.h>

typedef struct PuzzleContext_ PuzzleContext;

namespace OFN
{

/* Forward declarations. */
class Image;
class Database;

/**
 * %Context class.
 */
class Context
{
public:
    /**
     * Constructor.
     */
    Context();

    /**
     * Destructor.
     */
    ~Context();

public:
    using StringVector = std::vector<std::string>;

    /**
     * Commit a new image and its fingerprint to the database.
     */
    void Commit(Image* image);

    /**
     * Search for similar images in the database.
     */
    void Search(Image* image);

    /**
     * @brief Save the image to the database.
     *
     * @returns The inserted row ID on success, -1 otherwise.
     */
    sqlite3_int64 SaveImage(const Image* image);

    /**
     * @brief Save the image signature to the database.
     *
     * @returns The inserted row ID on success, -1 otherwise.
     */
    sqlite3_int64 SaveImageSignature(const Image* image, sqlite3_int64 image_id);

    /**
     * @brief Save the image words compressed to the database.
     */
    bool SaveImageWords(const Image* image, sqlite3_int64 image_id,
                        sqlite3_int64 signature_id);

    /**
     * @brief Compress a list of words using PuzzleCompressedCvec.
     */
    StringVector CompressWords(const StringVector& words) const;

public:
    /** Getters */
    PuzzleContext* GetPuzzleContext() const { return puzzle_; }

private:
    int error_;
    std::shared_ptr<Database> db_;
    PuzzleContext* puzzle_;
};

}
