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

#include <cstddef>
#include <cassert>
#include <cstring>

#include "OFN/Puzzle.h"
#include "OFN/Image.h"
#include "OFN/Context.h"

extern "C" {
# include <puzzle.h>
}

using namespace OFN;

Image::Image(std::shared_ptr<Context> context, const std::string& filename) :
    context_(context),
    file_name_(filename)
{
    auto ctx = context->GetPuzzleContext();
    cvec_ = new PuzzleCvec;

    // Initialize and load the image as a cvec.
    puzzle_init_cvec(ctx, cvec_);

    if (puzzle_fill_cvec_from_file(ctx, cvec_, filename.c_str()) != 0)
        throw PuzzleException("Could not fill cvec from file");
}

Image::~Image()
{
    puzzle_free_cvec(context_->GetPuzzleContext(), cvec_);
    delete cvec_;
}

std::vector<std::string> Image::GetWords() const
{
    std::vector<std::string> words;
    words.reserve(MAX_WORDS);

    assert(cvec_->sizeof_vec >
           static_cast<size_t>(MAX_WORDS + MAX_WORD_LENGTH));

    for (int i = 0; i < MAX_WORDS; i++)
    {
        words.push_back(std::string(reinterpret_cast<char*>(&cvec_->vec[i]),
                                    MAX_WORD_LENGTH));
    }

    return words;
}

