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
#include <algorithm>
#include <iterator>
#include <getopt.h>
#include <cstring>

#include "OFN/OFN.h"
#include "OFN/Image.h"
#include "OFN/Context.h"
#include "OFN/Puzzle.h"

using namespace OFN;

/** Command-line options. */
static constexpr struct option CommandLineOptions[] = {
    { "help",    no_argument, 0, 'h' },
    { "version", no_argument, 0, 'v' }
};

/** Command-line command arguments. */
static constexpr struct Application::Command Commands[] = {
    { "commit", &Application::Commit },
    { "search", &Application::Search }
};

Application::Application() :
    context_(std::make_shared<Context>())
{
#ifndef NDEBUG
    spdlog::set_level(spdlog::level::debug);
#endif
}

Application::~Application()
{
}

void Application::Search(std::vector<std::string> parameters)
{
    auto console = spdlog::get("console");
    auto filename = parameters[0];

    console->info("Searching for similar images to `{}'", filename);
}

void Application::Commit(std::vector<std::string> parameters) noexcept
{
    auto console = spdlog::get("console");
    auto filename = parameters[0];

    console->info("Comitting image `{}'", filename);

    try
    {
        auto image = std::make_shared<OFN::Image>(context_, filename);

        context_->Commit(image);
    }
    catch (const Puzzle::RuntimeError& error)
    {
        console->error("Puzzle error: {}", error.what());
    }
}

using ParameterList = std::vector<std::string>;

ParameterList Application::ParseParameters(int argc, char* argv[])
{
    int option;
    int idx = 0;
    ParameterList parameters;

    if (argc < 2)
    {
        PrintUsage(argv[0]);

        return ParameterList();
    }

    while ((option = getopt_long(argc, argv, "hv", CommandLineOptions, &idx)) !=
           -1)
    {
        if (option == 'h')
        {
            PrintUsage(argv[0]);

            return ParameterList();
        }
        else if (option == 'v')
        {
            printf("OFN %s (c) Mikkel Kroman\n\n", OFN::Version);
        }
    }

    while (optind < argc)
        parameters.emplace_back(argv[optind++]);

    return parameters;
}

void Application::PrintUsage(const char* executable)
{
    printf("Usage: %s [-h] <commit|search> <file>\n\n", executable);
    printf("ofn %s (c) Mikkel Kroman\n\n", OFN::Version);
    puts("Options:");
    puts("  -h, --help     Display this message\n");
}

int main(int argc, char* argv[])
{
    auto console = spdlog::get("console");
    auto app = std::make_unique<Application>();

    try
    {
        auto parameters = app->ParseParameters(argc, argv);

        if (parameters.size() < 2)
        {
            app->PrintUsage(argv[0]);

            return EXIT_SUCCESS;
        }

        auto command =
            std::find_if(std::begin(Commands), std::end(Commands), [&](auto c) {
                return c.name == parameters[0];
            });

        if (command != std::end(Commands))
        {
            /* Remove the command parameter */
            parameters.erase(std::begin(parameters));

            /* Call the associated command */
            ((*app).*command->function)(parameters);
        }
        else
        {
            console->error("Unknown command `{}'", parameters[0]);
            return 1;
        }
    }
    catch (Application::CommandLineError& error)
    {
        console->error("Application command-line error: {}", error.what());
    }

    return 0;
}
