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

#include <stdexcept>
#include <memory>

extern "C" {
#include <puzzle.h>
}

namespace OFN
{

class PuzzleException : public std::runtime_error
{
public:
    PuzzleException(const std::string& what_arg) : 
        std::runtime_error(what_arg) {}
};

namespace Puzzle
{

class Context
{
public:
    /**
     * Construct a puzzle context.
     */
    Context()
    {
        puzzle_init_context(&ctx_);
    }

    /**
     * Construct a puzzle context with specified dimensions.
     *
     * @param width the maximum width
     * @param height the maximum height
     */
    Context(unsigned int width, unsigned int height)
    {
        puzzle_init_context(&ctx_);

        SetMaxWidth(width);
        SetMaxHeight(height);
    }

    /**
     * Destruct a puzzle context.
     */
    ~Context()
    {
        puzzle_free_context(&ctx_); 
    }

    /**
     * Get the maximum width.
     */
    unsigned int GetMaxWidth() const
    {
        return ctx_.puzzle_max_width;
    }

    /**
     * Get the maximum height.
     */
    unsigned int GetMaxHeight() const
    {
        return ctx_.puzzle_max_height;
    }

    /**
     * Get the maximum number of lambdas.
     */
    unsigned int GetLambdas() const
    {
        return ctx_.puzzle_lambdas;
    }

    /**
     * Get the noise cutoff.
     */
    double GetNoiseCutoff() const
    {
        return ctx_.puzzle_noise_cutoff;
    }

    /**
     * Get the p ratio.
     */
    double GetPRatio() const
    {
        return ctx_.puzzle_p_ratio;
    }

    /**
     * Get the contrast barrier for cropping.
     */
    double GetContrastBarrierForCropping() const
    {
        return ctx_.puzzle_contrast_barrier_for_cropping;
    }

    /**
     * Get the maximum cropping ratio.
     */
    double GetMaxCroppingRatio() const
    {
        return ctx_.puzzle_max_cropping_ratio;
    }

    /**
     * Get whether autocrop is enabled.
     */
    bool AutoCrop() const
    {
        return ctx_.puzzle_enable_autocrop;
    }

    /**
     * Set the maximum width.
     */
    bool SetMaxWidth(unsigned int width)
    {
        return (puzzle_set_max_width(&ctx_, width) == 0);
    }

    /**
     * Set the maximum height.
     */
    bool SetMaxHeight(unsigned int height)
    {
        return (puzzle_set_max_height(&ctx_, height) == 0);
    }

    /**
     * Set the number of lambdas.
     */
    bool SetLambdas(unsigned int lambdas)
    {
        return (puzzle_set_lambdas(&ctx_, lambdas) == 0);
    }

    /**
     * Set the noise cutoff.
     */
    bool SetNoiseCutoff(double noise_cutoff)
    {
        return (puzzle_set_noise_cutoff(&ctx_, noise_cutoff) == 0);
    }

    /**
     * Set the p ratio.
     */
    bool SetPRatio(double ratio)
    {
        return (puzzle_set_p_ratio(&ctx_, ratio) == 0);
    }

    /**
     * Set the contrast barrier for cropping.
     */
    bool SetContrastBarrierForCropping(double barrier)
    {
        return (puzzle_set_contrast_barrier_for_cropping(&ctx_, barrier) == 0);
    }

    /**
     * Set the maximum cropping ratio.
     */
    bool SetMaxCroppingRatio(double ratio)
    {
        return (puzzle_set_max_cropping_ratio(&ctx_, ratio) == 0);
    }

    /**
     * Set whether autocrop is enabled.
     */
    bool SetAutoCrop(int enable)
    {
        return (puzzle_set_autocrop(&ctx_, enable) == 0);
    }

    /**
     * Return the puzzle context.
     */
    PuzzleContext* GetPuzzleContext()
    {
        return &ctx_;
    }

private:
    PuzzleContext ctx_;
};

class CVec
{
public:
    /**
     * Construct an empty cvec.
     */
    CVec(std::shared_ptr<Context> context) :
        context_(context)
    {
        puzzle_init_cvec(GetPuzzleContext(), &cvec_);
    }

    /**
     * Construct a cvec and fill it with bitmap data from an image file.
     */
    CVec(std::shared_ptr<Context> context, const std::string& file) :
        context_(context)
    {
        puzzle_init_cvec(GetPuzzleContext(), &cvec_);
        
        if (!LoadFile(file))
            throw PuzzleException("Could not load bitmap file");
    }

    /**
     * Destruct the cvec.
     */
    ~CVec()
    {
        puzzle_free_cvec(GetPuzzleContext(), &cvec_);
    }

    /**
     * Fill a cvec with bitmap data from an image file.
     *
     * @returns true on success, false otherwise.
     */
    bool LoadFile(const std::string& file)
    {
        return (puzzle_fill_cvec_from_file(GetPuzzleContext(), &cvec_,
                                           file.c_str()) == 0);
    }

    /**
     * Get the raw puzzle context.
     */
    inline PuzzleContext* GetPuzzleContext()
    {
        return context_->GetPuzzleContext();
    }

private:
    PuzzleCvec cvec_;
    std::shared_ptr<Context> context_;
};

}

}
