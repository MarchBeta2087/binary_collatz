/*

SPDX-License-Identifier: MIT

Copyright (c) 2026 MarchBeta2087

output.h

*/

#ifndef OUTPUT_H
#define OUTPUT_H

#include <stdio.h>
#include "binary_bigint.h"

#ifdef __cplusplus
extern "C" {
#endif

/* 设置文件输出模式下的目标文件（默认 stdout） */
void output_set_file(FILE *fp);

/* 设置是否启用缩略模式 */
void output_set_short(int enable);

/*
 * 根据 mode 输出一条提示信息及紧跟的二进制数。
 * mode 取值："console", "silent", "file-output"
 * prompt 示例："Step1 Y: "
 */
void process_binary(const char *prompt, const BinaryBigint *bb, const char *mode);

/*
 * 根据 mode 输出一条格式化文本（类似 printf）。
 * 静默模式下不输出任何内容。
 */
void output_message(const char *mode, const char *format, ...);

#ifdef __cplusplus
}
#endif

#endif
