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

#include <memory>

#include "Puzzle/Errors.h"
#include "Puzzle/Context.h"
#include "Puzzle/CompressedCVec.h"

namespace OFN
{
namespace Puzzle
{

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
    CVec(std::shared_ptr<Context> context) :
        context_(context)
    {
        puzzle_init_cvec(GetPuzzleContext(), &cvec_);
    }

    /**
     * Construct a cvec and fill it with bitmap data from an image file.
     *
     * @param context a pointer to a puzzle context
     * @param file    a path to a valid image file
     */
    CVec(std::shared_ptr<Context> context, const std::string& file) :
        context_(context)
    {
        puzzle_init_cvec(GetPuzzleContext(), &cvec_);
        
        if (!LoadFile(file))
            throw BitmapLoadError("Could not load bitmap file");
    }

    /**
     * Destruct the cvec.
     */
    ~CVec()
    {
        puzzle_free_cvec(GetPuzzleContext(), &cvec_);
    }

    /**
     * Fill a cvec with bitmap data from an image file.
     *
     * @param file a path to a valid image file
     *
     * @returns true on success, false otherwise.
     */
    bool LoadFile(const std::string& file)
    {
        return (puzzle_fill_cvec_from_file(GetPuzzleContext(), &cvec_,
                                           file.c_str()) == 0);
    }

    /**
     * Compress the vec and return a new compressed cvec.
     *
     * @return a pointer to a new compressed cvec.
     */
    std::unique_ptr<CompressedCVec> Compress() const
    {
        auto cvec = std::make_unique<CompressedCVec>(context_);

        if (puzzle_compress_cvec(GetPuzzleContext(), cvec->GetCvec(), &cvec_) !=
            0)
            return nullptr;

        return cvec;
    }

    /**
     * Set the vec buffer.
     *
     * @param vec the new vec buffer
     */
    void SetVec(signed char* vec)
    {
        cvec_.vec = vec;
    }

    /**
     * Set the vec buffer to point to a string to use as buffer.
     *
     * @param string the string to use as buffer
     */
    void SetVec(const std::string& string)
    {
        cvec_.vec =
            reinterpret_cast<signed char*>(const_cast<char*>(string.data()));

        cvec_.sizeof_vec = string.size();
    }

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
    PuzzleContext* GetPuzzleContext() const
    {
        return context_->GetPuzzleContext();
    }

private:
    PuzzleCvec cvec_;
    std::shared_ptr<Context> context_;
};

}
}
