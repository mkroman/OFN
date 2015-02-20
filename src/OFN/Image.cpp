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

using namespace OFN;

Image::Image(std::shared_ptr<Context> context, const std::string& filename) :
    context_(context),
    file_name_(filename)
{
    cvec_ = new Puzzle::CVec(context->GetPuzzleContext(), filename);
}

Image::~Image()
{
    delete cvec_;
}

std::vector<std::string> Image::GetWords() const
{
    std::vector<std::string> words;
    words.reserve(MAX_WORDS);

    assert(cvec_->GetSize() > static_cast<size_t>(MAX_WORDS + MAX_WORD_LENGTH));

    for (int i = 0; i < MAX_WORDS; i++)
    {
        words.push_back(std::string(
            reinterpret_cast<char*>(&cvec_->GetVec()[i]), MAX_WORD_LENGTH));
    }

    return words;
}

