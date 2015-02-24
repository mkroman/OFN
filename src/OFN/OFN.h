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

#include <memory>

#include <getopt.h>

#include "spdlog/spdlog.h"
#include "OFN/Puzzle.h"

#include "OFN/Context.h"
#include "OFN/Image.h"

namespace OFN
{

/** The current application version. */
static const char* Version = "0.1.1";

static auto Console = spdlog::stdout_logger_mt("console");

/**
 * Application class for wrapping runtime functionality into an object.
 */
class Application
{
public:
    /**
     * CommandLineError exception class.
     */
    class CommandLineError : public std::runtime_error
    {
    public:
        CommandLineError(const char* msg) : std::runtime_error(msg) {}
    };

    /**
     * Command structure for defining command-line commands.
     */
    struct Command
    {
        using Functor = void (Application::*)(std::vector<std::string>);

        const char* name;
        Functor function;
    };

public:

    /**
     * Construct a new application instance.
     */
    Application();

    /**
     * Destruct the application instance.
     */
    ~Application();

    /**
     * Search for similar images in the database.
     */
    void Search(std::vector<std::string> parameters);

    /**
     * Commit a new image to the database.
     */
    void Commit(std::vector<std::string> parameters);

    /**
     * Process an image, comitting if necessary and then printing similar
     * images.
     */
    void Process(std::vector<std::string> parameters);

    /**
     * Print the command-line usage.
     */
    void PrintUsage(const char* executable);

    /**
     * Parse the command-line parameters and set flags accordingly.
     *
     * @param argc The number of arguments
     * @param argv A pointer to an array of arguments
     * @return A string vector with additional parameters (i.e. parameters that
     *   didn't set any flags)
     */
    std::vector<std::string> ParseParameters(int argc, char* argv[]);

protected:
    std::shared_ptr<Context> context_;
};

}
