/*

SPDX-License-Identifier: MIT

Copyright (c) 2026 MarchBeta2087

binary_bigint.c

*/

#include "binary_bigint.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ---------------- 辅助函数声明 ---------------- */
static uint64_t mult3_add_carry(uint64_t word, uint64_t carry, uint64_t* new_carry);
static int clz64(uint64_t x);

/* ---------------- 基本操作 ---------------- */
BinaryBigint* BinaryBigint_init(void) {
    BinaryBigint* bb = (BinaryBigint*)malloc(sizeof(BinaryBigint));
    if (!bb) exit(1);
    bb->length = 512;
    bb->start = 0;
    bb->end = 0;
    bb->data = (uint64_t*)calloc(8, sizeof(uint64_t));
    if (!bb->data) exit(1);
    return bb;
}

static int is_big_endian(void) {
    uint32_t x = 1;
    return (*(char*)&x == 0);
}

void BinaryBigint_delete(BinaryBigint* bb) {
    if (bb) {
        free(bb->data);
        free(bb);
    }
}

void BinaryBigint_expand(BinaryBigint* bb) {
    uint64_t old_len = bb->length;
    uint64_t new_len = old_len * 2;
    uint64_t old_units = old_len / 64;
    uint64_t new_units = new_len / 64;
    uint64_t* new_data = (uint64_t*)calloc(new_units, sizeof(uint64_t));
    if (!new_data) exit(1);
    memcpy(new_data, bb->data, old_units * sizeof(uint64_t));
    free(bb->data);
    bb->data = new_data;
    bb->length = new_len;
}

void BinaryBigint_ensure_space(BinaryBigint* bb, uint64_t required_bits) {
    while (bb->length < required_bits) {
        BinaryBigint_expand(bb);
    }
}

void BinaryBigint_homing(BinaryBigint* bb) {
    if (bb->start == 0) return;
    uint64_t s = bb->start;
    uint64_t e = bb->end;
    if (s >= e) {
        bb->start = 0;
        bb->end = 0;
        return;
    }
    uint64_t len_bits = e - s;
    uint64_t word_s = s / 64;
    uint64_t bit_s = s % 64;
    uint64_t word_len = (len_bits + 63) / 64;
    if (bit_s == 0) {
        memmove(bb->data, bb->data + word_s, word_len * sizeof(uint64_t));
    } else {
        uint64_t i;
        for (i = 0; i < word_len; i++) {
            uint64_t low = bb->data[word_s + i];
            uint64_t high = (i + 1 < (e + 63)/64) ? bb->data[word_s + i + 1] : 0;
            bb->data[i] = (low >> bit_s) | (high << (64 - bit_s));
        }
    }
    memset(bb->data + word_len, 0, (bb->length/64 - word_len) * sizeof(uint64_t));
    bb->start = 0;
    bb->end = len_bits;
}

void BinaryBigint_try_home_expand(BinaryBigint* bb) {
    if (bb->end >= bb->length - 8) {
        uint64_t old_start = bb->start;
        BinaryBigint_homing(bb);
        if (old_start <= (bb->length >> 1) + 16) {
            BinaryBigint_expand(bb);
        }
    }
}

void BinaryBigint_copy(const BinaryBigint* src, BinaryBigint* dst) {
    uint64_t bits = src->end - src->start;
    BinaryBigint_ensure_space(dst, bits);
    memset(dst->data, 0, dst->length / 8);
    if (bits == 0) {
        dst->start = 0;
        dst->end = 0;
        return;
    }
    uint64_t s_start = src->start;
    uint64_t s_end = src->end;
    uint64_t word_s = s_start / 64;
    uint64_t bit_s = s_start % 64;
    uint64_t word_len = (bits + 63) / 64;
    uint64_t i;
    if (bit_s == 0) {
        memcpy(dst->data, src->data + word_s, word_len * sizeof(uint64_t));
    } else {
        for (i = 0; i < word_len; i++) {
            uint64_t low = src->data[word_s + i];
            uint64_t high = (i + 1 < (s_end + 63)/64) ? src->data[word_s + i + 1] : 0;
            dst->data[i] = (low >> bit_s) | (high << (64 - bit_s));
        }
    }
    dst->start = 0;
    dst->end = bits;
}

/* ---------------- 算法步骤 ---------------- */
uint64_t step1_remove_trailing_zeros(BinaryBigint* Y) {
    uint64_t pos = Y->start;
    uint64_t end = Y->end;
    uint64_t i = 0;
    while (pos < end && get_bit(Y, pos) == 0) {
        i++; pos++;
    }
    Y->start = pos;
    return i; // 增加的步数，每删除一个 0 加一步
}

