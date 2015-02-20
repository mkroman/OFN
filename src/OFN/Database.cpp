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

#include <sqlite3.h>

#include "spdlog/spdlog.h"
#include "OFN/Database.h"
#include "OFN/Image.h"

using namespace OFN;

void Database::print_sqlite_trace(void* argument, const char* sql)
{
    auto console = spdlog::stdout_logger_mt("console");
    const Database* db = reinterpret_cast<Database*>(argument);

    console->info("({}) SQL: {}", db->GetPath(), sql);
}

Database::Database(const std::string& path) :
    db_path_(path)
{
    auto console = spdlog::stdout_logger_mt("console");

    if (sqlite3_open(path.c_str(), &db_) != SQLITE_OK)
    {
        console->error("Failed to open sqlite3 database: {}",
                       sqlite3_errmsg(db_));
    }

    sqlite3_trace(db_, print_sqlite_trace, this);
}

Database::~Database()
{
    auto console = spdlog::stdout_logger_mt("console");

    if (sqlite3_close(db_) != SQLITE_OK)
    {
        console->error("Failed to close sqlite3 database: {}",
                       sqlite3_errmsg(db_));
    }
}

const std::string& Database::GetPath() const
{
    return db_path_;
}

sqlite3_stmt* Database::PrepareStatement(const std::string& sql) const
{
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, NULL) != SQLITE_OK)
        return nullptr;

    return stmt;
}

std::string Database::GetErrorMessage() const
{
    return std::string(sqlite3_errmsg(db_));
}

sqlite3_int64 Database::GetLastRowID()
{
    return sqlite3_last_insert_rowid(db_);
}

bool Database::Execute(const std::string& sql) const
{
    return (sqlite3_exec(db_, sql.c_str(), NULL, NULL, NULL) == SQLITE_OK);
}

