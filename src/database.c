#include <stdio.h>
#include <sqlite3.h>
#include "database.h"

struct sqlite3* sqlite_db = NULL;

int ofn_database_open()
{
    int result;

    result = sqlite3_open("ofn.db", &sqlite_db);

    if (result != SQLITE_OK)
    {
        ofn_database_error("Failed to open database:");
    }

    return result;
}

int ofn_database_close()
{
    int result;

    result = sqlite3_close(sqlite_db);

    return result;
}

void ofn_database_error(const char* prefix)
{
    fprintf(stderr, "%s %s\n", prefix, sqlite3_errmsg(sqlite_db));
}
