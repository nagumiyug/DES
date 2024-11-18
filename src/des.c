#include "../inc/des.h"
#include <string.h> // 用于 memcpy 和 memset
#include <stdio.h>  // 用于调试打印（可选）
#include <stdint.h>
/// 常数表
// 置换选择1
const int PC_table_1[56] = {
    57, 49, 41, 33, 25, 17,  9,
     1, 58, 50, 42, 34, 26, 18,
    10,  2, 59, 51, 43, 35, 27,
    19, 11,  3, 60, 52, 44, 36,
    63, 55, 47, 39, 31, 23, 15,
     7, 62, 54, 46, 38, 30, 22,
    14,  6, 61, 53, 45, 37, 29,
    21, 13,  5, 28, 20, 12, 4
};
// 循环移位表
const int shift_table[16] = {
    1, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1
};
// 置换选择2
const int PC_table_2[48] = {
    14, 17, 11, 24,  1,  5,  3, 28, 15,  6, 21, 10,
    23, 19, 12,  4, 26,  8, 16,  7, 27, 20, 13,  2,
    41, 52, 31, 37, 47, 55, 30, 40, 51, 45, 33, 48,
    44, 49, 39, 56, 34, 53, 46, 42, 50, 36, 29, 32
};
// 初始置换IP
const int IP_table[64] = {
    58, 50, 42, 34, 26, 18, 10, 2,
    60, 52, 44, 36, 28, 20, 12, 4,
    62, 54, 46, 38, 30, 22, 14, 6,
    64, 56, 48, 40, 32, 24, 16, 8,
    57, 49, 41, 33, 25, 17,  9, 1,
    59, 51, 43, 35, 27, 19, 11, 3,
    61, 53, 45, 37, 29, 21, 13, 5,
    63, 55, 47, 39, 31, 23, 15, 7
};
// 逆置换IP^-1
const int IP_table_reverse[64] = {
    40, 8, 48, 16, 56, 24, 64, 32,
    39, 7, 47, 15, 55, 23, 63, 31,
    38, 6, 46, 14, 54, 22, 62, 30,
    37, 5, 45, 13, 53, 21, 61, 29,
    36, 4, 44, 12, 52, 20, 60, 28,
    35, 3, 43, 11, 51, 19, 59, 27,
    34, 2, 42, 10, 50, 18, 58, 26,
    33, 1, 41,  9, 49, 17, 57, 25
};
// E扩展
const int E_box[48] = {
    32,  1,  2,  3,  4,  5,  4,  5,  6,  7,  8,  9,
     8,  9, 10, 11, 12, 13, 12, 13, 14, 15, 16, 17,
    16, 17, 18, 19, 20, 21, 20, 21, 22, 23, 24, 25,
    24, 25, 26, 27, 28, 29, 28, 29, 30, 31, 32,  1
};
// S盒
const int S_box[8][4][16] = {
    {
        {14, 4, 13, 1, 2, 15, 11, 8, 3, 10, 6, 12, 5, 9, 0, 7},
        {0, 15, 7, 4, 14, 2, 13, 1, 10, 6, 12, 11, 9, 5, 3, 8},
        {4, 1, 14, 8, 13, 6, 2, 11, 15, 12, 9, 7, 3, 10, 5, 0},
        {15, 12, 8, 2, 4, 9, 1, 7, 5, 11, 3, 14, 10, 0, 6, 13}
    },
    {
        {15, 1, 8, 14, 6, 11, 3, 4, 9, 7, 2, 13, 12, 0, 5, 10},
        {3, 13, 4, 7, 15, 2, 8, 14, 12, 0, 1, 10, 6, 9, 11, 5},
        {0, 14, 7, 11, 10, 4, 13, 1, 5, 8, 12, 6, 9, 3, 2, 15},
        {13, 8, 10, 1, 3, 15, 4, 2, 11, 6, 7, 12, 0, 5, 14, 9}
    },
    {
        {10, 0, 9, 14, 6, 3, 15, 5, 1, 13, 12, 7, 11, 4, 2, 8},
        {13, 7, 0, 9, 3, 4, 6, 10, 2, 8, 5, 14, 12, 11, 15, 1},
        {13, 6, 4, 9, 8, 15, 3, 0, 11, 1, 2, 12, 5, 10, 14, 7},
        {1, 10, 13, 0, 6, 9, 8, 7, 4, 15, 14, 3, 11, 5, 2, 12}
    },
    {
        {7, 13, 14, 3, 0, 6, 9, 10, 1, 2, 8, 5, 11, 12, 4, 15},
        {13, 8, 11, 5, 6, 15, 0, 3, 4, 7, 2, 12, 1, 10, 14, 9},
        {10, 6, 9, 0, 12, 11, 7, 13, 15, 1, 3, 14, 5, 2, 8, 4},
        {3, 15, 0, 6, 10, 1, 13, 8, 9, 4, 5, 11, 12, 7, 2, 14}
    },
    {
        {2, 12, 4, 1, 7, 10, 11, 6, 8, 5, 3, 15, 13, 0, 14, 9},
        {14, 11, 2, 12, 4, 7, 13, 1, 5, 0, 15, 10, 3, 9, 8, 6},
        {4, 2, 1, 11, 10, 13, 7, 8, 15, 9, 12, 5, 6, 3, 0, 14},
        {11, 8, 12, 7, 1, 14, 2, 13, 6, 15, 0, 9, 10, 4, 5, 3}
    },
    {
        {12, 1, 10, 15, 9, 2, 6, 8, 0, 13, 3, 4, 14, 7, 5, 11},
        {10, 15, 4, 2, 7, 12, 9, 5, 6, 1, 13, 14, 0, 11, 3, 8},
        {9, 14, 15, 5, 2, 8, 12, 3, 7, 0, 4, 10, 1, 13, 11, 6},
        {4, 3, 2, 12, 9, 5, 15, 10, 11, 14, 1, 7, 6, 0, 8, 13}
    },
    {
        {4, 11, 2, 14, 15, 0, 8, 13, 3, 12, 9, 7, 5, 10, 6, 1},
        {13, 0, 11, 7, 4, 9, 1, 10, 14, 3, 5, 12, 2, 15, 8, 6},
        {1, 4, 11, 13, 12, 3, 7, 14, 10, 15, 6, 8, 0, 5, 9, 2},
        {6, 11, 13, 8, 1, 4, 10, 7, 9, 5, 0, 15, 14, 2, 3, 12}
    },
    {
        {13, 2, 8, 4, 6, 15, 11, 1, 10, 9, 3, 14, 5, 0, 12, 7},
        {1, 15, 13, 8, 10, 3, 7, 4, 12, 5, 6, 11, 0, 14, 9, 2},
        {7, 11, 4, 1, 9, 12, 14, 2, 0, 6, 10, 13, 15, 3, 5, 8},
        {2, 1, 14, 7, 4, 10, 8, 13, 15, 12, 9, 0, 3, 5, 6, 11}
    }
};
// P盒
const int P_box[32] = {
    16,  7, 20, 21, 29, 12, 28, 17,
     1, 15, 23, 26,  5, 18, 31, 10, 
     2,  8, 24, 14, 32, 27,  3,  9,
    19, 13, 30,  6, 22, 11,  4, 25
};

