// SPDX-License-Identifier: Apache-2.0
// ----------------------------------------------------------------------------
// Copyright 2011-2020 Arm Limited
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may not
// use this file except in compliance with the License. You may obtain a copy
// of the License at:
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
// WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
// License for the specific language governing permissions and limitations
// under the License.
// ----------------------------------------------------------------------------

/**
 * @brief Functions and data declarations.
 */

#ifndef ASTCENCCLI_INTERNAL_INCLUDED
#define ASTCENCCLI_INTERNAL_INCLUDED

#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>

#include "astcenc.h"
#include "astcenc_mathlib.h"

// TODO: Replace this with astcenc_image
struct astc_codec_image
{
	uint8_t ***data8;
	uint16_t ***data16;
	int xsize;
	int ysize;
	int zsize;
	int padding;

	// Regional average-and-variance information, initialized by
	// compute_averages_and_variances() only if the astc encoder
	// is requested to do error weighting based on averages and variances.
	float4 *input_averages;
	float4 *input_variances;
	float *input_alpha_averages;
	int linearize_srgb;
};

// Config options to be read from command line
struct cli_config_options
{
	int array_size;
	int silentmode;
	int y_flip;
	int linearize_srgb;
	int thread_count;
	int low_fstop;
	int high_fstop;

	astcenc_swizzle swz_encode {
		ASTCENC_SWZ_R,
		ASTCENC_SWZ_G,
		ASTCENC_SWZ_B,
		ASTCENC_SWZ_A
	};

	astcenc_swizzle swz_decode {
		ASTCENC_SWZ_R,
		ASTCENC_SWZ_G,
		ASTCENC_SWZ_B,
		ASTCENC_SWZ_A
	};
};

/**
 * Functions to load image from file.
 *
 * @param filename            The file path on disk.
 * @param padding             The texel padding needed around the image.
 * @param y_flip              Should this image be Y flipped?
 * @param linearize_srgb      Should this image be converted to linear from sRGB?
 * @param[out] is_hdr         Is the loaded image HDR?
 * @param[out] num_components The number of components in the loaded image.
 *
 * @return The astc image file, or nullptr on error.
 */
astc_codec_image* astc_codec_load_image(
	const char* filename,
	int padding,
	bool y_flip,
	bool linearize_srgb,
	bool& is_hdr,
	int& num_components);

int astc_codec_store_image(
	const astc_codec_image* output_image,
	const char* output_filename,
	const char** file_format_name,
	int y_flip);

int get_output_filename_enforced_bitness(
	const char* filename);


astc_codec_image* alloc_image(
	int bitness,
	int xsize,
	int ysize,
	int zsize,
	int padding);

void free_image(
	astc_codec_image* img);

void fill_image_padding_area(
	astc_codec_image* img);

int determine_image_channels(
	const astc_codec_image* img);

// helper functions to prepare an ASTC image object from a flat array
// Used by the image loaders in "astc_file_load_store.cpp"
astc_codec_image* astc_img_from_floatx4_array(
	const float* image,
	int xsize,
	int ysize,
	int padding,
	int y_flip);

astc_codec_image*astc_img_from_unorm8x4_array(
	const uint8_t*imageptr,
	int xsize,
	int ysize,
	int padding,
	int y_flip);

// helper functions to prepare a flat array from an ASTC image object.
// the array is allocated with new[], and must be freed with delete[].
float* floatx4_array_from_astc_img(
	const astc_codec_image* img,
	int y_flip);

uint8_t* unorm8x4_array_from_astc_img(
	const astc_codec_image* img,
	int y_flip);

/* ============================================================================
  Functions for printing build info and help messages
============================================================================ */
void astcenc_print_header();

void astcenc_print_shorthelp();

void astcenc_print_longhelp();

/**
 * @brief Compute error metrics comparing two images.
 *
 * @param compute_hdr_metrics Non-zero if HDR metrics should be computed.
 * @param input_components    The number of input color components.
 * @param img1                The original iamge.
 * @param img2                The compressed image.
 * @param fstop_lo            The low exposure fstop (HDR only).
 * @param fstop_hi            The high exposure fstop (HDR only).
 * @param show_psnr           Non-zero if metrics should be logged to stdout.
 */
void compute_error_metrics(
	int compute_hdr_metrics,
	int input_components,
	const astc_codec_image* img1,
	const astc_codec_image* img2,
	int fstop_lo,
	int fstop_hi);

/**
 * @brief Get the current time.
 *
 * @returns The current time in seconds since arbitrary epoch.
 */
double get_time();

/**
 * @brief Get the number of CPU cores.
 *
 * @returns The number of online or onlineable CPU cores in the system.
 */
int get_cpu_count();

/**
 * @brief Run-time detection if the host CPU supports SSE 4.2.
 * @returns Zero if not supported, positive value if it is.
 */
int cpu_supports_sse42();

/**
 * @brief Run-time detection if the host CPU supports popcnt.
 * @returns Zero if not supported, positive value if it is.
 */
int cpu_supports_popcnt();

/**
 * @brief Run-time detection if the host CPU supports avx2.
 * @returns Zero if not supported, positive value if it is.
 */
int cpu_supports_avx2();

#endif
