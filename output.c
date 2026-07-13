/*

SPDX-License-Identifier: MIT

Copyright (c) 2026 MarchBeta2087

output.c

*/

#include "output.h"
#include <stdarg.h>
#include <string.h>

static FILE *output_file = NULL;   /* 文件模式下的输出流，默认由调用者设置 */
static int short_mode_enabled = 0; /* 是否启用缩略模式 */

void output_set_file(FILE *fp) {
    output_file = fp;
}

void output_set_short(int enable) {
    short_mode_enabled = enable;
}

/* 根据模式返回实际应该写入的 FILE*，静默模式返回 NULL */
static FILE *get_target(const char *mode) {
    if (strcmp(mode, "silent") == 0) return NULL;
    if (strcmp(mode, "file-output") == 0)
        return output_file ? output_file : stdout;
    return stdout;   /* console 或默认 */
}

void process_binary(const char *prompt, const BinaryBigint *bb, const char *mode) {
    FILE *fp = get_target(mode);
    if (!fp) return;                /* 静默模式，不输出 */
    fprintf(fp, "%s", prompt);

    uint64_t len = (bb->end > bb->start) ? (bb->end - bb->start) : 0;

    if (short_mode_enabled && len > 16) {
        /* 1. 打印高位 8 位 (从 bb->end - 1 开始递减) */
        for (int i = 0; i < 8; i++) {
            uint64_t bit = get_bit(bb, bb->end - 1 - i);
            fputc(bit ? '1' : '0', fp);
        }
        
        fprintf(fp, "...");
        
        /* 2. 打印低位 8 位 (从 bb->start + 7 开始递减到 bb->start) */
        for (int i = 0; i < 8; i++) {
            uint64_t bit = get_bit(bb, bb->start + 7 - i);
            fputc(bit ? '1' : '0', fp);
        }
        
        fprintf(fp, " (%llu bits)", (unsigned long long)len);
    } else {
        fprint_binary(fp, bb);          /* 长度小于等于 16 或未开启缩略，原样输出 */
    }
    fputc('\n', fp);
}

void output_message(const char *mode, const char *format, ...) {
    FILE *fp = get_target(mode);
    if (!fp) return;
    va_list args;
    va_start(args, format);
    vfprintf(fp, format, args);
    va_end(args);
}
