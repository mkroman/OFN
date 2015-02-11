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
#include <sqlite3.h>
#include <puzzle.h>

#include "ofn.h"


void print_trace(void* a, const char* b)
{
    fprintf(stderr, "SQL QUERY: %s\n", b);
}

static const int MAX_WORD_LENGTH = 10;
static const int MAX_WORDS = 100;

int ofn_init(ofn_ctx* ctx)
{
    ctx->db = (sqlite3*)malloc(sizeof(sqlite3*));
    ctx->puzzle = (PuzzleContext*)malloc(sizeof(PuzzleContext));

    if (sqlite3_open("ofn.db", &ctx->db) != SQLITE_OK)
    {
        fprintf(stderr, "sqlite3_open: %s\n", sqlite3_errmsg(ctx->db));

        return 1;
    }

    sqlite3_trace(ctx->db, print_trace, NULL);

    puzzle_init_context(ctx->puzzle);

    return 0;
}

int ofn_close(ofn_ctx* ctx)
{
    if (sqlite3_close(ctx->db) != SQLITE_OK)
        return 1;
    
    return 0;
}

// Insert a new image signature.
//
// @returns The signature id on success, 0 otherwise.
sqlite3_int64 ofn_save_signature(ofn_ctx* ctx, sqlite3_int64 image_id,
                                 PuzzleCvec* cvec)
{
    sqlite3_stmt* stmt;
    PuzzleCompressedCvec compressed_cvec;

    // Compress the signature.
    //
    // Initialize the compressed cvec.
    puzzle_init_compressed_cvec(ctx->puzzle, &compressed_cvec);

    // Compress the cvec.
    if (puzzle_compress_cvec(ctx->puzzle, &compressed_cvec, cvec) != 0)
    {
        fprintf(stderr, "ofn_save_signature: puzzle_compress_cvec failed\n");

        return 1;
    }

    // Prepare the SQL statement.
    if (sqlite3_prepare(ctx->db, "INSERT INTO signatures (image_id, "
                                 "compressed_signature) VALUES(?, ?)",
                        -1, &stmt, NULL) != SQLITE_OK)
    {
        fprintf(stderr,
                "ofn_save_signature: Failed to prepare SQL statement\n");

        return 1;
    }

    sqlite3_bind_int64(stmt, 1, image_id);
    sqlite3_bind_blob(stmt, 2, compressed_cvec.vec,
                      compressed_cvec.sizeof_compressed_vec, NULL);

    if (sqlite3_step(stmt) != SQLITE_DONE)
    {
        fprintf(stderr, "Failed to insert signature\n");

        sqlite3_finalize(stmt);

        return 0;
    }

    sqlite3_finalize(stmt);
    puzzle_free_compressed_cvec(ctx->puzzle, &compressed_cvec);

    return sqlite3_last_insert_rowid(ctx->db);
}

// Insert a new image row.
//
// @returns The image id on success, or 0 on failure.
sqlite3_int64 ofn_save_image(ofn_ctx* ctx, const char* filename)
{
    sqlite3_stmt* stmt;

    if (sqlite3_prepare(ctx->db,
                        "INSERT INTO images (filename, digest) VALUES(?, ?)",
                        -1, &stmt, NULL) != SQLITE_OK)
    {
        fprintf(stderr, "ofn_save_image: Failed to prepare SQL statement: %s\n",
                sqlite3_errmsg(ctx->db));

        return 0;
    }

    // Insert the image into the database.
    sqlite3_bind_text(stmt, 1, filename, strlen(filename), NULL);

    // FIXME: Needs SHA256 digest!
    sqlite3_bind_text(stmt, 2, "123", strlen("123"), NULL);

    if (sqlite3_step(stmt) != SQLITE_DONE)
    {
        fprintf(stderr, "ofn_save_image: %s\n", sqlite3_errmsg(ctx->db));

        sqlite3_finalize(stmt);

        return 0;
    }

    sqlite3_finalize(stmt);

    return sqlite3_last_insert_rowid(ctx->db);
}

int ofn_commit(ofn_ctx* ctx, const char* filename)
{
    int i;
    sqlite3_int64 image_id, signature_id;
    sqlite3_stmt* stmt;
    PuzzleCvec cvec;

    puzzle_init_cvec(ctx->puzzle, &cvec);

    if (puzzle_fill_cvec_from_file(ctx->puzzle, &cvec, filename) != 0)
    {
        fprintf(stderr, "Could not fill cvec!\n");

        return 1;
    }

    // Insert the image.
    image_id = ofn_save_image(ctx, filename);

    if (image_id == 0)
    {
        fprintf(stderr, "ofn_insert_image failed\n");

        return 1;
    }

    signature_id = ofn_save_signature(ctx, image_id, &cvec);

    if (signature_id == 0)
    {
        fprintf(stderr, "ofn_save_signature failed\n");

        return 1;
    }

    // Insert 100 signatures.
    if (sqlite3_prepare(
            ctx->db,
            "INSERT INTO words (signature_id, pos_and_word) VALUES(?, ?)", -1,
            &stmt, NULL) != SQLITE_OK)
    {
        fprintf(stderr, "ofn_commit: Failed to prepare SQL statement: %s\n",
                sqlite3_errmsg(ctx->db));

        return 1;
    }

    sqlite3_exec(ctx->db, "BEGIN TRANSACTION", NULL, NULL, NULL);

    static char buf[MAX_WORD_LENGTH + 1];
    PuzzleCvec word;
    PuzzleCompressedCvec word_compressed;

    puzzle_init_cvec(ctx->puzzle, &word);
    puzzle_init_compressed_cvec(ctx->puzzle, &word_compressed);

    for (i = 0; i < MAX_WORDS; i++)
    {
         sqlite3_reset(stmt);

         assert(i < cvec.sizeof_vec);
         memcpy(buf, cvec.vec + i, MAX_WORD_LENGTH);

         // Compress the word.
         word.vec = (signed char*)buf;
         word.sizeof_vec = MAX_WORD_LENGTH;

         if (puzzle_compress_cvec(ctx->puzzle, &word_compressed, &word) != 0)
         {
             fprintf(stderr, "Failed to compress word\n");
             abort();
         }

         assert(word_compressed.sizeof_compressed_vec < sizeof(buf));

         buf[0] = i;
         memcpy(buf + 1, word_compressed.vec + i, word_compressed.sizeof_compressed_vec);

         sqlite3_bind_int64(stmt, 1, signature_id);
         sqlite3_bind_blob(stmt, 2, buf, word_compressed.sizeof_compressed_vec + 1, NULL);

         if (sqlite3_step(stmt) != SQLITE_DONE)
         {
             fprintf(stderr, "Failed to insert word #%d: %s\n", i,
                     sqlite3_errmsg(ctx->db));
         }
    }

    sqlite3_finalize(stmt);
    sqlite3_exec(ctx->db, "END TRANSACTION", NULL, NULL, NULL);

    return 0;
}

