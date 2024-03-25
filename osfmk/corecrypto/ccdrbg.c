/* Copyright (c) (2022,2023) Apple Inc. All rights reserved.
 *
 * corecrypto is licensed under Apple Inc.’s Internal Use License Agreement (which
 * is contained in the License.txt file distributed with corecrypto) and only to
 * people who accept that license. IMPORTANT:  Any license rights granted to you by
 * Apple Inc. (if any) are limited to internal use within your organization only on
 * devices and computers you own or control, for the sole purpose of verifying the
 * security characteristics and correct functioning of the Apple Software.  You may
 * not, directly or indirectly, redistribute the Apple Software or any portions thereof.
 *
 * @APPLE_OSREFERENCE_LICENSE_HEADER_START@
 *
 * This file contains Original Code and/or Modifications of Original Code
 * as defined in and that are subject to the Apple Public Source License
 * Version 2.0 (the 'License'). You may not use this file except in
 * compliance with the License. The rights granted to you under the License
 * may not be used to create, or enable the creation or redistribution of,
 * unlawful or unlicensed copies of an Apple operating system, or to
 * circumvent, violate, or enable the circumvention or violation of, any
 * terms of an Apple operating system software license agreement.
 *
 * Please obtain a copy of the License at
 * http://www.opensource.apple.com/apsl/ and read it before using this file.
 *
 * The Original Code and all software distributed under the License are
 * distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
 * EXPRESS OR IMPLIED, AND APPLE HEREBY DISCLAIMS ALL SUCH WARRANTIES,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR NON-INFRINGEMENT.
 * Please see the License for the specific language governing rights and
 * limitations under the License.
 *
 * @APPLE_OSREFERENCE_LICENSE_HEADER_END@
 */

#include "cc_internal.h"
#include "ccdrbg.h"
#include "ccdrbg_internal.h"

bool
ccdrbg_must_reseed(const struct ccdrbg_info *info,
    const struct ccdrbg_state *drbg)
{
	CC_ENSURE_DIT_ENABLED

	return info->must_reseed(drbg);
}

int
ccdrbg_init(const struct ccdrbg_info *info,
    struct ccdrbg_state *drbg,
    size_t entropyLength, const void* entropy,
    size_t nonceLength, const void* nonce,
    size_t psLength, const void* ps)
{
	CC_ENSURE_DIT_ENABLED

	return info->init(info, drbg, entropyLength, entropy, nonceLength, nonce, psLength, ps);
}

int
ccdrbg_reseed(const struct ccdrbg_info *info,
    struct ccdrbg_state *drbg,
    size_t entropyLength, const void *entropy,
    size_t additionalLength, const void *additional)
{
	CC_ENSURE_DIT_ENABLED

	return info->reseed(drbg, entropyLength, entropy, additionalLength, additional);
}


int
ccdrbg_generate(const struct ccdrbg_info *info,
    struct ccdrbg_state *drbg,
    size_t dataOutLength, void *dataOut,
    size_t additionalLength, const void *additional)
{
	CC_ENSURE_DIT_ENABLED

	return info->generate(drbg, dataOutLength, dataOut, additionalLength, additional);
}

void
ccdrbg_done(const struct ccdrbg_info *info, struct ccdrbg_state *drbg)
{
	info->done(drbg);
}

size_t
ccdrbg_context_size(const struct ccdrbg_info *info)
{
	return info->size;
}