/**
 * @brief Generate subkeys
 * @param[in] key original key
 * @param[out] subKeys generated subkeys
 * @return 0 OK
 * @return 1 Failed
 */
int des_make_subkeys(const unsigned char key[8], unsigned char subKeys[16][6]) {
    if (key == NULL || subKeys == NULL) {
        return 1;
    }

    // 将8字节的key转换为uint64_t
    uint64_t key64 = 0;
    for (int i = 0; i < 8; i++) {
        key64 = (key64 << 8) | key[i];
    }
    // 置换选择1
    uint64_t t = 0;
    for (int i = 0; i < 56; i++) {
        int bit_position = 64 - PC_table_1[i];
        uint64_t bit = (key64 >> bit_position) & 1;     // 提取目标位
        t |= (bit << (55 - i));                         // 将位移到目标位置
    }
    uint32_t C = (t >> 28) & 0x0FFFFFFF;
    uint32_t D = t & 0x0FFFFFFF;

    // 初始化subKeys
    memset(subKeys, 0, sizeof(unsigned char) * 16 * 6);
    
    // 16轮生成子密钥
    for (int round = 0; round < 16; round++) {
        // C 循环左移
        C = ((C << shift_table[round]) | (C >> (28 - shift_table[round]))) & 0x0FFFFFFF;
        // D 循环左移
        D = ((D << shift_table[round]) | (D >> (28 - shift_table[round]))) & 0x0FFFFFFF;
        // 置换选择2
        uint64_t CD = (((uint64_t)C << 28) | D) & 0x0FFFFFFFFFFFFFFF;
        uint64_t K = 0;
        for (int i = 0; i < 48; i++) {
            int bit_position = 56 - PC_table_2[i];
            uint64_t bit = (CD >> bit_position) & 1;    // 提取目标位
            K |= (bit << (47 - i));              // 将位移到目标位置
        }
        // 将K的48位存入subKeys[round][6]中
        for (int i = 0; i < 6; i++) {
            subKeys[round][i] = (K >> (40 - 8 * i)) & 0xFF; // 分解为6字节
        }
    }
    return 0;
}
// 轮函数
uint32_t f(uint32_t R, const unsigned char subKeys[6]) {
    // 将密钥转为uint64_t
    uint64_t K = 0;
    for (int i = 0; i < 6; i++) {
        K = (K << 8) | subKeys[i];
    }
    // E扩展
    uint64_t E = 0;
    for (int i = 0; i < 48; i++) {
        int bit_position = 32 - E_box[i];
        uint64_t bit = ((uint64_t)R >> bit_position) & 1;
        E |= (bit << (47 - i));
    }
    // 与子密钥模2加
    E = (E ^ K) & 0x0FFFFFFFFFFFF;
    // S盒运算
    uint32_t S = 0;
    for (int i = 0; i < 8; i++) {
        uint8_t block = (E >> (42 - 6 * i)) & 0x3F;     // 提取6位块
        int row = ((block >> 5) << 1) | (block & 0x1);  // 行号由第1和第6位确定
        int col = (block >> 1) & 0xF;                   // 列号由中间4位确定
        S = (S << 4) | S_box[i][row][col];   // 4位替换
    }
    // 置换运算P
    uint32_t P = 0;
    for (int i = 0; i < 32; i++) {
        int bit_position = 32 - P_box[i];
        uint32_t bit = (S >> bit_position) & 1;
        P |= (bit << (31 - i));
    }
    return P;
}
/**
 * @brief DES encrypt single block
 * @param[in] input plaintext, [length = DES_BLOCK_SIZE]
 * @param[in] subKeys subKeys
 * @param[out] output ciphertext, [length = DES_BLOCK_SIZE]
 */