int ofn_search(ofn_ctx* ctx, const char* filename)
{
    int i;
    int result, return_code = 0;
    sqlite3_stmt* stmt;

    PuzzleCvec cvec;
    puzzle_init_cvec(ctx->puzzle, &cvec);

    result = puzzle_fill_cvec_from_file(ctx->puzzle, &cvec, filename);

    if (result == 0)
    {
        //ofn_get_signature(ctx, &cvec, signature);

        result = sqlite3_prepare_v2(
            ctx->db, "SELECT DISTINCT(signature_id) AS signature_id FROM words "
                     "WHERE pos_and_word IN (?, ?, ?, ?, ?, ?, ?, ?, "
                     "?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, "
                     "?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, "
                     "?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, "
                     "?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, "
                     "?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)",
            -1, &stmt, NULL);

        if (result != SQLITE_OK)
        {
            fprintf(stderr, "ofn_search: Failed to prepare SQL statement: %s\n",
                    sqlite3_errmsg(ctx->db));

            return 1;
        }

        static char buf[MAX_WORD_LENGTH + 1];

        PuzzleCvec word;
        PuzzleCompressedCvec word_compressed;

        puzzle_init_cvec(ctx->puzzle, &word);
        puzzle_init_compressed_cvec(ctx->puzzle, &word_compressed);

        for (i = 0; i < MAX_WORDS; i++)
        {
             sqlite3_reset(stmt);

             assert(i < cvec.sizeof_vec);
             memcpy(buf, cvec.vec + i, MAX_WORD_LENGTH);

             // Compress the word.
             word.vec = (signed char*)buf;
             word.sizeof_vec = MAX_WORD_LENGTH;

             if (puzzle_compress_cvec(ctx->puzzle, &word_compressed, &word) != 0)
             {
                 fprintf(stderr, "Failed to compress word\n");
                 abort();
             }

             buf[0] = i;
             memcpy(buf + 1, word_compressed.vec + i,
                    word_compressed.sizeof_compressed_vec);

             sqlite3_bind_blob(stmt, i + 1, buf,
                               word_compressed.sizeof_compressed_vec + 1,
                               SQLITE_TRANSIENT);
        }

        fprintf(stderr, "%s\n", sqlite3_sql(stmt));

        result = sqlite3_step(stmt);

        if (result == SQLITE_ERROR)
        {
            fprintf(stderr, "Failed to query database: %s\n",
                    sqlite3_errmsg(ctx->db));
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
        else if (result == SQLITE_DONE)
        {
            printf("No results!\n");
        }

        sqlite3_finalize(stmt);
    }

    return return_code;
}

void print_usage(const char* prog)
{
    printf("Usage: %s [-h] [commit <file>|search]\n\n", prog);
    puts("ofn " OFN_VERSION " (c) Mikkel Kroman\n");
    puts("Options:");
    puts("  -h, --help     Display this message\n");
}

static struct option command_line_options[] = {
    { "help",    no_argument, 0, 'h' },
    { "version", no_argument, 0, 'v' },
    { 0,         0,           0, 0 }
};

int main(int argc, char** argv)
{
    int option;
    int option_index = 0;

    if (argc < 2)
    {
        print_usage(argv[0]);

        return EXIT_SUCCESS;
    }

    while ((option = getopt_long(argc, argv, "hv", command_line_options,
                                 &option_index)) != -1)
    {
        switch (option)
        {
            case 'h':
                print_usage(argv[0]);
                return EXIT_SUCCESS;

                break;
            case 'v':
                break;

            default:
            {
            }
        }
    }

    const char* filename = NULL;
    const char* cmd;

    ofn_ctx* ctx;

    if (optind < argc)
    {
        cmd = argv[optind++];

        ctx = malloc(sizeof(ofn_ctx));
        ofn_init(ctx);

        if (!strcmp(cmd, "commit"))
        {
            if (optind == argc)
            {
                printf("Missing file argument\n");

                return EXIT_FAILURE;
            }

            filename = argv[optind++];
            printf("Committing file %s.\n", filename);
            ofn_commit(ctx, filename);
        }
        else if (!strcmp(cmd, "search"))
        {
            if (optind == argc)
            {
                printf("Missing file argument\n");

                return EXIT_FAILURE;
            }

            filename = argv[optind++];
            printf("Searching for file %s.\n", filename);
            ofn_search(ctx, filename);
        }
    }

    ofn_close(ctx);

    return 0;
}
