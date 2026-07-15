# Binary Collatz v0.5

Copyright (c) 2026 MarchBeta2087

一个基于大整数位操作，演示 Collatz 过程的 C 语言程序，对给定的非零二进制串迭代执行删除尾零、删除尾随 "01" 模式、乘以 3 以及变换操作，直至结果变为 `11b`。

## 更新日志

### [v0.5] - 2026-07-15
- **新增特性**:
  - 新增 `-B --benchmark` 命令行参数，用于性能测试。

### [v0.4] - 2026-07-14
- **新增特性**:
  - 新增 `-a, --stat` 命令行参数，用于统计迭代轮数以及实际 Collatz 步数（到达 1 需要步数）。

### [v0.3] - 2026-07-13
- **新增特性**：
  - 新增 `-t, --time` 命令行参数，支持在程序内部进行高精度单调时间测量，测时结果通过标准错误流（`stderr`）输出，避免污染正常重定向的数据文件。
  - 新增 `-s, --short` 缩略输出模式。针对长度超过 16 位的二进制数仅展示前 8 位与后 8 位并标明总位数，有效减少了超长数据迭代时的屏幕占用与磁盘空间浪费。
- **缺陷修复**：
  - 修正了文件输出模式下 `setvbuf` 作用于 `NULL` 指针导致配置不生效的逻辑缺陷。修复后，1MB 文件全缓冲可正常开启，显著降低了高频磁盘写入时的系统调用开销（`sys` 时间）。

### [v0.2] - 2026-07-13
- **架构重构**：
  - 引入了 `output.c` / `output.h` 独立输出模块，解耦了计算逻辑与 I/O 逻辑。
  - 支持命令行参数解析，提供 `console`（默认）、`silent`（静默）和 `file-output`（文件输出）三种工作模式。
- **性能优化**：
  - 二进制输出函数引入了字节预计算查找表（`BYTE_TO_BIN`），极大提升了字符格式化拼接速度。

### [v0.1] - 2026-07-12
- **初始版本**：
  - 实现了基于大数位操作的 Collatz 仿真核心算法。
  - 引入了 `BinaryBigint` 动态数组大数结构，支持自适应扩容。
  - 采用指针延迟对齐（Soft Deletion）和必要时触发的 `homing` 内存移位策略，减少不必要的数据拷贝。

## 算法流程

1. 删除输入二进制串末尾所有的 `0`，得到 Y。
2. 删除 Y 末尾所有的 `01` 模式（如 `100101` → `10`），得到 Z。
3. 将 Z 乘以 3，得到 W。
4. 若 W 恰好为 `11b`，则终止；否则，将 W 中最低的 `0` 变为 `1`，并删除所有更低的有效位，得到新的 Y，返回步骤 2 继续。

## 原理

Z000000b × 3 + (010101b × 3 + 1) = Z000000b × 3 + (111111b + 1) = Z000000b × 3 + 1000000b = (Z × 3 + 1) × 1000000b。需要注意的是，000000b 和 010101b 在这里并非分别是具体的 6 个 0 和 3 个 01，而是**泛指** 2N 个 0 和 N 个 01。

## 为什么 W 是 11b 则终止？为什么不是 111b、1111b？

因为 111b 是 7，7 不是 3 的倍数；1111b 是 15，15 = 5 × 3，5 是 101b，但 101b 末尾有 01，在步骤 2 已经被删了。

11b 终止的原理：1 × 3 + 1 = 4，4 ÷ 2 = 2，2 ÷ 2 = 1。

## 构建与运行

### 依赖

- GCC 或支持 C99 以上的编译器
- 标准 C 库

### 编译

```bash
gcc -std=c99 -O3 -Wall -Wextra -s -o binary_collatz main.c binary_bigint.c output.c benchmark.c
```

注意：如果在 Windows 使用 MinGW-w64 等 C 编译器编译，请添加 `-lbcrypt`。

### 使用

程序支持从标准输入读取一个非零的二进制字符串（仅包含字符 0 和 1），并提供了丰富的命令行配置参数：

