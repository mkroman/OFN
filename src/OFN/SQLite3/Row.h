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
 * @file Row.h
 * @author Mikkel Kroman
 * @date 24 Feb 2015
 * @brief Row class for prepared statements.
 */

#include <cstddef>
#include <sqlite3.h>

namespace OFN
{
namespace SQLite3
{

/**
 * +Data class.
 */
class Data
{
public:
    Data(const char* data, size_t size) :
        size_(size),
        data_(data)
    {
    }

    Data(Data&& data) :
        size_(data.size_),
        data_(data.data_)
    {
    }

    const char* data() const
    {
        return data_;
    }

    size_t size() const
    {
        return size_;
    }
    
private:
    size_t size_;
    const char* data_;
};

/**
 * +Row class.
 */
class Row
{
public:
    Row() :
        stmt_(nullptr) 
    {
    }

    /**
     * Constructor from a sqlite3 statement pointer.
     */
    Row(sqlite3_stmt* stmt) :
        stmt_(stmt),
        num_columns_(sqlite3_column_count(stmt))
    {
    }

    /**
     * Move constructor.
     */
    Row(Row&& row) :
        stmt_(row.stmt_),
        num_columns_(row.num_columns_)
    {
    }

    /**
     * Support the bool operator.
     */
    operator bool() const
    {
        return stmt_ != nullptr;
    }

    /**
     * Get the column type.
     */
    int GetType(int index) const
    {
        return sqlite3_column_type(stmt_, index);
    }

    /**
     * Get the value of a column as an integer.
     */
    int GetInt(int index) const
    {
        return sqlite3_column_int(stmt_, index);
    }

    /**
     * Get the value of a column as a 64-bit integer.
     */
    int GetInt64(int index) const
    {
        return sqlite3_column_int64(stmt_, index);
    }

    /**
     * Get the value of a column as a string.
     */
    Data GetText(int index) const
    {
        auto size = sqlite3_column_bytes(stmt_, index);
        auto text = sqlite3_column_text(stmt_, index);

        return Data(reinterpret_cast<const char*>(text), size);
    }

    /**
     * Get the value of a column as a blob.
     */
    Data GetBlob(int index) const
    {
        auto size = sqlite3_column_bytes(stmt_, index);
        auto blob = sqlite3_column_blob(stmt_, index);

        return Data(reinterpret_cast<const char*>(blob), size);
    }

private:
    sqlite3_stmt* stmt_;
    int num_columns_;
};

}
}
