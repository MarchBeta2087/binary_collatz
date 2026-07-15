/*

SPDX-License-Identifier: MIT

Copyright (c) 2026 MarchBeta2087

benchmark.c - 性能测试模块，支持 Windows / Unix 跨平台 CSPRNG，含 R^2 输出

*/

#include "benchmark.h"
#include "binary_bigint.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <inttypes.h>

/* ---------- 跨平台 CSPRNG ---------- */
#if defined(_WIN32) || defined(_WIN64)
  #include <windows.h>
  #include <bcrypt.h>
  #pragma comment(lib, "bcrypt.lib")
#else
  #include <unistd.h>   /* for close() */
#endif

static int generate_random_bytes(unsigned char *buf, size_t len) {
#if defined(_WIN32) || defined(_WIN64)
    BCRYPT_ALG_HANDLE hAlg = NULL;
    NTSTATUS status = BCryptOpenAlgorithmProvider(&hAlg, BCRYPT_RNG_ALGORITHM, NULL, 0);
    if (status < 0) return 0;
    status = BCryptGenRandom(hAlg, buf, (ULONG)len, 0);
    BCryptCloseAlgorithmProvider(hAlg, 0);
    return (status >= 0);
#elif defined(__unix__) || defined(__APPLE__) || defined(__ANDROID__)
    FILE *fp = fopen("/dev/urandom", "rb");
    if (!fp) return 0;
    size_t got = fread(buf, 1, len, fp);
    fclose(fp);
    return (got == len);
#else
    for (size_t i = 0; i < len; i++) buf[i] = (unsigned char)(rand() & 0xFF);
    return 1;
#endif
}

/* ---------- 高精度计时 ---------- */
static double get_time_sec(void) {
#if defined(__unix__) || defined(__APPLE__) || defined(__ANDROID__)
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (double)ts.tv_sec + (double)ts.tv_nsec * 1e-9;
#elif defined(_WIN32) || defined(_WIN64)
    LARGE_INTEGER freq, counter;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&counter);
    return (double)counter.QuadPart / (double)freq.QuadPart;
#else
    return (double)clock() / CLOCKS_PER_SEC;
#endif
}

/* ---------- 生成随机二进制串（首位保证为 '1'） ---------- */
static char *generate_random_binary(uint64_t bits) {
    char *str = (char*)malloc(bits + 1);
    if (!str) return NULL;
    str[bits] = '\0';
    str[0] = '1';

    size_t bytes_needed = (bits - 1 + 7) / 8;
    unsigned char *buf = (unsigned char*)malloc(bytes_needed);
    if (!buf) { free(str); return NULL; }

    if (!generate_random_bytes(buf, bytes_needed)) {
        free(buf); free(str); return NULL;
    }

    uint64_t pos = 1;
    for (size_t i = 0; i < bytes_needed && pos < bits; i++) {
        unsigned char byte = buf[i];
        for (int b = 7; b >= 0 && pos < bits; b--) {
            str[pos++] = (byte & (1u << b)) ? '1' : '0';
        }
    }
    free(buf);
    return str;
}

/* ---------- 执行一次完整 Collatz 过程（静默） ---------- */
static uint64_t execute_collatz(const char *bin_str, uint64_t *out_steps) {
    BinaryBigint *Y = parse_binary_string(bin_str);
    if (!Y) return 0;

    uint64_t i = Y->end;
    while (i > Y->start && get_bit(Y, i - 1) == 0) i--;
    if (i == Y->start) { BinaryBigint_delete(Y); return 0; }
    Y->end = i;

    uint64_t deleted_zeros = step1_remove_trailing_zeros(Y);
    int one_on_step_one = step1_is_1(Y);

    uint64_t iterations = 0;
    uint64_t iterated_steps = 0;

    for (;;) {
        iterations++;
        BinaryBigint *Z = BinaryBigint_init();
        iterated_steps += step2_remove_trailing_01(Y, Z);

        BinaryBigint *W = BinaryBigint_init();
        iterated_steps += step3_mult3(Z, W);
        BinaryBigint_delete(Z);

        if (step4_is_11(W)) {
            BinaryBigint_delete(W);
            break;
        }

        BinaryBigint *Y_new = BinaryBigint_init();
        iterated_steps += step4_transform(W, Y_new);
        BinaryBigint_delete(W);
        BinaryBigint_delete(Y);
        Y = Y_new;
    }

    BinaryBigint_delete(Y);

    uint64_t total_steps = deleted_zeros;
    if (!one_on_step_one) total_steps += iterated_steps + 2;
    *out_steps = total_steps;
    return iterations;
}

