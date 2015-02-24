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
 * @file Connection.h
 * @author Mikkel Kroman
 * @date 24 Feb 2015
 * @brief Connection class for the SQLite3 C++ wrapper.
 */

#include <string>
#include <memory>
#include <sqlite3.h>

#include "OFN/SQLite3/Errors.h"

namespace OFN
{
namespace SQLite3
{

class Statement;

/**
 * +Connection class.
 */
class Connection
{
public:
    /**
     * Construct a new connection.
     *
     * @param filename the database filename
     *
     * @throws ConnectionError if `sqlite3_open` fails.
     */
    Connection(const std::string& filename) :
        db_(nullptr)
    {
        sqlite3* result;

        if (sqlite3_open(filename.c_str(), &result) != SQLITE_OK)
            throw ConnectionError("Failed to open database");

        db_ = result;
    }

    /**
     * Destruct a connection.
     *
     * Closes the SQLite database.
     */
    ~Connection()
    {
        if (db_ != nullptr)
            sqlite3_close(db_);
    }

    /**
     * Get the raw sqlite3 handle.
     *
     * @returns a raw pointer to the sqlite3 opaque handle.
     */
    sqlite3* GetHandle() const
    {
        return db_;
    }

    using TraceFunc = void (*)(void*, const char*);

    /**
     * Set a trace function for the sqlite3 database.
     */
    void SetTrace(TraceFunc f)
    {
        sqlite3_trace(db_, f, this);
    }

    /**
     * Prepare a SQL statement.
     *
     * @param sql the sql statement, utf-8 encoded.
     *
     * @returns a pointer to a prepared statement.
     */
    std::unique_ptr<Statement> Prepare(const std::string& sql)
    {
        sqlite3_stmt* result;

        if (sqlite3_prepare_v2(db_, sql.c_str(), sql.size(), &result, NULL) !=
            SQLITE_OK)
            return nullptr;

        return std::make_unique<Statement>(result);
    }

    /**
     * Prepare and execute a single SQL statement.
     */
    auto Execute(const std::string& statement)
    {
        return sqlite3_exec(db_, statement.c_str(), NULL, NULL, NULL);
    }

    /**
     * Get the last error message.
     *
     * @returns the last error message.
     */
    std::string GetErrorMessage() const
    {
        return std::string(sqlite3_errmsg(db_));
    }

    /**
     * Get the last inserted row id.
     *
     * @returns the last inserted row id.
     */
    unsigned int GetLastInsertRowID() const
    {
        return sqlite3_last_insert_rowid(db_);
    }

private:
    sqlite3* db_;
};

}
}
