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

#include <memory>

#include "OFN/Puzzle/CVec.h"
#include "OFN/Puzzle/Context.h"
#include "OFN/Puzzle/CompressedCVec.h"

using namespace OFN::Puzzle;

CompressedCVec::CompressedCVec(std::shared_ptr<Context> context) :
    context_(context)
{
    puzzle_init_compressed_cvec(context_->GetPuzzleContext(), &cvec_);
}

CompressedCVec::CompressedCVec(std::shared_ptr<Context> context, char* vec,
                               size_t size) :
    context_(context)
{
    puzzle_init_compressed_cvec(context_->GetPuzzleContext(), &cvec_);

    cvec_.vec = reinterpret_cast<unsigned char*>(vec);
    cvec_.sizeof_compressed_vec = size;
}

CompressedCVec::~CompressedCVec()
{
    puzzle_free_compressed_cvec(context_->GetPuzzleContext(), &cvec_);
}

std::unique_ptr<CVec> CompressedCVec::Uncompress() const
{
    auto cvec = std::make_unique<CVec>(context_);
    auto puzzle_ctx = context_->GetPuzzleContext();

    if (puzzle_uncompress_cvec(puzzle_ctx, &cvec_, &cvec->cvec_) != 0)
        return nullptr;

    return cvec;
}

PuzzleContext* CompressedCVec::GetPuzzleContext()
{
    return context_->GetPuzzleContext();
}

