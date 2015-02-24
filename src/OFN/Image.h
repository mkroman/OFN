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

namespace OFN
{

class Context;

/**
 * Image class.
 */
class Image
{
public:
    static const int MAX_WORDS = 100;
    static const int MAX_WORD_LENGTH = 10;

public:
    /**
     * Constructor.
     * 
     * @param context The OFN context.
     * @param filename The image filename.
     */
    Image(std::shared_ptr<Context> context, const std::string& filename);

    /**
     * Destructor.
     */
    ~Image();

    /**
     * Get a list of words from the signature.
     */
    std::vector<std::string> GetWords() const;

    /**
     * Get the filename.
     *
     * @returns the filename.
     */
    const std::string& GetFileName() const
    {
        return file_name_;
    }

    /**
     * Get the Cvec.
     *
     * @returns the cvec.
     */
    auto GetCvec() {
        return cvec_;
    }

private:
    std::shared_ptr<Context> context_;
    std::string file_name_;
    Puzzle::CVec* cvec_;
};

}

