#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include "../inc/des.h"

// Print bytes in hexadecimal format
void print_bytes(const unsigned char *data, size_t size)
{
    for (size_t i = 0; i < size; i++)
    {
        printf("%02X ", data[i]);
    }
    printf("\n");
}

// Print throughput in bits per second
void print_sc_bps(const uint64_t *t, int benches, int rounds, int block_bits)
{
    if (benches < 2)
    {
        fprintf(stderr, "ERROR: Need at least two bench counts!\n");
        return;
    }

    uint64_t acc = 0;
    for (int i = 0; i < benches; i++) acc += t[i];

    uint64_t bits = benches * rounds * block_bits;
    double kbits = (double)bits / (1 << 10);
    double mbits = (double)bits / (1 << 20);
    double gbits = (double)bits / (1 << 30);

    double secend = (double)acc / 1000000000; // Convert nanoseconds to seconds
    double throughput_bits_s = (double)bits / secend; // bits per second
    double throughput_kbits_s = (double)kbits / secend; // kilobits per second
    double throughput_mbits_s = (double)mbits / secend; // megabits per second
    double throughput_gbits_s = (double)gbits / secend; // gigabits per second

    if (throughput_bits_s < 1000)
        printf("throughput: %f bps\n", throughput_bits_s);
    else if (throughput_kbits_s < 1000)
        printf("throughput: %f Kbps\n", throughput_kbits_s);
    else if (throughput_mbits_s < 1000)
        printf("throughput: %f Mbps\n", throughput_mbits_s);
    else
        printf("throughput: %f Gbps\n", throughput_gbits_s);

    printf("\n");
}

// Correctness test function
void test_des_correctness()
{
    unsigned char key[DES_KEY_SIZE] = { 0x13, 0x34, 0x57, 0x79, 0x9A, 0xBC, 0xDE, 0xF0 }; // Fixed example key
    unsigned char plaintext[DES_BLOCK_SIZE] = { 0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF }; // Fixed example plaintext
    unsigned char subKeys[16][6];
    unsigned char ciphertext[DES_BLOCK_SIZE];
    unsigned char decrypted[DES_BLOCK_SIZE];

    // Generate subkeys
    if (des_make_subkeys(key, subKeys) != 0)
    {
        printf("Failed to generate subkeys.\n");
        return;
    }

    printf("Original plaintext: ");
    print_bytes(plaintext, DES_BLOCK_SIZE);

    // Encrypt
    des_encrypt_block(plaintext, subKeys, ciphertext);
    printf("Encrypted ciphertext: ");
    print_bytes(ciphertext, DES_BLOCK_SIZE);

    // Decrypt
    des_decrypt_block(ciphertext, subKeys, decrypted);
    printf("Decrypted plaintext: ");
    print_bytes(decrypted, DES_BLOCK_SIZE);

    // Verify decryption result
    if (memcmp(plaintext, decrypted, DES_BLOCK_SIZE) == 0)
    {
        printf("Correctness test passed: Decryption matches the original plaintext.\n");
    }
    else
    {
        printf("Correctness test failed: Decryption does not match the original plaintext.\n");
    }
}

// Performance test function
void test_des_performance()
{
    unsigned char key[DES_KEY_SIZE] = { 0x13, 0x34, 0x57, 0x79, 0x9A, 0xBC, 0xDE, 0xF0 }; // Fixed example key
    unsigned char plaintext[DES_BLOCK_SIZE] = { 0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF }; // Fixed example plaintext
    unsigned char subKeys[16][6];
    unsigned char output[DES_BLOCK_SIZE];
    int rounds = 1000000; // Number of encryption rounds
    uint64_t *time_taken = (uint64_t *)malloc(sizeof(uint64_t) * rounds); // Store time for each encryption

    // Generate subkeys
    if (des_make_subkeys(key, subKeys) != 0)
    {
        printf("Failed to generate subkeys.\n");
        free(time_taken);
        return;
    }

    // Perform performance test
    for (int i = 0; i < rounds; i++)
    {
        clock_t start_time = clock();

        // Encrypt a single block
        des_encrypt_block(plaintext, subKeys, output);

        clock_t end_time = clock();
        time_taken[i] = (uint64_t)(end_time - start_time);
    }

    // Print throughput
    print_sc_bps(time_taken, rounds, 1, DES_BLOCK_BITS);

    free(time_taken);
}


int main()
{
    // Perform correctness test
    printf(">> Performing correctness test...\n");
    test_des_correctness();

    printf("\n");

    // Perform performance test
    printf(">> Performing performance test...\n");
    test_des_performance();

    return 0;
}
