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

/**
 * @file Puzzle.h
 * @date 20 Feb 2015
 * @brief C++ wrapper for libpuzzle.
 * @details This is a C++ wrapper for libpuzzle.
 * @author Mikkel Kroman <mk@maero.dk>
 */

#pragma once

#include <stdexcept>
#include <memory>

extern "C" {
# include <puzzle.h>
}

namespace Puzzle
{

class CompressedCVec;

/**
 * Runtime error class.
 */
class RuntimeError : public std::runtime_error
{
public:
    /**
     * Construct a new runtime error with a +what+ argument.
     */
    RuntimeError(const std::string& what_arg) : std::runtime_error(what_arg) {}
};

/**
 * Bitmap load error class.
 */
class BitmapLoadError : public RuntimeError
{
public:
    /**
     * Construct a new bitmap load error with a +what+ argument.
     */
    BitmapLoadError(const std::string& what_arg) : RuntimeError(what_arg) {}
};

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

/**
 * CVec class.
 */
class CVec
{
public:
    /**
     * Construct an empty cvec.
     *
     * @param context a pointer to a puzzle context
     */
    CVec(std::shared_ptr<Context> context) :
        context_(context)
    {
        puzzle_init_cvec(GetPuzzleContext(), &cvec_);
    }

    /**
     * Construct a cvec and fill it with bitmap data from an image file.
     *
     * @param context a pointer to a puzzle context
     * @param file    a path to a valid image file
     */
    CVec(std::shared_ptr<Context> context, const std::string& file) :
        context_(context)
    {
        puzzle_init_cvec(GetPuzzleContext(), &cvec_);
        
        if (!LoadFile(file))
            throw BitmapLoadError("Could not load bitmap file");
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
     * @param file a path to a valid image file
     *
     * @returns true on success, false otherwise.
     */
    bool LoadFile(const std::string& file)
    {
        return (puzzle_fill_cvec_from_file(GetPuzzleContext(), &cvec_,
                                           file.c_str()) == 0);
    }

    /**
     * Compress the vec and return a new compressed cvec.
     *
     * @return a pointer to a new compressed cvec.
     */
    std::unique_ptr<CompressedCVec> Compress() const;

    /**
     * Set the vec buffer.
     *
     * @param vec the new vec buffer
     */
    void SetVec(signed char* vec)
    {
        cvec_.vec = vec;
    }

    /**
     * Set the vec buffer to point to a string to use as buffer.
     *
     * @param string the string to use as buffer
     */
    void SetVec(const std::string& string)
    {
        cvec_.vec =
            reinterpret_cast<signed char*>(const_cast<char*>(string.data()));
        cvec_.sizeof_vec = string.size();
    }

    /**
     * Get a pointer to the vec buffer.
     *
     * @return a pointer to the vec buffer.
     */
    signed char* GetVec() const
    {
        return cvec_.vec;
    }

    /**
     * Get a raw pointer to the cvec struct.
     *
     * @return a raw pointer to the cvec struct.
     */
    const PuzzleCvec* GetCvec() const
    {
        return &cvec_;
    }

    /**
     * Get the cvec buffer size.
     *
     * @return the cvec buffer size.
     */
    size_t GetSize() const
    {
        return cvec_.sizeof_vec;
    }

    /**
     * Get the raw puzzle C context.
     *
     * @return a pointer to the puzzle C context.
     */
    inline PuzzleContext* GetPuzzleContext() const
    {
        return context_->GetPuzzleContext();
    }

private:
    PuzzleCvec cvec_;
    std::shared_ptr<Context> context_;
};

/**
 * CompressedCVec class.
 */
class CompressedCVec
{
public:
    /**
     * Construct a new compressed cvec.
     *
     * @param context a pointer to a puzzle context
     */
    CompressedCVec(std::shared_ptr<Context> context) :
        context_(context)
    {
        puzzle_init_compressed_cvec(GetPuzzleContext(), &cvec_);
    }

    /**
     * Get a pointer to the vec buffer.
     *
     * @return a pointer to the vec buffer.
     */
    unsigned char* GetVec() const
    {
        return cvec_.vec;
    }

    /**
     * Get the size of the compressed vec.
     *
     * @return the size of the compressed vec.
     */
    size_t GetSize() const
    {
        return cvec_.sizeof_compressed_vec;
    }

    /**
     * Compress a puzzle cvec and save it in this compressed cvec.
     *
     * @param cvec the cvec to compress
     *
     * @note it is it up to the user to call `puzzle_free_compressed_cvec`
     *   before compressing a cvec a second time.
     */
    void Compress(const CVec& cvec)
    {
        puzzle_compress_cvec(GetPuzzleContext(), &cvec_, cvec.GetCvec());
    }

    /**
     * Get a raw pointer to the compressed libpuzzle C cvec.
     *
     * @return a pointer to the cvec.
     */
    PuzzleCompressedCvec* GetCvec()
    {
        return &cvec_;
    }

protected:
    /**
     * Get the raw puzzle context.
     *
     * @return a pointer to the raw puzzle C context.
     */
    inline PuzzleContext* GetPuzzleContext()
    {
        return context_->GetPuzzleContext();
    }

private:
    PuzzleCompressedCvec cvec_;
    std::shared_ptr<Context> context_;
};

}

