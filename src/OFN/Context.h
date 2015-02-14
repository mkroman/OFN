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
struct PuzzleContext_;
typedef PuzzleContext_ PuzzleContext;
}

namespace OFN
{

/*
 * Forward declarations.
 */
class Image;

/**
 * OFN Context.
 */
class Context
{
public:
    /**
     * Constructor.
     */
    Context();

    /**
     * Destructor.
     */
    ~Context();

public:
    /**
     * Commit a new image and its fingerprint to the database.
     */
    void Commit(Image* image);

    /**
     * Search for similar images in the database.
     */
    void Search(Image* image);

public:
    /**
     * Getters.
     */
    PuzzleContext* GetPuzzleContext() const { return puzzle_; }

private:
    int error_;
    PuzzleContext* puzzle_;
};

}
