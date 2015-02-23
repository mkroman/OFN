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
 * @file SQLite3.h
 * @author Mikkel Kroman <mk@maero.dk>
 * @date 22 Feb 2015
 * @brief C++ wrapper for the SQLite3 library.
 */

#pragma once

#include <string>
#include <memory>
#include <stdexcept>
#include <sqlite3.h>

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

/*
 * Forward declarations.
 */
class PreparedStatement;

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
    std::unique_ptr<PreparedStatement> PrepareStatement(const std::string& sql)
    {
        sqlite3_stmt* result;

        if (sqlite3_prepare_v2(db_, sql.c_str(), sql.size(), &result, NULL) !=
            SQLITE_OK)
        {
            throw ConnectionError("SQL statement preperation failed");
        }

        return std::make_unique<PreparedStatement>(result);
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

template <class T>
struct Bindable;

class PreparedStatement
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
    PreparedStatement(const Connection& connection, const std::string& sql) :
        stmt_(nullptr)
    {
        sqlite3_stmt* result;

        if (sqlite3_prepare_v2(connection.GetHandle(), sql.c_str(), sql.size(),
                               &result, NULL) != SQLITE_OK)
        {
            throw ConnectionError("SQL statement preperation failed");
        }

        stmt_ = result;
    }

    /**
     * Construct a prepared statement with a pointer to a raw already-prepared
     * statement.
     *
     * @param stmt raw pointer to a sqlite3 statement type
     */
    PreparedStatement(sqlite3_stmt* const stmt) :
        stmt_(stmt)
    {
    }

    /**
     * Destruct a prepared statement.
     */
    ~PreparedStatement()
    {
        if (stmt_ != nullptr)
            sqlite3_finalize(stmt_);
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
     * @returns SQLITE_DONE on success, an error code otherwise.
     */
    int Step() const
    {
        return sqlite3_step(stmt_);
    }

    /**
     * Reset the prepared statement.
     *
     * @returns SQLITE_OK if the previous invocation of the prepared statement
     * was successful, otherwise it returns a SQLite error code.
     */
    int Reset() const
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

protected:
    sqlite3_stmt* stmt_;
};

}
