/*

SPDX-License-Identifier: MIT

Copyright (c) 2026 MarchBeta2087

benchmark.h

*/

#ifndef BENCHMARK_H
#define BENCHMARK_H

#include <stdint.h>

/* 运行性能测试，生成 CSV 文件
   low   : 最低位数（bit）
   high  : 最高位数（bit）
   step  : 步进（bit）
   samples : 每档样本数
   csv_filename : 输出 CSV 文件名（若为 NULL，默认 "benchmark_results.csv"）
*/
void benchmark_run(uint64_t low, uint64_t high, uint64_t step,
                   uint64_t samples, const char *csv_filename);

#endif
