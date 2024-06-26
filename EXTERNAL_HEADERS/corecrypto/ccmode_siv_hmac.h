/* Copyright (c) (2019-2022) Apple Inc. All rights reserved.
 *
 * corecrypto is licensed under Apple Inc.’s Internal Use License Agreement (which
 * is contained in the License.txt file distributed with corecrypto) and only to
 * people who accept that license. IMPORTANT:  Any license rights granted to you by
 * Apple Inc. (if any) are limited to internal use within your organization only on
 * devices and computers you own or control, for the sole purpose of verifying the
 * security characteristics and correct functioning of the Apple Software.  You may
 * not, directly or indirectly, redistribute the Apple Software or any portions thereof.
 */

#ifndef _CORECRYPTO_CCMODE_SIV_HMAC_H
#define _CORECRYPTO_CCMODE_SIV_HMAC_H

#include <corecrypto/cc.h>
#include <corecrypto/ccmode.h>
#include <corecrypto/ccmode_impl.h>
#include <corecrypto/ccdigest.h>
#include <corecrypto/cchmac.h>
#include <corecrypto/ccsha2.h>

#define ccsiv_hmac_ctx_decl(_size_, _name_) cc_ctx_decl_vla(ccsiv_hmac_ctx, _size_, _name_)
#define ccsiv_hmac_ctx_clear(_size_, _name_) cc_clear(_size_, _name_)

/*!
 @function   ccsiv_hmac_context_size
 @abstract   Return size of context
 
 @param      mode       Descriptor for the mode
 */
size_t ccsiv_hmac_context_size(const struct ccmode_siv_hmac *mode);

/*!
 @function   ccsiv_hmac_block_size
 @abstract   Return size of context
 
 @param      mode       Descriptor for the mode
 */
size_t ccsiv_hmac_block_size(const struct ccmode_siv_hmac *mode);

/*!
 @function   ccsiv_hmac_ciphertext_size
 @abstract   Return size of Ciphertext (which is the ciphertext and corresponding tag) given the mode and plaintext length
 
 @param      ctx               Current siv_hmac context that has been previously initialized
 @param      plaintext_size    Size of the plaintext
 
 @discussion returns the length of the aead ciphertext that the context will generate which includes both the encrypted plaintext
 and tag.
 */
size_t ccsiv_hmac_ciphertext_size(ccsiv_hmac_ctx *ctx, size_t plaintext_size);

/*!
 @function   ccsiv_hmac_plaintext_size
 @abstract   Return size of plaintext given a ciphertext length and mode.

 @param      ctx               Current siv_hmac context that has been previously initialized
 @param      ciphertext_size    Size of the ciphertext (which includes the tag)

 @discussion returns the length of the plaintext which results from the decryption of a ciphertext of the corresponding size (here ciphertext size includes the tag).
 */
size_t ccsiv_hmac_plaintext_size(ccsiv_hmac_ctx *ctx, size_t ciphertext_size);

/*!
 @function   ccsiv_hmac_init
 @abstract   Initialize a context for siv_hmac with an associated mode, given key and specifying output tag size.
 
 @param      mode               Descriptor for the mode
 @param      ctx                Alocated context to be intialized
 @param      key_byte_len       Length of the key:  Supported key sizes are 32, 48, 64 bytes
 @param      key                key for siv_hmac
 @param      tag_size           The length of the output tag requested. Must be at least 20 bytes, and can be as large as the
 associated digest's output
 
 @discussion In order to  compute HMAC_SIV_Enc_k(a1,...,am, n, x) where ai is the ith piece of associated data, n is a nonce and x
 is a plaintext, we first initialize the context with this call, and then use it to call ccsiv_hmac_aad for each ai, followed by
 ccsiv_hmac_set_nonce for nonce n, and finally a call to ccsiv_hmac_crypt for the plaintext x. Note the order of the calls to aad,
 nonce and then crypt is critical. If a second encryption is needed then a call to ccsiv_hmac_reset can be used to reset state,
 and begin again.
 */
int ccsiv_hmac_init(const struct ccmode_siv_hmac *mode, ccsiv_hmac_ctx *ctx, size_t key_byte_len, const uint8_t *key, size_t tag_size);

/*!
 @function   ccsiv_hmac_aad
 @abstract   Add the next piece of associated data to the hmac_siv's computation of the tag. Note this call is optional and no
 associated data needs to be provided. Multiple pieces of associated data can be provided by multiple calls to this
 function. Each input is regarded as a separate piece of associated data, and the mac is NOT simply computed on the
 concatenation of all of the associated data inputs. Therefore on decryption the same inputs must be provided in
 the same order.
 
 @param      mode               Descriptor for the mode
 @param      ctx                Intialized ctx
 @param      nbytes             Length of the current associated data being added
 @param      in                 Associated data to be authenticated.
 
 @discussion Adds the associated data given by in to the computation of the tag in the associated data.
 */
