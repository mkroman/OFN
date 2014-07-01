#pragma once
#ifndef __OFN_H
#define __OFN_H

#include <puzzle.h>

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
