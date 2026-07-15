/*

SPDX-License-Identifier: MIT

Copyright (c) 2026 MarchBeta2087

main.c

*/

/* main.c */
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <inttypes.h>
#include "binary_bigint.h"
#include "output.h"
#include "benchmark.h"   /* 新增 */

#define ver "v0.5"
#define year "2026"
#define author "MarchBeta2087"
#define license "MIT"
#define source_link "https://github.com/MarchBeta2087/binary_collatz"

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
    printf("  -a, --stat           Get statistics\n");
    printf("  -t, --time           Measure and display execution time inside the program\n");
    printf("  -s, --short          Shorten output of binaries >16 bits (shows first/last 8 bits)\n");
    /* 新增性能测试选项 */
    printf("  -B, --benchmark      Run performance benchmark (silent mode, no normal output)\n");
    printf("      --low=<bits>     Minimum bits for benchmark (default: 10000)\n");
    printf("      --high=<bits>    Maximum bits for benchmark (default: 100000)\n");
    printf("      --step=<bits>    Step size in bits (default: 10000)\n");
    printf("      --samples=<n>    Samples per bit-length (default: 10)\n");
    printf("  -h, --help           Show this help\n");
    printf("  -v, --version        Show version\n");
}

static void print_version(void) {
    printf("Binary Collatz %s\n", ver);
    printf("Copyright (c) %s %s\n\n", year, author);
    printf("This program is open sourced under the %s license,\n", license);
    printf("and the source code can be obtained from:\n\n%s\n\n", source_link);
}

int main(int argc, char *argv[]) {
    const char *mode = "console";
    const char *out_filename = NULL;
    FILE *file_out = NULL;
    int measure_time = 0;
    int short_mode = 0;
    int statistics = 0;

    /* 性能测试参数 */
    int benchmark = 0;
    uint64_t low = 10000;
    uint64_t high = 100000;
    uint64_t step = 10000;
    uint64_t samples = 10;
    const char *csv_file = NULL;   /* 若用户通过 -o 指定，则使用，否则默认 */

    uint64_t deleted_zeros = 0;
    uint64_t iterated_steps = 0;
    uint64_t iterations = 0;
    int one_on_step_one = 1;

    static struct option long_opts[] = {
        {"mode",     required_argument, 0, 'm'},
        {"help",     no_argument,       0, 'h'},
        {"version",  no_argument,       0, 'v'},
        {"time",     no_argument,       0, 't'},
        {"short",    no_argument,       0, 's'},
        {"stat",     no_argument,       0, 'a'},
        {"benchmark", no_argument,      0, 'B'},
        {"low",      required_argument, 0, 1},   /* 长选项专用 */
        {"high",     required_argument, 0, 2},
        {"step",     required_argument, 0, 3},
        {"samples",  required_argument, 0, 4},
        {0, 0, 0, 0}
    };

    int opt, opt_idx;
    while ((opt = getopt_long(argc, argv, "hvm:o:atsB", long_opts, &opt_idx)) != -1) {
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
        case 'a':
            statistics = 1;
            break;
        case 't':
            measure_time = 1;
            break;
        case 's':
            short_mode = 1;
            break;
        case 'B':
            benchmark = 1;
            break;
        case 1:  /* --low */
            low = (uint64_t)strtoull(optarg, NULL, 10);
            break;
        case 2:  /* --high */
            high = (uint64_t)strtoull(optarg, NULL, 10);
            break;
        case 3:  /* --step */
            step = (uint64_t)strtoull(optarg, NULL, 10);
            break;
        case 4:  /* --samples */
            samples = (uint64_t)strtoull(optarg, NULL, 10);
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

    /* 若启用性能测试，则直接运行并退出（忽略其他模式） */
    if (benchmark) {
        /* 如果用户指定了 -o 用于 CSV 输出，则使用该文件名 */
        if (out_filename) csv_file = out_filename;
        benchmark_run(low, high, step, samples, csv_file);
        return EXIT_SUCCESS;
    }

    /* 以下为原有正常流程 */
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
        setvbuf(file_out, NULL, _IOFBF, 1 << 20);
        output_set_file(file_out);
    }

    if (short_mode) output_set_short(1);

    print_version();
    printf("Please enter a string containing only 0 and 1:\n\n");
    char *line = read_input();
    if (!line) {
        if (file_out) fclose(file_out);
        return EXIT_FAILURE;
    }

    double start_time = 0.0;
    if (measure_time) start_time = get_time_sec();

    BinaryBigint *Y = parse_binary_string(line);
    /* 去除前导零 */
    {
        uint64_t i = Y->end;
        while (i > Y->start && get_bit(Y, i - 1) == 0) i--;
        if (i == Y->start) {
            BinaryBigint_delete(Y);
            free(line);
            if (file_out) fclose(file_out);
            return EXIT_FAILURE;
        }
        Y->end = i;
    }
    free(line);

    process_binary("Initial X: ", Y, mode);
    deleted_zeros = step1_remove_trailing_zeros(Y);
    one_on_step_one = step1_is_1(Y);
    process_binary("Step1 Y: ", Y, mode);

    for (;;) {
        iterations++;
        output_message(mode, "\nIteration %" PRIu64 ":\n", iterations);
        BinaryBigint *Z = BinaryBigint_init();
        iterated_steps += step2_remove_trailing_01(Y, Z);
        process_binary("Step2 Z: ", Z, mode);

        BinaryBigint *W = BinaryBigint_init();
        iterated_steps += step3_mult3(Z, W);
        BinaryBigint_delete(Z);
        process_binary("Step3 W: ", W, mode);

        if (step4_is_11(W)) {
            output_message(mode, "Terminate: W is 11\n");
            BinaryBigint_delete(W);
            break;
        }

        BinaryBigint *Y_new = BinaryBigint_init();
        iterated_steps += step4_transform(W, Y_new);
        BinaryBigint_delete(W);
        BinaryBigint_delete(Y);
        Y = Y_new;
        process_binary("Step4 new Y: ", Y, mode);
    }

    BinaryBigint_delete(Y);

    if (measure_time) {
        double elapsed = get_time_sec() - start_time;
        fprintf(stderr, "\nInternal execution time: %.6f seconds\n", elapsed);
    }

    if (statistics) {
        fprintf(stderr, "\nIterations: %" PRIu64 "\n", iterations);
        uint64_t steps = deleted_zeros;
        if (!one_on_step_one) steps += iterated_steps + 2;
        fprintf(stderr, "\nSteps: %" PRIu64 "\n", steps);
    }

    if (file_out) fclose(file_out);
    return EXIT_SUCCESS;
}
