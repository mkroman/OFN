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

/**
 * @file CVec.h
 * @date 20 Feb 2015
 * @brief CVec class for libpuzzle.
 * @author Mikkel Kroman
 */

#include <string>
#include <memory>

extern "C" {
#include <puzzle.h>
}

namespace OFN
{
namespace Puzzle
{

/*
 * Forward declarations.
 */
class Context;
class CompressedCVec;

/**
 * CVec class.
 */
class CVec
{
public:
    /**
     * Construct an empty cvec.
     *
     * @param context a pointer to a puzzle context
     */
    CVec(std::shared_ptr<Context> context);

    /**
     * Construct a cvec and fill it with bitmap data from an image file.
     *
     * @param context a pointer to a puzzle context
     * @param file    a path to a valid image file
     */
    CVec(std::shared_ptr<Context> context, const std::string& file);

    /**
     * Destruct the cvec.
     */
    ~CVec();

    /**
     * Fill a cvec with bitmap data from an image file.
     *
     * @param file a path to a valid image file
     *
     * @returns true on success, false otherwise.
     */
    bool LoadFile(const std::string& file);

    /**
     * Compress the vec and return a new compressed cvec.
     *
     * @return a pointer to a new compressed cvec.
     */
    std::unique_ptr<CompressedCVec> Compress() const;

    /**
     * Set the vec buffer.
     *
     * @param vec the new vec buffer
     */
    void SetVec(signed char* vec);

    /**
     * Set the vec buffer to point to a string to use as buffer.
     *
     * @param string the string to use as buffer
     */
    void SetVec(const std::string& string);

    /**
     * Get a pointer to the vec buffer.
     *
     * @return a pointer to the vec buffer.
     */
    signed char* GetVec() const
    {
        return cvec_.vec;
    }

    /**
     * Get a raw pointer to the cvec struct.
     *
     * @return a raw pointer to the cvec struct.
     */
    const PuzzleCvec* GetCvec() const
    {
        return &cvec_;
    }

    /**
     * Get the cvec buffer size.
     *
     * @return the cvec buffer size.
     */
    size_t GetSize() const
    {
        return cvec_.sizeof_vec;
    }

    /**
     * Get the raw puzzle C context.
     *
     * @return a pointer to the puzzle C context.
     */
    //PuzzleContext* GetPuzzleContext() const;

private:
    PuzzleCvec cvec_;
    std::shared_ptr<Context> context_;
};

}
}

