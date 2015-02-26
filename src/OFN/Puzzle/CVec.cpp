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

#include <string>
#include <memory>

#include "OFN/Puzzle/CVec.h"
#include "OFN/Puzzle/Errors.h"
#include "OFN/Puzzle/Context.h"
#include "OFN/Puzzle/CompressedCVec.h"

using namespace OFN::Puzzle;

CVec::CVec(std::shared_ptr<Context> context) :
    context_(context)
{
    puzzle_init_cvec(context_->GetPuzzleContext(), &cvec_);
}


CVec::CVec(std::shared_ptr<Context> context, const std::string& file) :
    context_(context)
{
    puzzle_init_cvec(context_->GetPuzzleContext(), &cvec_);
    
    if (!LoadFile(file))
        throw BitmapLoadError("Could not load bitmap file");
}

CVec::~CVec()
{
    puzzle_free_cvec(context_->GetPuzzleContext(), &cvec_);
}

bool CVec::LoadFile(const std::string& file)
{
    return (puzzle_fill_cvec_from_file(context_->GetPuzzleContext(), &cvec_,
                                       file.c_str()) == 0);
}

std::unique_ptr<CompressedCVec> CVec::Compress() const
{
    auto cvec = std::make_unique<CompressedCVec>(context_);
    auto puzzle_ctx = context_->GetPuzzleContext();

    if (puzzle_compress_cvec(puzzle_ctx, &cvec->cvec_, &cvec_) != 0)
        return nullptr;

    return cvec;
}

void CVec::SetVec(signed char* vec, size_t size)
{
    cvec_.vec = vec;
    cvec_.sizeof_vec = size;
}

void CVec::SetVec(const std::string& string)
{
    cvec_.vec =
        reinterpret_cast<signed char*>(const_cast<char*>(string.data()));

    cvec_.sizeof_vec = string.size();
}

double CVec::GetDistance(const CVec& other) const
{
    return puzzle_vector_normalized_distance(context_->GetPuzzleContext(),
                                             &cvec_, &other.cvec_, 0);
}

std::unique_ptr<CVec>
OFN::Puzzle::CVecFromCompressedBuffer(std::shared_ptr<Context>& context,
                                      const char* vector, size_t size)
{
    auto compressed = CompressedCVec{context, const_cast<char*>(vector), size};
    auto uncompressed = compressed.Uncompress();

    // Avoid having the destructor deallocate the buffer.
    compressed.SetVec(nullptr, 0);

    return uncompressed;
}

