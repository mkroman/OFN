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
#include <sqlite3.h>

#include "spdlog/spdlog.h"
#include "SQLite3.h"

/**
 * Forward declarations.
 */
typedef struct sqlite3 sqlite3;
typedef struct sqlite3_stmt sqlite3_stmt;

namespace OFN
{

class Image;

class Database
{
public:
    /** Constructor. */
    Database(const std::string& path);

    /** Destructor. */
    ~Database();

    static void print_sqlite_trace(void* db, const char* zSql);

    /** Get the database path. */
    const std::string& GetPath() const;

    /** Prepare an SQL statement. */
    sqlite3_stmt* PrepareStatement(const std::string& sql) const;

    /** Execute SQL statement. */
    bool Execute(const std::string& sql) const;

    /** Get the last sqlite error message. */
    std::string GetErrorMessage() const;

    /** Get the last inserted row id. */
    sqlite3_int64 GetLastRowID();

private:
    sqlite3* db_;
    std::string db_path_;
};

}
