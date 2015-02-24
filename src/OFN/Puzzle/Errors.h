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

/**
 * @file Errors.h
 * @date 24 Feb 2015
 * @brief Error classes for libpuzzle. 
 * @author Mikkel Kroman
 */

#include <string>
#include <stdexcept>

namespace OFN
{
namespace Puzzle
{


/**
 * Runtime error class.
 */
class RuntimeError : public std::runtime_error
{
public:
    /**
     * Construct a new runtime error with a +what+ argument.
     */
    RuntimeError(const std::string& what_arg) : std::runtime_error(what_arg) {}
};

/**
 * Bitmap load error class.
 */
class BitmapLoadError : public RuntimeError
{
public:
    /**
     * Construct a new bitmap load error with a +what+ argument.
     */
    BitmapLoadError(const std::string& what_arg) : RuntimeError(what_arg) {}
};



}
}
