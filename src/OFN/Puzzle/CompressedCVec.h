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
 * @file CompressedCVec.h
 * @date 20 Feb 2015
 * @brief CompressedCVec class for libpuzzle.
 * @author Mikkel Kroman
 */

#include <memory>

extern "C" {
#include "puzzle.h"
}

#include "OFN/Puzzle/CVec.h"

namespace OFN
{
namespace Puzzle
{

/*
 * Forward declarations.
 */
class Context;

/**
 * CompressedCVec class.
 */
class CompressedCVec
{
    friend class CVec;
public:
    /**
     * Construct a new compressed cvec.
     *
     * @param context a pointer to a puzzle context
     */
    CompressedCVec(std::shared_ptr<Context> context);

    /**
     * Construct a compressed cvec with a vec buffer and size.
     */
    CompressedCVec(std::shared_ptr<Context> context, char* vec, size_t size);

    /**
     * Destruct a compressed cvec.
     */
    ~CompressedCVec();

    /**
     * Uncompress[sic] the compressed vector.
     */
    std::unique_ptr<CVec> Uncompress() const;

    /**
     * Get a pointer to the vec buffer.
     *
     * @return a pointer to the vec buffer.
     */
    unsigned char* GetVec() const
    {
        return cvec_.vec;
    }

    /**
     * Get the size of the compressed vec.
     *
     * @return the size of the compressed vec.
     */
    size_t GetSize() const
    {
        return cvec_.sizeof_compressed_vec;
    }

    /**
     * Get a raw pointer to the compressed libpuzzle C cvec.
     *
     * @return a pointer to the cvec.
     */
    PuzzleCompressedCvec* GetCvec()
    {
        return &cvec_;
    }

    /**
     * Set the vector buffer.
     */
    void SetVec(char* vec, size_t size)
    {
        cvec_.vec = reinterpret_cast<unsigned char*>(vec);
        cvec_.sizeof_compressed_vec = size;
    }

protected:
    /**
     * Get the raw puzzle context.
     *
     * @return a pointer to the raw puzzle C context.
     */
    inline PuzzleContext* GetPuzzleContext();

private:
    PuzzleCompressedCvec cvec_;
    std::shared_ptr<Context> context_;
};

}
}
