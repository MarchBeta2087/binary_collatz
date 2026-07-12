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
static inline uint64_t get_bit(const BinaryBigint* bb, uint64_t pos);
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
void step1_remove_trailing_zeros(BinaryBigint* Y) {
    uint64_t pos = Y->start;
    uint64_t end = Y->end;
    while (pos < end && get_bit(Y, pos) == 0) {
        pos++;
    }
    Y->start = pos;
}

void step2_remove_trailing_01(const BinaryBigint* Y, BinaryBigint* Z) {
    BinaryBigint_copy(Y, Z);
    uint64_t pos = Z->start;
    uint64_t end = Z->end;
    while (pos + 1 < end && get_bit(Z, pos) == 1 && get_bit(Z, pos + 1) == 0) {
        pos += 2;
    }
    Z->start = pos;
}

void step3_mult3(const BinaryBigint* Z, BinaryBigint* W) {
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
}

int step4_is_11(const BinaryBigint* W) {
    if (W->end - W->start != 2) return 0;
    return (get_bit(W, W->start) == 1) && (get_bit(W, W->start + 1) == 1);
}

void step4_transform(const BinaryBigint* W, BinaryBigint* Y_new) {
    BinaryBigint_copy(W, Y_new);
    uint64_t pos = Y_new->start;
    uint64_t end = Y_new->end;
    while (pos < end && get_bit(Y_new, pos) == 1) {
        pos++;
    }
    /* 将最低的 0 改为 1 */
    Y_new->data[pos / 64] |= ((uint64_t)1 << (pos % 64));
    /* 删除更低位 */
    Y_new->start = pos;
}

/* ---------------- 工具函数 ---------------- */
static inline uint64_t get_bit(const BinaryBigint* bb, uint64_t pos) {
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

void print_binary(const BinaryBigint* bb) {
    uint64_t s = bb->start;
    uint64_t e = bb->end;
    if (s >= e) {
        putchar('0');
        return;
    }
    uint64_t i;
    for (i = e; i > s; ) {
        i--;
        putchar(get_bit(bb, i) ? '1' : '0');
    }
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