int step1_is_1(const BinaryBigint* Y) {
    if (Y->end - Y->start != 1) return 0;
    return get_bit(Y, Y->start) == 1;
}

uint64_t step2_remove_trailing_01(const BinaryBigint* Y, BinaryBigint* Z) {
    BinaryBigint_copy(Y, Z);
    uint64_t pos = Z->start;
    uint64_t end = Z->end;
    uint64_t i = 0;
    while (pos + 1 < end && get_bit(Z, pos) == 1 && get_bit(Z, pos + 1) == 0) {
        i += 2; pos += 2;
    }
    Z->start = pos;
    return i; // 增加的步数，每删除一个 01 加两步
}

uint64_t step3_mult3(const BinaryBigint* Z, BinaryBigint* W) {
    BinaryBigint* tmp = BinaryBigint_init();
    BinaryBigint_copy(Z, tmp);
    BinaryBigint_homing(tmp);   /* 确保 start = 0 */
    uint64_t z_len = tmp->end;
    uint64_t z_units = (z_len + 63) / 64;
    BinaryBigint_ensure_space(W, z_len + 2);
    memset(W->data, 0, W->length / 8);
    uint64_t carry = 0;
    uint64_t i;
    for (i = 0; i < z_units; i++) {
        uint64_t new_carry;
        W->data[i] = mult3_add_carry(tmp->data[i], carry, &new_carry);
        carry = new_carry;
    }
    uint64_t w_units = z_units;
    if (carry > 0) {
        W->data[w_units] = carry;
        w_units++;
    }
    /* 计算实际 end */
    while (w_units > 0 && W->data[w_units - 1] == 0) w_units--;
    if (w_units == 0) {
        W->end = 0;
    } else {
        uint64_t last_word = W->data[w_units - 1];
        W->end = (w_units - 1) * 64 + (64 - clz64(last_word));
    }
    W->start = 0;
    BinaryBigint_delete(tmp);
    BinaryBigint_try_home_expand(W);
    return 1; // 乘以 3 就是一步
}

int step4_is_11(const BinaryBigint* W) {
    if (W->end - W->start != 2) return 0;
    return (get_bit(W, W->start) == 1) && (get_bit(W, W->start + 1) == 1);
}

uint64_t step4_transform(const BinaryBigint* W, BinaryBigint* Y_new) {
    BinaryBigint_copy(W, Y_new);
    uint64_t pos = Y_new->start;
    uint64_t end = Y_new->end;
    uint64_t i = 0;
    while (pos < end && get_bit(Y_new, pos) == 1) {
        i++; pos++;
    }
    Y_new->data[pos / 64] |= ((uint64_t)1 << (pos % 64));
    if (pos >= Y_new->end) {
        Y_new->end = pos + 1;      // 原先为空时，end 需要更新
    }
    Y_new->start = pos;
    return i; // 增加的步数，每删除一个 1 加一步
}

/* ---------------- 工具函数 ---------------- */
uint64_t get_bit(const BinaryBigint* bb, uint64_t pos) {
    return (bb->data[pos / 64] >> (pos % 64)) & 1;
}

static uint64_t mult3_add_carry(uint64_t word, uint64_t carry, uint64_t* new_carry) {
    uint64_t w_lo = word & 0xFFFFFFFF;
    uint64_t w_hi = word >> 32;
    uint64_t prod_lo = w_lo * 3 + carry;
    uint64_t carry_lo = prod_lo >> 32;
    uint64_t prod_hi = w_hi * 3 + carry_lo;
    *new_carry = prod_hi >> 32;
    return (prod_lo & 0xFFFFFFFF) | (prod_hi << 32);
}

static int clz64(uint64_t x) {
    if (x == 0) return 64;
    int n = 0;
    if ((x >> 32) == 0) { n += 32; x <<= 32; }
    if ((x >> 48) == 0) { n += 16; x <<= 16; }
    if ((x >> 56) == 0) { n += 8;  x <<= 8;  }
    if ((x >> 60) == 0) { n += 4;  x <<= 4;  }
    if ((x >> 62) == 0) { n += 2;  x <<= 2;  }
    if ((x >> 63) == 0) { n += 1;  }
    return n;
}

/* ---------- 预计算表：0-255 每个字节对应 8 个二进制字符（MSB 在前）---------- */
static char BYTE_TO_BIN[256][8];
static int byte_table_initialized = 0;

