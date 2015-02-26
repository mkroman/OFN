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

#include "OFN/SQLite3/Statement.h"

using namespace OFN::SQLite3;

namespace OFN
{
namespace SQLite3
{

template <>
int Statement::Bind<int64_t>(int index, const int64_t& value)
{
    return sqlite3_bind_int64(stmt_, index, value);
}

template <>
int Statement::Bind<int>(int index, const int& value)
{
    return sqlite3_bind_int(stmt_, index, value);
}

template <>
int Statement::Bind<double>(int index, const double& value)
{
    return sqlite3_bind_double(stmt_, index, value);
}

template<>
int Statement::Bind<std::string>(int index, const std::string& value)
{
    return sqlite3_bind_text(stmt_, index, value.data(), value.size(), NULL);
}

}
}
