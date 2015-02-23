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

#include "OFN/Puzzle.h"

namespace SQLite3
{
class Connection;
}

namespace OFN
{

class RuntimeError : public std::runtime_error
{
public:
    RuntimeError(const std::string& what_arg) : std::runtime_error(what_arg) {}
};

class TransactionError : public RuntimeError
{
public:
    TransactionError(const std::string& what_arg) : RuntimeError(what_arg) {}
};

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
    void Commit(const std::shared_ptr<Image>& image);

    /**
     * Search for similar images in the database.
     */
    void Search(const std::shared_ptr<Image>& image);

    /**
     * @brief Save the image to the database.
     *
     * @returns The inserted row ID on success, -1 otherwise.
     */
    int SaveImage(const std::shared_ptr<Image>& image);

    /**
     * @brief Save the image signature to the database.
     *
     * @returns The inserted row ID on success, -1 otherwise.
     */
    int SaveImageSignature(const std::shared_ptr<Image>& image, int image_id);

    /**
     * @brief Save the image words compressed to the database.
     */
    bool SaveImageWords(const std::shared_ptr<Image>& image, int image_id,
                        int signature_id);

    /**
     * @brief Compress a list of words using PuzzleCompressedCvec.
     */
    std::vector<std::string> CompressWords(const StringVector& words) const;


public:
    /* Getters */
    std::shared_ptr<Puzzle::Context> GetPuzzleContext() const
    {
        return puzzle_;
    }

protected:
    /**
     * @brief Compute a SHA256 hash for a given file.
     *
     * @param string path to the file to generate a digest for
     *
     * @returns the hash as a string, but in raw form.
     */
    std::string SHA256File(const std::string& file) const;

private:
    std::shared_ptr<SQLite3::Connection> conn_;
    std::shared_ptr<Puzzle::Context> puzzle_;
};

}
