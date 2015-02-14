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

#include "OFN/Image.h"
#include "OFN/Context.h"

extern "C" {
# include <puzzle.h>
}

using namespace OFN;

Image::Image(const Context* context, const std::string& filename)
    : error_(0), context_(context), file_name_(filename)
{
    PuzzleContext* ctx = context->GetPuzzleContext();
    cvec_ = new PuzzleCvec;

    // Initialize and load the image as a cvec.
    puzzle_init_cvec(ctx, cvec_);

    if (puzzle_fill_cvec_from_file(ctx, cvec_, filename.c_str()) != 0)
        error_ = 1;
}

Image::~Image()
{
    puzzle_free_cvec(context_->GetPuzzleContext(), cvec_);
    delete cvec_;
}
