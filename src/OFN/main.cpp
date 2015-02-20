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

#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <getopt.h>
#include <cstring>

#include "OFN/OFN.h"
#include "OFN/Image.h"
#include "OFN/Context.h"
#include "OFN/Puzzle.h"

using ContextPtr = std::shared_ptr<OFN::Context>;

void cmd_commit(const char* filename, ContextPtr c);
void cmd_search(const char* filename, ContextPtr c);

struct command {
    const char* name;
    void (*function)(const char*, ContextPtr);
};

static struct option command_line_options[] = {
    { "help",    no_argument, 0, 'h' },
    { "version", no_argument, 0, 'v' },
    { 0,         0,           0, 0 }
};

static struct command command_line_commands[] = {
    { "commit", cmd_commit },
    { "search", cmd_search },
    { 0, 0 }
};

void cmd_commit(const char* filename, ContextPtr context)
{
    try
    {
        context->Commit(
            std::unique_ptr<OFN::Image>(new OFN::Image(context, filename)));
    }
    catch (const OFN::PuzzleException& exception)
    {
        fprintf(stderr, "libpuzzle exception thrown: %s\n", exception.what());
    }
}

void cmd_search(const char* filename, OFN::Context* c)
{
    try
    {
        auto image = new OFN::Image(c, filename);
        c->Search(image);
        delete image;
    }
    catch (const OFN::PuzzleException& exception)
    {
        fprintf(stderr, "libpuzzle exception thrown: %s\n", exception.what());
    }
}

void print_usage(const char* executable)
{
    printf("Usage: %s [-h] commit|search <file>\n\n", executable);
    printf("ofn %s (c) Mikkel Kroman\n\n", OFN_VERSION);
    puts("Options:");
    puts("  -h, --help     Display this message\n");
}

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
        if (option == 'h')
        {
            print_usage(argv[0]);

            return EXIT_SUCCESS;
        }
    }


    auto context = std::make_shared<OFN::Context>();
    const char* cmd;

    if (optind < argc)
    {
        cmd = argv[optind++];

        if (optind == argc)
        {
            fprintf(stderr, "Missing file argument\n");

            return EXIT_FAILURE;
        }

        for (auto* c = command_line_commands; c->name != 0; c++)
        {
            if (strcmp(c->name, cmd) == 0)
            {
                c->function(argv[optind++], context);
                break;
            }
        }
    }

    return 0;
}
