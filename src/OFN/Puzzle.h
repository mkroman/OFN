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

#include <stdexcept>
#include <memory>

extern "C" {
#include <puzzle.h>
}

namespace OFN
{

class PuzzleException : public std::runtime_error
{
public:
    PuzzleException(const std::string& what_arg) : 
        std::runtime_error(what_arg) {}
};

namespace Puzzle
{

class Context
{
public:
    /**
     * Construct the puzzle context.
     */
    Context()
    {
        puzzle_init_context(&ctx_);
    }

    /**
     * Destruct the puzzle context.
     */
    ~Context()
    {
        puzzle_free_context(&ctx_); 
    }

    /**
     * Return the puzzle context.
     */
    PuzzleContext* GetPuzzleContext()
    {
        return &ctx_;
    }

private:
    PuzzleContext ctx_;
};

class CVec
{
public:
    /**
     * Construct an empty cvec.
     */
    CVec(std::shared_ptr<Context> context) :
        context_(context)
    {
        puzzle_init_cvec(GetPuzzleContext(), &cvec_);
    }

    /**
     * Construct a cvec and fill it with bitmap data from an image file.
     */
    CVec(std::shared_ptr<Context> context, const std::string& file) :
        context_(context)
    {
        puzzle_init_cvec(GetPuzzleContext(), &cvec_);
        
        if (!LoadFile(file))
            throw PuzzleException("Could not load bitmap file");
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
     * @returns true on success, false otherwise.
     */
    bool LoadFile(const std::string& file)
    {
        return (puzzle_fill_cvec_from_file(GetPuzzleContext(), &cvec_,
                                           file.c_str()) == 0);
    }

    /**
     * Get the raw puzzle context.
     */
    inline PuzzleContext* GetPuzzleContext()
    {
        return context_->GetPuzzleContext();
    }

private:
    PuzzleCvec cvec_;
    std::shared_ptr<Context> context_;
};

}

}
