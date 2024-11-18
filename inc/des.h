#ifndef DES_H
#define DES_H

#ifdef __cplusplus
extern "C" {
#endif

#define DES_BLOCK_BITS 64  /* bits of DES algoithm block */
#define DES_BLOCK_SIZE 8  /* bytes of DES algoithm block */
#define DES_KEY_SIZE 8 /* bytes of DES algoithm double key */

    /**
     * @brief Generate subkeys
     * @param[in] key original key
     * @param[out] subKeys generated subkeys
     * @return 0 OK
     * @return 1 Failed
     */
    int des_make_subkeys(const unsigned char key[8], unsigned char subKeys[16][6]);

    /**
     * @brief DES encrypt single block
     * @param[in] input plaintext, [length = DES_BLOCK_SIZE]
     * @param[in] subKeys subKeys
     * @param[out] output ciphertext, [length = DES_BLOCK_SIZE]
     */
    void des_encrypt_block(const unsigned char *input, unsigned char subKeys[16][6], unsigned char *output);

    /**
     * @brief DES decrypt single block
     * @param[in] input ciphertext, [length = DES_BLOCK_SIZE]
     * @param[in] subKeys subKeys
     * @param[out] output plaintext, [length = DES_BLOCK_SIZE]
     */
    void des_decrypt_block(const unsigned char *input, unsigned char subKeys[16][6], unsigned char *output);

#ifdef __cplusplus
}
#endif

#endif // DES_H
