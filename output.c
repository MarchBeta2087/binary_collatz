/*

SPDX-License-Identifier: MIT

Copyright (c) 2026 MarchBeta2087

output.c

*/

#include "output.h"
#include <stdarg.h>
#include <string.h>

static FILE *output_file = NULL;   /* 文件模式下的输出流，默认由调用者设置 */

void output_set_file(FILE *fp) {
    output_file = fp;
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
    fprint_binary(fp, bb);          /* 使用扩展后的二进制输出函数 */
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