/* ---------- 统计计算 ---------- */
static void compute_stats(double *data, uint64_t n,
                          double *mean, double *stddev, double *cv) {
    double sum = 0.0, sum_sq = 0.0;
    for (uint64_t i = 0; i < n; i++) {
        sum += data[i];
        sum_sq += data[i] * data[i];
    }
    *mean = sum / n;
    double variance = (sum_sq / n) - (*mean) * (*mean);
    if (variance < 0) variance = 0;
    *stddev = sqrt(variance);
    *cv = (*mean != 0) ? (*stddev / *mean) : 0.0;
}

/* ---------- 二次多项式拟合 (y = a*x^2 + b*x + c)，同时计算 R^2 ---------- */
static void poly_fit2(const double *x, const double *y, uint64_t n,
                      double *a, double *b, double *c, double *r2) {
    double Sx = 0, Sx2 = 0, Sx3 = 0, Sx4 = 0;
    double Sy = 0, Sxy = 0, Sx2y = 0;
    for (uint64_t i = 0; i < n; i++) {
        double xi = x[i], yi = y[i];
        double xi2 = xi * xi;
        double xi3 = xi2 * xi;
        double xi4 = xi3 * xi;
        Sx  += xi;  Sx2 += xi2;  Sx3 += xi3;  Sx4 += xi4;
        Sy  += yi;  Sxy += xi * yi;  Sx2y += xi2 * yi;
    }

    double mat[3][4];
    mat[0][0] = Sx4; mat[0][1] = Sx3; mat[0][2] = Sx2; mat[0][3] = Sx2y;
    mat[1][0] = Sx3; mat[1][1] = Sx2; mat[1][2] = Sx;  mat[1][3] = Sxy;
    mat[2][0] = Sx2; mat[2][1] = Sx;  mat[2][2] = n;   mat[2][3] = Sy;

    for (int col = 0; col < 3; col++) {
        int max_row = col;
        double max_val = fabs(mat[col][col]);
        for (int row = col + 1; row < 3; row++) {
            if (fabs(mat[row][col]) > max_val) {
                max_val = fabs(mat[row][col]);
                max_row = row;
            }
        }
        if (max_row != col) {
            for (int j = col; j < 4; j++) {
                double tmp = mat[col][j];
                mat[col][j] = mat[max_row][j];
                mat[max_row][j] = tmp;
            }
        }
        for (int row = col + 1; row < 3; row++) {
            double factor = mat[row][col] / mat[col][col];
            for (int j = col; j < 4; j++) {
                mat[row][j] -= factor * mat[col][j];
            }
        }
    }

    double sol[3];
    for (int i = 2; i >= 0; i--) {
        double sum = mat[i][3];
        for (int j = i + 1; j < 3; j++) {
            sum -= mat[i][j] * sol[j];
        }
        sol[i] = sum / mat[i][i];
    }

    *a = sol[0];
    *b = sol[1];
    *c = sol[2];

    /* 计算 R^2 */
    double mean_y = Sy / n;
    double ss_res = 0.0, ss_tot = 0.0;
    for (uint64_t i = 0; i < n; i++) {
        double fitted = (*a) * x[i] * x[i] + (*b) * x[i] + (*c);
        double resid = y[i] - fitted;
        ss_res += resid * resid;
        double total = y[i] - mean_y;
        ss_tot += total * total;
    }
    *r2 = (ss_tot > 1e-15) ? (1.0 - ss_res / ss_tot) : 1.0;
}

