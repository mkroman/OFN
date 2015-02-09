/*
 * Copyright (C) 2015 Mikkel Kroman <mk@maero.dk>
 * 
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
 * OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#pragma once
#ifndef OFN_H
#define OFN_H

#include <puzzle.h>

#define OFN_VERSION "v0.1"

static PuzzleContext ofn_puzzle_context;
static PuzzleCvec ofn_puzzle_cvec;

static const int SIGNATURE_SIZE = 544;

// Initialize the ofn database and puzzle context.
int ofn_init();

// Deinitialize the database and puzzle context.
int ofn_close();

// Commit a new image file to the database.
int ofn_commit(const char* filename);

// Search for an image.
// Returns 1 if the image was found, 0 otherwise.
int ofn_search(const char* filename);

// Get a fingerprint of a vector.
void ofn_fingerprint(PuzzleCvec* cvec, char* buffer);

#endif
