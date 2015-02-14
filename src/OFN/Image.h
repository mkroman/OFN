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

extern "C" {
# include <puzzle.h>
}

// struct PuzzleCvec_;
// typedef PuzzleCvec_ PuzzleCvec;
// typedef struct PuzzleCompressedCvec_ PuzzleCompressedCvec;

namespace OFN
{

class Context;

/**
 * Image class.
 */
class Image
{
public:
    class CompressedWordIterator
        : public std::iterator<std::input_iterator_tag, class T>
    {
        PuzzleCvec* cvec_;
        PuzzleContext* ctx_;
        PuzzleCompressedCvec* cvec_compressed_;
    public:
        CompressedWordIterator(PuzzleContext* ctx, PuzzleCvec* cvec)
            : cvec_(cvec), ctx_(ctx)
        {
            cvec_compressed_ = new PuzzleCompressedCvec;
            puzzle_init_compressed_cvec(ctx, cvec_compressed_);
        }

        CompressedWordIterator(const CompressedWordIterator& lvalue) :
            cvec_(lvalue.cvec_),
            ctx_(lvalue.ctx_),
            cvec_compressed_(lvalue.cvec_compressed_)
        {
        }

        ~CompressedWordIterator()
        {
            puzzle_free_compressed_cvec(ctx_, cvec_compressed_);
            delete cvec_compressed_;
        }
    };

public:
    /**
     * Constructor.
     * 
     * @param context The OFN context.
     * @param filename The image filename.
     */
    Image(const Context* context, const std::string& filename);

    /**
     * Destructor.
     */
    ~Image();

    /*
     * Get the error field. 
     * 
     * @returns The error field. Non-zero means there was an error.
     */
    int GetError() { return error_; }

    /**
     * Get the filename.
     *
     * @returns the filename.
     */
    const std::string& GetFileName() const { return file_name_; }

    /**
     * Get the Cvec.
     *
     * @returns the cvec.
     */
    const PuzzleCvec* GetCvec() const { return cvec_; }

private:
    int error_;
    const Context* context_;
    std::string file_name_;
    PuzzleCvec* cvec_;
};

}

