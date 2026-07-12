/*

SPDX-License-Identifier: MIT

Copyright (c) 2026 MarchBeta2087

binary_bigint.h

*/

#include <stdint.h>

#ifndef BINARY_BIGINT_H
#define BINARY_BIGINT_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint64_t  length;   /* 单位是 bit，默认为最低值 512-bit，可乘以 2 扩容至 1024-bit、2048-bit 等等，最大可以到 2^63-bit，也就是 2^60 bytes 或者 1EB */
    uint64_t  start;
    uint64_t  end;
    uint64_t* data;     /* 动态数组，小端序存储：data[0] 存低 64 位。length = 512 对应的数组长度是 8，length = 1024 对应的数组长度是 16 */
} BinaryBigint;

/* ---------------- 函数声明 ---------------- */
BinaryBigint* BinaryBigint_init(void);
void          BinaryBigint_delete(BinaryBigint* bb);
void          BinaryBigint_expand(BinaryBigint* bb); // 扩容到原来的两倍
void          BinaryBigint_homing(BinaryBigint* bb); // [start, end) 内数据覆盖到 [0, end-start)

/* 算法步骤 */
void step1_remove_trailing_zeros(BinaryBigint* Y);
void step2_remove_trailing_01(const BinaryBigint* Y, BinaryBigint* Z);
void step3_mult3(const BinaryBigint* Z, BinaryBigint* W);
int  step4_is_11(const BinaryBigint* W);
void step4_transform(const BinaryBigint* W, BinaryBigint* Y_new);

char*         read_input(void);
BinaryBigint* parse_binary_string(const char* str);
void          print_binary(const BinaryBigint* bb);

#ifdef __cplusplus
}
#endif

#endif
