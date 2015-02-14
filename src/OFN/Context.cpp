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

#include <cstdio>
#include <cstddef>

#include "OFN/Image.h"
#include "OFN/Context.h"

extern "C" {
# include <puzzle.h>
}

using namespace OFN;

Context::Context()
{
    puzzle_ = new PuzzleContext;
    puzzle_init_context(puzzle_);
}

Context::~Context()
{
    puzzle_free_context(puzzle_);
    delete puzzle_;
}

void Context::Search(Image* image)
{
    printf("Searching for images similar to %s\n",
           image->GetFileName().c_str());
}

void Context::Commit(Image* image)
{
    (void)image;
}

