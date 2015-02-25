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
 * @file Statement.h
 * @author Mikkel Kroman
 * @date 24 Feb 2015
 * @brief Prepared statement class.
 */

#include <sqlite3.h>

#include "OFN/SQLite3/Connection.h"
#include "OFN/SQLite3/Row.h"

namespace OFN
{
namespace SQLite3
{

class Statement
{
public:
    /**
     * Construct a prepared statment from a connection and a SQL statment.
     *
     * @param connection the database connection
     * @param sql        the SQL statement
     *
     * @throws ConnectionError if prepaing the statment fails.
     */
    Statement(const Connection& connection, const std::string& sql) :
        state_(0),
        stmt_(nullptr)
    {
        sqlite3_stmt* result;

        if (sqlite3_prepare_v2(connection.GetHandle(), sql.c_str(), sql.size(),
                               &result, NULL) == SQLITE_OK)
            stmt_ = result;
    }

    /**
     * Construct a prepared statement with a pointer to a raw already-prepared
     * statement.
     *
     * @param stmt raw pointer to a sqlite3 statement type
     */
    Statement(sqlite3_stmt* const stmt) :
        state_(0),
        stmt_(stmt)
    {
    }

    /**
     * Destruct a prepared statement.
     */
    ~Statement()
    {
        if (stmt_ != nullptr)
            sqlite3_finalize(stmt_);
    }
    
    /**
     * Bool operator for checking if the preparation failed.
     */
    operator bool() const
    {
        return stmt_ != nullptr;
    }

    /**
     * Bind a parameter to the prepared statement.
     *
     * @param index the parameter index
     * @param value the value
     */
    template <typename T>
    int Bind(int index, const T& value);

    /**
     * Bind a parameter with a specific size to the prepared statement.
     *
     * This is useful for binding blobs or other large objects.
     *
     * @param index the parameter index
     * @param value a pointer to the data to bind
     * @param size  the size of value in bytes
     * @param transient true if sqlite should keep its own copy of value, this
     * is needed if the content might change before the statement is evaluated.
     *
     * @returns SQLITE_OK on success, or an SQLite error code otherwise.
     */
    int Bind(int index, const void* value, size_t size, bool transient = false)
    {
        return sqlite3_bind_blob(stmt_, index, value, size,
                                 transient ? SQLITE_TRANSIENT : nullptr);
    }
    
    /**
     * Evaluate the prepared statement.
     *
     * @returns a row.
     */
    Row Step()
    {
        state_ = sqlite3_step(stmt_);

        if (state_ == SQLITE_ROW)
            return Row(stmt_);
        else
            return Row();
    }

    /**
     * Reset the prepared statement.
     *
     * @returns SQLITE_OK if the previous invocation of the prepared statement
     * was successful, otherwise it returns a SQLite error code.
     */
    int Reset()
    {
        return sqlite3_reset(stmt_);
    }

    /**
     * Get a raw pointer to the statement.
     *
     * @returns a raw pointer to the prepared statement.
     */
    sqlite3_stmt* GetStatement() const
    {
        return stmt_;
    }

    bool IsDone() const
    {
        return (state_ == SQLITE_DONE);
    }

    bool HasRow() const
    {
        return (state_ == SQLITE_ROW);
    }

protected:
    int state_;
    sqlite3_stmt* stmt_;
};

}
}