```bash
Usage: ./binary_collatz [OPTIONS]

Options:
  --mode=silent        Silent mode, no intermediate output
  --mode=file-output   File output mode (use -o to specify file)
  --mode=console       Console mode (default)
  -o <file>            Output file for file-output mode
  -a, --stat           Get statistics
  -t, --time           Measure and display execution time inside the program
  -s, --short          Shorten output of binaries >16 bits (shows first/last 8 bits)
  -B, --benchmark      Run performance benchmark (silent mode, no normal output)
      --low=<bits>     Minimum bits for benchmark (default: 10000)
      --high=<bits>    Maximum bits for benchmark (default: 100000)
      --step=<bits>    Step size in bits (default: 10000)
      --samples=<n>    Samples per bit-length (default: 10)
  -h, --help           Show this help
  -v, --version        Show version
```

#### 各选项中文解释

```bash
Usage: ./binary_collatz [OPTIONS]

Options:
  --mode=silent        静默模式，不输出任何中间迭代过程
  --mode=file-output   文件输出模式（必须使用 -o 选项指定输出路径）
  --mode=console       控制台输出模式（默认）
  -o <file>            指定文件输出模式下的保存路径
  -a, --stat           获取统计数据
  -t, --time           在程序内部精确测量算法的运行时间（输出至标准错误流 stderr）
  -s, --short          缩略模式。当二进制串长度 >16 时，仅输出“首8位...末8位 (位数)”；长度 >
  -B, --benchmark      运行性能测试（静默模式，无正常输出）
      --low=<bits>     测试最小位数，默认 10000
      --high=<bits>    测试最大位数，默认 100000
      --step=<bits>    测试步进，默认 10000
      --samples=<n>    每个位长度档位的样本数，默认 10
  -h, --help           显示此帮助信息
  -v, --version        显示版本号
```

#### 运行示例

1. **基本控制台输出**：
   ```bash
   echo "100101" | ./binary_collatz
   ```

2. **缩略输出以减少屏幕占用**：
   ```bash
   echo "100101110101010111" | ./binary_collatz -s
   ```

3. **输出到文件并测量耗时**：
   ```bash
   cat large_input.txt | ./binary_collatz --mode=file-output -o result.txt -t -s
   ```

4. **获取统计数据**:
   ```bash
   cat large_input.txt | ./binary_collatz --mode=file-output -o result.txt -a -t -s
   ```

5. **测试性能**:
   ```bash
   ./binary_collatz -B
   ```

## 内部实现与性能优化

- **大数表示**：使用动态数组以小端序存储大整数，每个元素为 `uint64_t`。支持从 512-bit 开始自动扩容至任意长度（理论上限达 $2^{63}$ bit）。
- **指针延迟对齐（Soft Deletion）**：通过调整 `start` 和 `end` 指针避免不必要的数据物理搬移，仅在必要时触发 `homing` 对齐和扩容。
- **高阶 I/O 缓冲**：在文件输出模式下，程序会自动为输出文件流绑定 1MB 的全缓冲区（`setvbuf`），避免高频次的物理写盘导致运行效率下降。
- **高精度测时**：在 Unix/Linux/Android 下利用系统的单调时钟 `clock_gettime` 进行微秒级时间统计，且测时结果输出至 stderr，不干扰数据重定向。

## 注意事项

1. 注意：如果在 Windows 上使用 MinGW-w64 等 C 编译器编译，请添加 `-lbcrypt`。
2. Windows 上的时间精度是 1 毫秒，这是由 Windows 操作系统上的 `CLOCKS_PER_SEC` 确定的，详见 [https://learn.microsoft.com/zh-cn/cpp/c-runtime-library/reference/clock?view=msvc-170](https://learn.microsoft.com/zh-cn/cpp/c-runtime-library/reference/clock?view=msvc-170)。

## 许可证

本项目采用 **MIT 许可证**。详见 [LICENSE](LICENSE.txt) 文件。
