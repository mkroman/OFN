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

/**
 * @file Puzzle.h
 * @date 20 Feb 2015
 * @brief C++ wrapper for libpuzzle.
 * @details This is a C++ wrapper for libpuzzle.
 * @author Mikkel Kroman <mk@maero.dk>
 */

#pragma once

extern "C" {
# include <puzzle.h>
}

#include "Puzzle/Errors.h"
#include "Puzzle/Context.h"
#include "Puzzle/CVec.h"
#include "Puzzle/CompressedCVec.h"

namespace OFN
{ 
namespace Puzzle
{


}
}
