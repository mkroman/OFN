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

/**
 * @file Context.h
 * @date 24 Feb 2015
 * @brief libpuzzle context wrapper.
 * @author Mikkel Kroman
 */

extern "C" {
#include <puzzle.h>
}

namespace OFN
{
namespace Puzzle
{

/**
 * Context class.
 */
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
     *
     * @return the maximum width.
     */
    unsigned int GetMaxWidth() const
    {
        return ctx_.puzzle_max_width;
    }

    /**
     * Get the maximum height.
     *
     * @return the maximum height.
     */
    unsigned int GetMaxHeight() const
    {
        return ctx_.puzzle_max_height;
    }

    /**
     * Get the maximum number of lambdas.
     *
     * @return the maximum number of lambdas.
     */
    unsigned int GetLambdas() const
    {
        return ctx_.puzzle_lambdas;
    }

    /**
     * Get the noise cutoff.
     *
     * @return the noise cutoff.
     */
    double GetNoiseCutoff() const
    {
        return ctx_.puzzle_noise_cutoff;
    }

    /**
     * Get the p ratio.
     *
     * @return the p ratio.
     */
    double GetPRatio() const
    {
        return ctx_.puzzle_p_ratio;
    }

    /**
     * Get the contrast barrier for cropping.
     *
     * @return the contrast barrier for cropping.
     */
    double GetContrastBarrierForCropping() const
    {
        return ctx_.puzzle_contrast_barrier_for_cropping;
    }

    /**
     * Get the maximum cropping ratio.
     *
     * @return the maximum cropping ratio.
     */
    double GetMaxCroppingRatio() const
    {
        return ctx_.puzzle_max_cropping_ratio;
    }

    /**
     * Get whether autocrop is enabled.
     *
     * @return true if autocrop is enabled, false otherwise.
     */
    bool AutoCrop() const
    {
        return ctx_.puzzle_enable_autocrop;
    }

    /**
     * Set the maximum width.
     *
     * @param width the new maximum width
     *
     * @return true on success, false otherwise.
     */
    bool SetMaxWidth(unsigned int width)
    {
        return (puzzle_set_max_width(&ctx_, width) == 0);
    }

    /**
     * Set the maximum height.
     *
     * @param height the new maximum height
     *
     * @return true on success, false otherwise.
     */
    bool SetMaxHeight(unsigned int height)
    {
        return (puzzle_set_max_height(&ctx_, height) == 0);
    }

    /**
     * Set the number of lambdas.
     *
     * @param lambdas the new amount of lambdas
     *
     * @return true on success, false otherwise.
     */
    bool SetLambdas(unsigned int lambdas)
    {
        return (puzzle_set_lambdas(&ctx_, lambdas) == 0);
    }

    /**
     * Set the noise cutoff.
     *
     * @param noise_cutoff the new noise cutoff
     *
     * @return true on success, false otherwise.
     */
    bool SetNoiseCutoff(double noise_cutoff)
    {
        return (puzzle_set_noise_cutoff(&ctx_, noise_cutoff) == 0);
    }

    /**
     * Set the p ratio.
     *
     * @param ratio the new p ratio
     *
     * @return true on success, false otherwise.
     */
    bool SetPRatio(double ratio)
    {
        return (puzzle_set_p_ratio(&ctx_, ratio) == 0);
    }

    /**
     * Set the contrast barrier for cropping.
     *
     * @param barrier the new contrast barrier
     *
     * @return true on success, false otherwise.
     */
    bool SetContrastBarrierForCropping(double barrier)
    {
        return (puzzle_set_contrast_barrier_for_cropping(&ctx_, barrier) == 0);
    }

    /**
     * Set the maximum cropping ratio.
     *
     * @param ratio the new cropping ratio
     *
     * @return true on success, false otherwise.
     */
    bool SetMaxCroppingRatio(double ratio)
    {
        return (puzzle_set_max_cropping_ratio(&ctx_, ratio) == 0);
    }

    /**
     * Set whether autocrop is enabled.
     *
     * @param enable whether to enable autocrop or not
     *
     * @return true on success, false otherwise.
     */
    bool SetAutoCrop(int enable)
    {
        return (puzzle_set_autocrop(&ctx_, enable) == 0);
    }

    /**
     * Get the C puzzle context.
     *
     * @return a pointer to the C puzzle context.
     */
    PuzzleContext* GetPuzzleContext()
    {
        return &ctx_;
    }

private:
    PuzzleContext ctx_;
};

}
}
