/*
 * Copyright (C) 2015 Mikkel Kroman <mk@maero.dk>
 * 
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
 * OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <assert.h>
#include <puzzle.h>
#include <sqlite3.h>

#include "ofn.h"
#include "database.h"

int ofn_init()
{
    int result;

    result = ofn_database_open();

    if (result != 0)
    {
        fprintf(stderr, "Failed to open the database\n");

        return 1;
    }

    puzzle_init_context(&ofn_puzzle_context);
    puzzle_init_cvec(&ofn_puzzle_context, &ofn_puzzle_cvec);

    return 0;
}

int ofn_close()
{
    int result;

    result = ofn_database_close();

    return result;
}

void ofn_get_signature(PuzzleCvec* cvec, char* buffer)
{
    int i;

    assert(cvec->sizeof_vec <= 544);

    for (i = 0; i < cvec->sizeof_vec; i++)
    {
        buffer[i] = cvec->vec[i] + 100;
    }

    buffer[cvec->sizeof_vec] = 0;
}

int ofn_commit(const char* filename)
{
    int i;
    int result;
    sqlite3_int64 image_id;
    char signature[544];
    char word[11], wordbuf[16];
    sqlite3_stmt* stmt;

    result = puzzle_fill_cvec_from_file(&ofn_puzzle_context, &ofn_puzzle_cvec,
                                        filename);

    if (result == 0)
    {
        ofn_get_signature(&ofn_puzzle_cvec, signature);

        // Insert the image information.
        result = sqlite3_prepare(sqlite_db,
                                 "INSERT INTO images (signature, file_path)"
                                 "VALUES(?, ?)",
                                 -1, &stmt, NULL);

        if (result != SQLITE_OK)
            ofn_database_error("Failed to prepare SQL statement:");
        else
        {
            // Insert the image into the database.
            sqlite3_bind_text(stmt, 1, filename, strlen(filename), NULL);
            sqlite3_bind_text(stmt, 2, signature, strlen(signature), NULL);

            result = sqlite3_step(stmt);

            if (result != SQLITE_DONE)
                ofn_database_error("Failed to insert image fingerprint:");
            else
            {
                image_id = sqlite3_last_insert_rowid(sqlite_db);
            }

            sqlite3_finalize(stmt);
        }

        // Insert 100 signatures.
        result = sqlite3_prepare(
            sqlite_db,
            "INSERT INTO signatures (image_id, signature) VALUES(?, ?)", -1,
            &stmt, NULL);

        if (result != SQLITE_OK)
            ofn_database_error("Failed to prepare SQL statement:");

        sqlite3_exec(sqlite_db, "BEGIN TRANSACTION", NULL, NULL, NULL);

        for (i = 0; i < 100; i++)
        {
            sqlite3_reset(stmt);

            strncpy(word, signature + (i * sizeof(char)), 10);
            word[10] = 0;

            snprintf(wordbuf, sizeof(wordbuf), "%d__%s\n", i, word);

            sqlite3_bind_int64(stmt, 1, image_id);
            sqlite3_bind_text(stmt, 2, wordbuf, strlen(wordbuf), NULL);

            result = sqlite3_step(stmt);

            if (result != SQLITE_DONE)
                ofn_database_error("Inserting signature failed:");
        }

        sqlite3_finalize(stmt);
        sqlite3_exec(sqlite_db, "END TRANSACTION", NULL, NULL, NULL);
    }

    return 0;
}

int ofn_search(const char* filename)
{
    int i;
    int result, return_code = 0;
    char signature[SIGNATURE_SIZE];
    char word[11], wordbuf[16];
    sqlite3_stmt* stmt;

    result = puzzle_fill_cvec_from_file(&ofn_puzzle_context, &ofn_puzzle_cvec,
                                        filename);

    if (result == 0)
    {
        ofn_get_signature(&ofn_puzzle_cvec, signature);

        result =
            sqlite3_prepare(sqlite_db, "SELECT image_id, signature FROM "
                                       "signatures WHERE signature IN (?100)",
                            -1, &stmt, NULL);

        if (result != SQLITE_OK)
            ofn_database_error("Failed to prepare SQL statement:");
        else
        {
            for (i = 0; i < 100; i++)
            {
                strncpy(word, signature + (i * sizeof(char)), 10);
                word[10] = 0;

                snprintf(wordbuf, sizeof(wordbuf), "%d__%s\n", i, word);

                /* sqlite3_bind_text(stmt, 1, signature, strlen(signature),
                 * NULL); */
                result = sqlite3_bind_text(stmt, i + 1, wordbuf,
                                           strlen(wordbuf), NULL);

                if (result != SQLITE_OK)
                    ofn_database_error("Failed to bind text:");
            }

            result = sqlite3_step(stmt);

            if (result == SQLITE_ERROR)
            {
                ofn_database_error("Failed to query database:");
                fprintf(stderr, "result: %d", result);
            }
            else if (result == SQLITE_ROW)
            {
                int image_id = 0;
                const unsigned char* sig;

                image_id = sqlite3_column_int(stmt, 0);
                sig = sqlite3_column_text(stmt, 1);

                printf("image_id: %d, sig: %s\n", image_id, sig);

                return_code = 1;
            }
        }

        sqlite3_finalize(stmt);
    }

    return return_code;
}

int main(int argc, char** argv)
{
    int o;
    int option_index;
    static struct option long_options[] = {
        /* Use flags like so:
        {"verbose",	no_argument,	&verbose_flag, 'V'}*/
        /* Argument styles: no_argument, required_argument, optional_argument */
        {"help", no_argument, 0, 'h'},
        {"version", no_argument, 0, 'v'},
        {0, 0, 0, 0}};

#define VERSION "0.1"

    while ((o = getopt_long(argc, argv, "h", long_options, &option_index)) != -1)
    {
        if (o == 'h')
        {
            printf("Usage: %s [-h]\n\n", argv[0]);
            puts("ofn " VERSION " (c) Mikkel Kroman\n");
            puts("Options:");
            puts("  -h, --help     Display this message\n");

            return EXIT_SUCCESS;
        }
    }
    
    return 0;
}