static void init_byte_table(void) {
    if (byte_table_initialized) return;
    for (int i = 0; i < 256; i++) {
        for (int b = 7; b >= 0; b--) {
            BYTE_TO_BIN[i][7 - b] = (i >> b) & 1 ? '1' : '0';
        }
    }
    byte_table_initialized = 1;
}

/* ---------- 优化后的 fprint_binary ---------- */
void fprint_binary(FILE *fp, const BinaryBigint *bb) {
    init_byte_table();

    uint64_t s = bb->start;
    uint64_t e = bb->end;
    if (s >= e) {
        fputc('0', fp);
        return;
    }

    /* 建议：在 main 里对 fp 调用 setvbuf(fp, NULL, _IOFBF, 1<<20); 
       这里如果 fp 是 stdout 且已重定向到文件，大块缓冲能进一步减少 sys 时间 */
    char buf[8192];
    size_t buf_pos = 0;

    uint64_t first_word = s >> 6;
    uint64_t first_bit  = s & 63;
    uint64_t last_word  = (e - 1) >> 6;
    uint64_t last_bit   = (e - 1) & 63;

    /* ---- 处理最高位字（可能不是完整的 64 位）---- */
    uint64_t w = bb->data[last_word];
    if (last_bit != 63) {
        w &= ((1ULL << (last_bit + 1)) - 1);  /* 屏蔽高位无效位 */
    }

    uint64_t stop = (last_word == first_word) ? first_bit : 0;
    for (int64_t b = (int64_t)last_bit; b >= (int64_t)stop; b--) {
        buf[buf_pos++] = ((w >> b) & 1) ? '1' : '0';
    }

    /* ---- 处理中间完整的字（从高位字到低位字）---- */
    for (int64_t wi = (int64_t)last_word - 1; wi > (int64_t)first_word; wi--) {
        w = bb->data[wi];
        static int big_endian = -1;
        if (big_endian == -1) big_endian = is_big_endian();
        uint8_t *bytes = (uint8_t *)&w;   // 将 uint64_t 看作 8 个字节
        if (big_endian) {
            // 大端：bytes[0] = MSB, bytes[7] = LSB，正序输出（0->7）
            for (int j = 0; j < 8; j++) {
                memcpy(buf + buf_pos, BYTE_TO_BIN[bytes[j]], 8);
                buf_pos += 8;
            }
        } else {
            // 小端：bytes[7] = MSB, bytes[0] = LSB，倒序输出（7->0）
            for (int j = 7; j >= 0; j--) {
                memcpy(buf + buf_pos, BYTE_TO_BIN[bytes[j]], 8);
                buf_pos += 8;
            }
        }
        if (buf_pos + 64 > sizeof(buf)) {
            fwrite(buf, 1, buf_pos, fp);
            buf_pos = 0;
        }
    }

    /* ---- 处理最低位字（如果与最高位字不同）---- */
    if (last_word > first_word) {
        w = bb->data[first_word];
        /* 从 bit 63 下降到 first_bit */
        for (int64_t b = 63; b >= (int64_t)first_bit; b--) {
            buf[buf_pos++] = ((w >> b) & 1) ? '1' : '0';
        }
    }

    if (buf_pos > 0) {
        fwrite(buf, 1, buf_pos, fp);
    }
}

void print_binary(const BinaryBigint *bb) {
    fprint_binary(stdout, bb);    // 保持原有行为
}

char* read_input(void) {
    size_t capacity = 1024;
    size_t len = 0;
    char* buf = (char*)malloc(capacity);
    if (!buf) return NULL;
    int c;
    while ((c = getchar()) != EOF && c != '\n') {
        if (len + 1 >= capacity) {
            capacity *= 2;
            char* newbuf = (char*)realloc(buf, capacity);
            if (!newbuf) { free(buf); return NULL; }
            buf = newbuf;
        }
        buf[len++] = (char)c;
    }
    buf[len] = '\0';
    return buf;
}

BinaryBigint* parse_binary_string(const char* str) {
    uint64_t len = strlen(str);
    BinaryBigint* bb = BinaryBigint_init();
    while (bb->length < len) {
        BinaryBigint_expand(bb);
    }
    bb->end = len;
    bb->start = 0;
    memset(bb->data, 0, bb->length / 8);
    uint64_t i;
    for (i = 0; i < len; i++) {
        if (str[len - 1 - i] == '1') {
            uint64_t word = i / 64;
            uint64_t bit = i % 64;
            bb->data[word] |= ((uint64_t)1 << bit);
        }
    }
    return bb;
}
