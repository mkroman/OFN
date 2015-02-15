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

std::vector<Image::Word> Image::GetWords() const
{
    std::vector<Image::Word> words;
    words.reserve(MAX_WORDS);

    for (int i = 0; i < MAX_WORDS; i++)
    {
        assert(cvec_->sizeof_vec > static_cast<size_t>(100 + i));

        Word w = {
            .word = reinterpret_cast<char*>(&cvec_->vec[i]), 
            .size = MAX_WORD_LENGTH
        };

        words.push_back(w);
    }

    return words;
}

std::vector<Image::CompressedWord> Image::GetCompressedWords() const
{
    PuzzleCvec cvec;
    PuzzleContext* puzzle_ctx = context_->GetPuzzleContext();
    PuzzleCompressedCvec cvec_compressed;
    std::vector<Image::CompressedWord> words;
    CompressedWord w = {{0}, 0};

    assert(cvec_->sizeof_vec >
           static_cast<size_t>(MAX_WORDS + MAX_WORD_LENGTH));

    words.reserve(MAX_WORDS);
    puzzle_init_cvec(puzzle_ctx, &cvec);
    puzzle_init_compressed_cvec(puzzle_ctx, &cvec_compressed);

    for (int i = 0; i < MAX_WORDS; i++)
    {
        cvec.vec = &cvec_->vec[i];
        cvec.sizeof_vec = MAX_WORD_LENGTH;

        // Compress the cvec
        puzzle_compress_cvec(puzzle_ctx, &cvec_compressed, &cvec);

        // Copy the compressed cvec to our word
        memcpy(&w.word, cvec_compressed.vec,
               cvec_compressed.sizeof_compressed_vec);

        w.size = cvec_compressed.sizeof_compressed_vec;

        // Add the word
        words.push_back(w);

        // Clear up allocated resources
        puzzle_free_compressed_cvec(puzzle_ctx, &cvec_compressed);
    }

    return words;
}