/* ---------- 性能测试主函数 ---------- */
void benchmark_run(uint64_t low, uint64_t high, uint64_t step,
                   uint64_t samples, const char *csv_filename) {
    if (low == 0 || high < low || step == 0 || samples == 0) {
        fprintf(stderr, "Invalid benchmark parameters.\n");
        return;
    }

    uint64_t num_points = (high - low) / step + 1;
    double *x = (double*)malloc(num_points * sizeof(double));
    double *y = (double*)malloc(num_points * sizeof(double));
    double *stddevs = (double*)malloc(num_points * sizeof(double));
    double *cvs = (double*)malloc(num_points * sizeof(double));
    uint64_t *sample_counts = (uint64_t*)malloc(num_points * sizeof(uint64_t));
    if (!x || !y || !stddevs || !cvs || !sample_counts) {
        fprintf(stderr, "Memory allocation failed.\n");
        goto cleanup0;
    }

    printf("Starting benchmark: low=%"PRIu64" bit, high=%"PRIu64" bit, step=%"PRIu64", samples=%"PRIu64"\n",
           low, high, step, samples);

    uint64_t idx = 0;
    for (uint64_t bits = low; bits <= high; bits += step, idx++) {
        double *times = (double*)malloc(samples * sizeof(double));
        if (!times) {
            fprintf(stderr, "Memory allocation failed for times.\n");
            goto cleanup1;
        }

        for (uint64_t s = 0; s < samples; s++) {
            char *bin = generate_random_binary(bits);
            if (!bin) {
                fprintf(stderr, "Failed to generate random binary of %"PRIu64" bits\n", bits);
                free(times);
                goto cleanup1;
            }

            double start = get_time_sec();
            uint64_t steps;
            execute_collatz(bin, &steps);
            double elapsed = get_time_sec() - start;
            times[s] = elapsed;
            free(bin);

            if (s % 5 == 0) {
                fprintf(stderr, "\r  bits=%"PRIu64" sample=%"PRIu64"/%"PRIu64, bits, s+1, samples);
                fflush(stderr);
            }
        }
        fprintf(stderr, "\n");

        double mean, stddev, cv;
        compute_stats(times, samples, &mean, &stddev, &cv);
        free(times);

        x[idx] = (double)bits / 1000.0;   /* 千位 */
        y[idx] = mean;
        stddevs[idx] = stddev;
        cvs[idx] = cv;
        sample_counts[idx] = samples;

        printf("  %"PRIu64" bits -> avg %.6f sec, std %.6f, cv %.4f\n",
               bits, mean, stddev, cv);
    }

    /* 二次拟合 & R^2 */
    double a, b, c, r2;
    poly_fit2(x, y, num_points, &a, &b, &c, &r2);
    printf("\nFitted polynomial: y = ");
    if (a < 0) printf("- ");
    printf("%.6e * x^2", fabs(a));

    if (b < 0) printf(" - ");
    else printf(" + ");
    printf("%.6e * x", fabs(b));

    if (c < 0) printf(" - ");
    else printf(" + ");
    printf("%.6e\n", fabs(c));

    printf("where x is the value in kilobits (kbit, 1 kbit = 1000 bit) and y is the value in seconds (s).\n");

    printf("R^2 = %.8f\n", r2);

    /* 导出 CSV */
    if (!csv_filename) csv_filename = "benchmark_results.csv";
    FILE *fp = fopen(csv_filename, "w");
    if (!fp) {
        perror("fopen");
        goto cleanup1;
    }

    fprintf(fp, "bits,kilobits,samples,mean_time_sec,stddev,cv,fitted_time\n");
    for (uint64_t i = 0; i < num_points; i++) {
        double fitted = a * x[i] * x[i] + b * x[i] + c;
        fprintf(fp, "%"PRIu64",%.3f,%"PRIu64",%.9f,%.9f,%.6f,%.9f\n",
                (uint64_t)(x[i] * 1000), x[i], sample_counts[i],
                y[i], stddevs[i], cvs[i], fitted);
    }

    fprintf(fp, "\n# Fit coefficients (y = a*x^2 + b*x + c)\n");
    fprintf(fp, "where x is the value in kilobits (kbit) and y is the value in seconds (s).\n");
    fprintf(fp, "# a=%.12e\n", a);
    fprintf(fp, "# b=%.12e\n", b);
    fprintf(fp, "# c=%.12e\n", c);
    fprintf(fp, "# R^2 = %.12f\n", r2);

    fclose(fp);
    printf("\nResults saved to %s\n", csv_filename);

cleanup1:
    free(x); free(y); free(stddevs); free(cvs); free(sample_counts);
cleanup0:
    return;
}
