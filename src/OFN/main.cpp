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
    { "commit",  &Application::Commit },
    { "search",  &Application::Search },
    { "process", &Application::Process }
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

    for (auto filename : parameters)
    {
        try
        {
            console->info("Searching for images similar to '{}'", filename);
            auto image = std::make_unique<OFN::Image>(context_, filename);

            context_->Search(*image);
        }
        catch (const Puzzle::RuntimeError& error)
        {
            console->error("Puzzle::RuntimeError: {}", error.what());
        }
    }
}

void Application::Commit(std::vector<std::string> parameters)
{
    auto console = spdlog::get("console");

    for (auto filename : parameters)
    {
        try
        {
            console->info("Comitting image '{}'", filename);

            auto image = std::make_unique<OFN::Image>(context_, filename);

            context_->Commit(*image);
        }
        catch (const Puzzle::RuntimeError& error)
        {
            console->error("Puzzle error: {}", error.what());
        }
    }
}

void Application::Process(std::vector<std::string> parameters)
{
    auto console = spdlog::get("console");
    auto filename = parameters[0];

    try
    {
        auto image = std::make_unique<OFN::Image>(context_, filename);
    }
    catch (const Puzzle::RuntimeError& error)
    {
        console->error("Puzzle::RuntimeError: {}", error.what());
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
            return 1;

        for (auto command : Commands)
        {
            if (parameters[0] == command.name)
            {
                /* Remove the command parameter */
                parameters.erase(std::begin(parameters));

                /* Call the associated command */
                ((*app).*command.function)(parameters);

                return 0;
            }
        }

        console->error("Unknown command `{}'", parameters[0]);

        return 1;
    }
    catch (const Application::CommandLineError& error)
    {
        console->error("Application command-line error: {}", error.what());
    }

    return 0;
}