void des_encrypt_block(const unsigned char *input, unsigned char subKeys[16][6], unsigned char *output) {
    // 将input转换为uint64_t
    uint64_t M = 0;
    for (int i = 0; i < 8; i++) {
        M = (M << 8) | input[i];
    }
    // 初始置换 IP
    uint64_t LR = 0;
    for (int i = 0; i < 64; i++) {
        int bit_position = 64 - IP_table[i];
        uint64_t bit = (M >> bit_position) & 1;
        LR |= (bit << (63 - i));
    }

    uint32_t L = (LR >> 32) & 0x0FFFFFFFF;
    uint32_t R = LR & 0x0FFFFFFFF;
    // 16轮加密
    for (int i = 0; i < 16; i++) {
        uint32_t L_pre = L;
        L = R;
        R = L_pre ^ f(R, subKeys[i]);
    }
    // 合并L16和R16
    LR = ((uint64_t)R << 32) | L;
    // 逆初始置换 IP^-1
    uint64_t C = 0;
    for (int i = 0; i < 64; i++) {
        int bit_position = 64 - IP_table_reverse[i];
        uint64_t bit = (LR >> bit_position) & 1;
        C |= (bit << (63 - i));
    }
    // 将C写入output
    for (int i = 0; i < 8; i++) {
        output[7 - i] = (C >> (i * 8)) & 0xFF;
    }
}

/**
 * @brief DES decrypt single block
 * @param[in] input ciphertext, [length = DES_BLOCK_SIZE]
 * @param[in] subKeys subKeys
 * @param[out] output plaintext, [length = DES_BLOCK_SIZE]
 */
void des_decrypt_block(const unsigned char *input, unsigned char subKeys[16][6], unsigned char *output) {
    // 将input转换为uint64_t
    uint64_t C = 0;
    for (int i = 0; i < 8; i++) {
        C = (C << 8) | input[i];
    }
    
    // 初始置换 IP
    uint64_t LR = 0;
    for (int i = 0; i < 64; i++) {
        int bit_position = 64 - IP_table[i];
        uint64_t bit = (C >> bit_position) & 1;
        LR |= (bit << (63 - i));
    }

    uint32_t L = (LR >> 32) & 0x0FFFFFFFF;
    uint32_t R = LR & 0x0FFFFFFFF;
    // 16轮加密
    for (int i = 0; i < 16; i++) {
        uint32_t L_pre = L;
        L = R;
        R = L_pre ^ f(R, subKeys[15-i]);
    }
    // 合并L16和R16
    LR = ((uint64_t)R << 32) | L;
    
    // 逆初始置换 IP^-1
    uint64_t M = 0;
    for (int i = 0; i < 64; i++) {
        int bit_position = 64 - IP_table_reverse[i];
        uint64_t bit = (LR >> bit_position) & 1;
        M |= (bit << (63 - i));
    }
    
    // 将M写入output
    for (int i = 0; i < 8; i++) {
        output[7 - i] = (M >> (i * 8)) & 0xFF;
    }
}