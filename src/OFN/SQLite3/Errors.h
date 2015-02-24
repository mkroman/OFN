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
 * @author Mikkel Kroman
 * @date 24 Feb 2015
 * @brief Runtime error classes for the sqlite3 wrapper.
 */

#include <string>
#include <stdexcept>

namespace OFN
{
namespace SQLite3
{

/**
 * Generic +SQLiteError class.
 *
 * You can use this for catching all SQLite-related exceptions, as they all
 * derive from this class.
 */
class SQLiteError : public std::runtime_error
{
public:
    SQLiteError(const std::string& what_arg) :
        std::runtime_error(what_arg)
    {
    }
};

/**
 * +ConnectionError class.
 *
 * Thrown when there is a connection problem with the SQLite database.
 */
class ConnectionError : public SQLiteError
{
public:
    ConnectionError(const std::string& what_arg) : SQLiteError(what_arg) {}
};

}
}
