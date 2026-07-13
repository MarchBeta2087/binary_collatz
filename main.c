/*

SPDX-License-Identifier: MIT

Copyright (c) 2026 MarchBeta2087

main.c

*/

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "binary_bigint.h"
#include "output.h"

#define ver "v0.3"
#define year "2026"
#define author "MarchBeta2087"
#define license "MIT"
#define source_link "https://github.com/MarchBeta2087/binary_collatz"

/* 高精度跨平台时间测量 */
static double get_time_sec(void) {
#if defined(__unix__) || defined(__APPLE__) || defined(__ANDROID__)
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (double)ts.tv_sec + (double)ts.tv_nsec * 1e-9;
#else
    return (double)clock() / CLOCKS_PER_SEC;
#endif
}

static void print_help(const char *prog) {
    printf("Usage: %s [OPTIONS]\n\n", prog);
    printf("Options:\n");
    printf("  --mode=silent        Silent mode, no intermediate output\n");
    printf("  --mode=file-output   File output mode (use -o to specify file)\n");
    printf("  --mode=console       Console mode (default)\n");
    printf("  -o <file>            Output file for file-output mode\n");
    printf("  -t, --time           Measure and display execution time inside the program\n");
    printf("  -s, --short          Shorten output of binaries >16 bits (shows first/last 8 bits)\n");
    printf("  -h, --help           Show this help\n");
    printf("  -v, --version        Show version\n");
}

static void print_version(void) {
    printf("Binary Collatz %s\n", ver);
    printf("Copyright (c) %s %s\n\n", year, author);
    printf("This program is ");
    printf("open sourced under ");
    printf("the %s license, \n", license);
    printf("and the source code ");
    printf("can be obtained from: \n\n%s\n\n", source_link);
}

int main(int argc, char *argv[]) {
    const char *mode = "console";     /* 默认模式 */
    const char *out_filename = NULL;
    FILE *file_out = NULL;
    int measure_time = 0;             /* 是否测量运行时间 */
    int short_mode = 0;               /* 是否启用缩略模式 */

    static struct option long_opts[] = {
        {"mode",    required_argument, 0, 'm'},
        {"help",    no_argument,       0, 'h'},
        {"version", no_argument,       0, 'v'},
        {"time",    no_argument,       0, 't'},
        {"short",   no_argument,       0, 's'},
        {0, 0, 0, 0}
    };

    int opt;
    while ((opt = getopt_long(argc, argv, "hvm:o:ts", long_opts, NULL)) != -1) {
        switch (opt) {
        case 'm':
            if (strcmp(optarg, "silent") == 0 ||
                strcmp(optarg, "file-output") == 0 ||
                strcmp(optarg, "console") == 0)
                mode = optarg;
            else {
                fprintf(stderr, "Invalid mode: %s\n", optarg);
                return EXIT_FAILURE;
            }
            break;
        case 'o':
            out_filename = optarg;
            break;
        case 't':
            measure_time = 1;
            break;
        case 's':
            short_mode = 1;
            break;
        case 'h':
            print_help(argv[0]);
            return EXIT_SUCCESS;
        case 'v':
            print_version();
            return EXIT_SUCCESS;
        default:
            print_help(argv[0]);
            return EXIT_FAILURE;
        }
    }

    /* 如果是文件输出模式，打开文件并设置 1MB 全缓冲 */
    if (strcmp(mode, "file-output") == 0) {
        if (!out_filename) {
            fprintf(stderr, "File output mode requires -o <file>\n");
            return EXIT_FAILURE;
        }
        file_out = fopen(out_filename, "w");
        if (!file_out) {
            perror("fopen");
            return EXIT_FAILURE;
        }
        setvbuf(file_out, NULL, _IOFBF, 1 << 20);  /* 修复：在此处成功打开文件后再进行缓冲设置 */
        output_set_file(file_out);
    }

    /* 启用缩略模式 */
    if (short_mode) {
        output_set_short(1);
    }

    /* ---------- 算法主流程（原逻辑，替换所有输出） ---------- */
    print_version();
    printf("Please enter a string containing only 0 and 1:\n\n");
    char *line = read_input();
    if (!line) {
        if (file_out) fclose(file_out);
        return EXIT_FAILURE;
    }

    /* 开始测量时间（从输入解析后、核心算法启动前开始计时） */
    double start_time = 0.0;
    if (measure_time) {
        start_time = get_time_sec();
    }

    BinaryBigint *Y = parse_binary_string(line);
    // ---------- 规范化：去除前导零 ----------
    {
        uint64_t i = Y->end;
        while (i > Y->start && get_bit(Y, i - 1) == 0) {
            i--;
        }
        if (i == Y->start) {
            // 全零，直接退出
            BinaryBigint_delete(Y);
            free(line);
            if (file_out) fclose(file_out);
            return EXIT_FAILURE;
        }
        Y->end = i;   // 截断高位多余的零
    }
    free(line);

    process_binary("Initial X: ", Y, mode);          // 替代原来的 printf+print_binary

    step1_remove_trailing_zeros(Y);
    process_binary("Step1 Y: ", Y, mode);

    for (;;) {
        BinaryBigint *Z = BinaryBigint_init();
        step2_remove_trailing_01(Y, Z);
        process_binary("Step2 Z: ", Z, mode);

        BinaryBigint *W = BinaryBigint_init();
        step3_mult3(Z, W);
        BinaryBigint_delete(Z);
        process_binary("Step3 W: ", W, mode);

        if (step4_is_11(W)) {
            output_message(mode, "Terminate: W is 11\n");
            BinaryBigint_delete(W);
            break;
        }

        BinaryBigint *Y_new = BinaryBigint_init();
        step4_transform(W, Y_new);
        BinaryBigint_delete(W);
        BinaryBigint_delete(Y);
        Y = Y_new;
        process_binary("Step4 new Y: ", Y, mode);
    }

    BinaryBigint_delete(Y);

    /* 停止计时并输出结果 */
    if (measure_time) {
        double elapsed = get_time_sec() - start_time;
        /* 输出至 stderr，避免干扰正常的重定向或文件输出内容 */
        fprintf(stderr, "\nInternal execution time: %.6f seconds\n", elapsed);
    }

    if (file_out) fclose(file_out);
    return EXIT_SUCCESS;
}

