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

#include "OFN/Puzzle.h"

std::unique_ptr<Puzzle::CompressedCVec> Puzzle::CVec::Compress() const
{
    auto cvec = std::make_unique<Puzzle::CompressedCVec>(context_);

    if (puzzle_compress_cvec(GetPuzzleContext(), cvec->GetCvec(), &cvec_) != 0)
        return nullptr;

    return cvec;
}