int ccsiv_hmac_aad(const struct ccmode_siv_hmac *mode, ccsiv_hmac_ctx *ctx, size_t nbytes, const uint8_t *in);

/*!
 @function   ccsiv_hmac_nonce
 @abstract   Add the nonce to the hmac_siv's computation of the the tag. Changes the internal state of the context
 so that after the call only a crypt or reset call is permitted.
 @param      mode               Descriptor for the mode
 @param      ctx                Intialized ctx
 @param      nbytes             Length of the current nonce data being added
 @param      in                 Nonce data to be authenticated.
 
 @discussion The nonce is a special form of authenticated data. If provided ( a call to hmac_nonce is optional) it allows
 randomization of the ciphertext (preventing deterministic encryption). While the length of the nonce is not limited, the
 amount of entropy that can be provided is limited by the number of bits in the block of the associated block-cipher in mode.
 */
int ccsiv_hmac_set_nonce(const struct ccmode_siv_hmac *mode, ccsiv_hmac_ctx *ctx, size_t nbytes, const uint8_t *in);

/*!
 @function   ccsiv_hmac_crypt
 @abstract   Depending on whether mode has been setup to encrypt or decrypt, this function
 1) Encrypts the plaintext given as input in, and provides the ciphertext (which is a concatenation of the tag
 followed by the encrypted plaintext) as output out. 2) Decrypts plaintext using the input ciphertext at in (which again is the
 tag, followed by encrypted plaintext), and then verifies that the computer tag and provided tags match.
 @param      mode               Descriptor for the mode
 @param      ctx                Intialized ctx
 @param      nbytes             Case 1) Length of the current plaintext
 Case 2) Length of the current ciphertext (tag length + plaintext length)
 @param      in                 Case 1) Plaintext
 Case 2) Ciphertext
 @discussion This function is only called once. If one wishes to compute another (en)/(de)cryption, one resets the state with
 ccsiv_hmac_reset, and then begins the process again. There is no way to stream large plaintext/ciphertext inputs into the
 function.
 @param     out           Case1)  Tag+ Ciphertext (buffer should be already allocated and of length tag + plaintext length)
                    Case 2) Plaintext (buffer should be already allocated and of length ciphertext - tag length

 In the case of a decryption, if there is a failure in verifying the computed tag against the provided tag (embedded int he ciphertext), then a decryption/verification
 failure is returned, and any internally computed plaintexts and tags are zeroed out.
 Lastly the contexts internal state is reset, so that a new decryption/encryption can be commenced.
 */
int ccsiv_hmac_crypt(const struct ccmode_siv_hmac *mode, ccsiv_hmac_ctx *ctx, size_t nbytes, const uint8_t *in, uint8_t *out);

/*!
 @function   ccsiv_hmac_reset
 @abstract   Resets the state of the siv_hamc ctx, maintaining the key, but preparing  the
 ctx to preform a new Associated Data Authenticated (En)/(De)cryption.
 @param      mode               Descriptor for the mode
 @param      ctx                Intialized ctx
 */
int ccsiv_hmac_reset(const struct ccmode_siv_hmac *mode, ccsiv_hmac_ctx *ctx);

/*!
 @function   ccsiv_hmac_one_shot
 @abstract   A simplified but more constrained way of performing an AEAD SIV HMAC (en)/(de)cryption. It is limited because only
 one piece of associated data may be provided.
 @param      mode               Descriptor for the mode
 @param      key_len            Length of the key:  Supported key sizes are 32, 48, 64 bytes
 @param      key                key for siv_hmac
 @param      tag_length         The length of the tag to produce or accept as input. Must be at least 20
 bytes, and can be as large as the hmac's digest's output
 @param      nonce_nbytes       Length of the current nonce data being added
 @param      nonce              Nonce data to be authenticated.
 @param      adata_nbytes       Length of the associated data.
 @param      adata              Associated data to be authenticated.
 @param      in_nbytes          Length of either the plaintext (for encryption) or ciphertext (for decryption)
 @param      in                 plaintext or ciphertext. Note that the ciphertext includes a tag of length tag_length prepended to it.
 @param      out                Buffer to hold ciphertext/plaintext. (Note Ciphertext is of size plaintext length + tag_length and plaintext is of length ciphertext - tag_length.)
 */

// One shot AEAD with only one input for adata, and a nonce.
int ccsiv_hmac_one_shot(const struct ccmode_siv_hmac *mode,
                        size_t key_len,
                        const uint8_t *key,
                        size_t tag_length,
                        unsigned nonce_nbytes,
                        const uint8_t *nonce,
                        unsigned adata_nbytes,
                        const uint8_t *adata,
                        size_t in_nbytes,
                        const uint8_t *in,
                        uint8_t *out);

#endif /* _CORECRYPTO_CCMODE_SIV_HMAC_H */
