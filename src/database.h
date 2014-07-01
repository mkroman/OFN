#pragma once
#ifndef __OFN_DATABASE_H
#define __OFN_DATABASE_H

#include <sqlite3.h>

extern struct sqlite3* sqlite_db;

// Open the ofn database.
int ofn_database_open();

// Close the ofn database.
int ofn_database_close();

// Print the last error message from the database.
void ofn_database_error(const char* prefix);

#endif
