# 测试报告

- 测试日期：2026 年 7 月 15 日
- 测试者：MarchBeta2087
- 测试环境：
  - 设备：Xiaomi Redmi K60 Ultra (23078RKD5C)
  - SoC：MediaTek Dimensity 9200 [MT6985] (1+3+4) @ 3.35 GHz
  - RAM：16GB
  - 操作系统：Linux 5.15.180-android13-8-00021-g46a5565a0982-ab13743836 on Termux 0.118.0
  - 程序版本：v0.5
- 编译选项：
  ```bash
  gcc -std=c99 -O3 -Wall -Wextra -s -o binary_collatz main.c binary_bigint.c output.c benchmark.c
  ```
- 测试项目：
  - 1000 位至 10000 位，步进 100 位，每档 100 个样本
  - 10000 位至 100000 位，步进 10000 位，每档 10 个样本

## 测试结果：

```bash
~/MarchBeta2087/binary_collatz $ ./binary_collatz -B --low=1000 --high=10000 --step=100 --samples=100
Starting benchmark: low=1000 bit, high=10000 bit, step=100, samples=100
  bits=1000 sample=96/100
  1000 bits -> avg 0.001756 sec, std 0.000224, cv 0.1278
  bits=1100 sample=96/100
  1100 bits -> avg 0.001944 sec, std 0.000148, cv 0.0760
  bits=1200 sample=96/100
  1200 bits -> avg 0.002222 sec, std 0.000152, cv 0.0684
  bits=1300 sample=96/100
  1300 bits -> avg 0.002551 sec, std 0.000460, cv 0.1802
  bits=1400 sample=96/100
  1400 bits -> avg 0.002791 sec, std 0.000157, cv 0.0562
  bits=1500 sample=96/100
  1500 bits -> avg 0.003053 sec, std 0.000192, cv 0.0628
  bits=1600 sample=96/100
  1600 bits -> avg 0.003403 sec, std 0.000192, cv 0.0563
  bits=1700 sample=96/100
  1700 bits -> avg 0.003604 sec, std 0.000194, cv 0.0537
  bits=1800 sample=96/100
  1800 bits -> avg 0.003919 sec, std 0.000220, cv 0.0560
  bits=1900 sample=96/100
  1900 bits -> avg 0.004216 sec, std 0.000232, cv 0.0550
  bits=2000 sample=96/100
  2000 bits -> avg 0.004468 sec, std 0.000232, cv 0.0518
  bits=2100 sample=96/100
  2100 bits -> avg 0.004919 sec, std 0.000662, cv 0.1346
  bits=2200 sample=96/100
  2200 bits -> avg 0.005185 sec, std 0.000227, cv 0.0437
  bits=2300 sample=96/100
  2300 bits -> avg 0.005580 sec, std 0.000334, cv 0.0598
  bits=2400 sample=96/100
  2400 bits -> avg 0.006110 sec, std 0.001547, cv 0.2532
  bits=2500 sample=96/100
  2500 bits -> avg 0.006329 sec, std 0.000461, cv 0.0729
  bits=2600 sample=96/100
  2600 bits -> avg 0.006693 sec, std 0.000323, cv 0.0483
  bits=2700 sample=96/100
  2700 bits -> avg 0.007066 sec, std 0.000340, cv 0.0482
  bits=2800 sample=96/100
  2800 bits -> avg 0.007383 sec, std 0.000409, cv 0.0554
  bits=2900 sample=96/100
  2900 bits -> avg 0.008055 sec, std 0.001223, cv 0.1518
  bits=3000 sample=96/100
  3000 bits -> avg 0.008135 sec, std 0.000297, cv 0.0365
  bits=3100 sample=96/100
  3100 bits -> avg 0.008607 sec, std 0.000524, cv 0.0609
  bits=3200 sample=96/100
  3200 bits -> avg 0.008928 sec, std 0.000359, cv 0.0402
  bits=3300 sample=96/100
  3300 bits -> avg 0.009334 sec, std 0.000406, cv 0.0435
  bits=3400 sample=96/100
  3400 bits -> avg 0.009679 sec, std 0.000402, cv 0.0416
  bits=3500 sample=96/100
  3500 bits -> avg 0.010011 sec, std 0.000399, cv 0.0399
  bits=3600 sample=96/100
  3600 bits -> avg 0.010506 sec, std 0.000402, cv 0.0382
  bits=3700 sample=96/100
  3700 bits -> avg 0.010876 sec, std 0.000366, cv 0.0337
  bits=3800 sample=96/100                                                                
  3800 bits -> avg 0.011171 sec, std 0.000381, cv 0.0341                                 
  bits=3900 sample=96/100
  3900 bits -> avg 0.011641 sec, std 0.000464, cv 0.0398                                
  bits=4000 sample=96/100
  4000 bits -> avg 0.011931 sec, std 0.000451, cv 0.0378
  bits=4100 sample=96/100
  4100 bits -> avg 0.012285 sec, std 0.000424, cv 0.0345
  bits=4200 sample=96/100
  4200 bits -> avg 0.012833 sec, std 0.000370, cv 0.0288
  bits=4300 sample=96/100
  4300 bits -> avg 0.013350 sec, std 0.000505, cv 0.0378
  bits=4400 sample=96/100
  4400 bits -> avg 0.013683 sec, std 0.000445, cv 0.0325
  bits=4500 sample=96/100
  4500 bits -> avg 0.014263 sec, std 0.000492, cv 0.0345
  bits=4600 sample=96/100
  4600 bits -> avg 0.014757 sec, std 0.000464, cv 0.0314
  bits=4700 sample=96/100
  4700 bits -> avg 0.015199 sec, std 0.000544, cv 0.0358
  bits=4800 sample=96/100
  4800 bits -> avg 0.015750 sec, std 0.000529, cv 0.0336
  bits=4900 sample=96/100
  4900 bits -> avg 0.016244 sec, std 0.000565, cv 0.0348
  bits=5000 sample=96/100
  5000 bits -> avg 0.016733 sec, std 0.000872, cv 0.0521
  bits=5100 sample=96/100
  5100 bits -> avg 0.017521 sec, std 0.002118, cv 0.1209
  bits=5200 sample=96/100
  5200 bits -> avg 0.017700 sec, std 0.000620, cv 0.0350
  bits=5300 sample=96/100
  5300 bits -> avg 0.018744 sec, std 0.002447, cv 0.1305
  bits=5400 sample=96/100
  5400 bits -> avg 0.018646 sec, std 0.000592, cv 0.0317
  bits=5500 sample=96/100
  5500 bits -> avg 0.019308 sec, std 0.000821, cv 0.0425
  bits=5600 sample=96/100
  5600 bits -> avg 0.020248 sec, std 0.001561, cv 0.0771
  bits=5700 sample=96/100
  5700 bits -> avg 0.020165 sec, std 0.000618, cv 0.0306
  bits=5800 sample=96/100
  5800 bits -> avg 0.020720 sec, std 0.000819, cv 0.0395                                   
  bits=5900 sample=96/100
  5900 bits -> avg 0.021481 sec, std 0.002453, cv 0.1142                                    
  bits=6000 sample=96/100
  6000 bits -> avg 0.021606 sec, std 0.000716, cv 0.0331
  bits=6100 sample=96/100
  6100 bits -> avg 0.022134 sec, std 0.000596, cv 0.0269
  bits=6200 sample=96/100
  6200 bits -> avg 0.022703 sec, std 0.000632, cv 0.0279
  bits=6300 sample=96/100
  6300 bits -> avg 0.023205 sec, std 0.000639, cv 0.0275
  bits=6400 sample=96/100
  6400 bits -> avg 0.023567 sec, std 0.000717, cv 0.0304
  bits=6500 sample=96/100
  6500 bits -> avg 0.024120 sec, std 0.000683, cv 0.0283
  bits=6600 sample=96/100
  6600 bits -> avg 0.024815 sec, std 0.000732, cv 0.0295
  bits=6700 sample=96/100
  6700 bits -> avg 0.025344 sec, std 0.000711, cv 0.0280
  bits=6800 sample=96/100
  6800 bits -> avg 0.025664 sec, std 0.000776, cv 0.0302
  bits=6900 sample=96/100
  6900 bits -> avg 0.027090 sec, std 0.003056, cv 0.1128
  bits=7000 sample=96/100
  7000 bits -> avg 0.026775 sec, std 0.000850, cv 0.0317
  bits=7100 sample=96/100
  7100 bits -> avg 0.027328 sec, std 0.001153, cv 0.0422
  bits=7200 sample=96/100
  7200 bits -> avg 0.028421 sec, std 0.002979, cv 0.1048
  bits=7300 sample=96/100
  7300 bits -> avg 0.028328 sec, std 0.000777, cv 0.0274
  bits=7400 sample=96/100
  7400 bits -> avg 0.029004 sec, std 0.001298, cv 0.0448
  bits=7500 sample=96/100
  7500 bits -> avg 0.029213 sec, std 0.000815, cv 0.0279
  bits=7600 sample=96/100
  7600 bits -> avg 0.029949 sec, std 0.000744, cv 0.0248
  bits=7700 sample=96/100
  7700 bits -> avg 0.030699 sec, std 0.001605, cv 0.0523
  bits=7800 sample=96/100
  7800 bits -> avg 0.031432 sec, std 0.002563, cv 0.0816
  bits=7900 sample=96/100
  7900 bits -> avg 0.031513 sec, std 0.002202, cv 0.0699
  bits=8000 sample=96/100
  8000 bits -> avg 0.032283 sec, std 0.001864, cv 0.0577
  bits=8100 sample=96/100
  8100 bits -> avg 0.032473 sec, std 0.000735, cv 0.0226
  bits=8200 sample=96/100
  8200 bits -> avg 0.033595 sec, std 0.002726, cv 0.0811
  bits=8300 sample=96/100
  8300 bits -> avg 0.033678 sec, std 0.000789, cv 0.0234
  bits=8400 sample=96/100
  8400 bits -> avg 0.034355 sec, std 0.000840, cv 0.0245
  bits=8500 sample=96/100
  8500 bits -> avg 0.036344 sec, std 0.001137, cv 0.0313
  bits=8600 sample=96/100
  8600 bits -> avg 0.037695 sec, std 0.001364, cv 0.0362
  bits=8700 sample=96/100
  8700 bits -> avg 0.038511 sec, std 0.001050, cv 0.0273
  bits=8800 sample=96/100
  8800 bits -> avg 0.041634 sec, std 0.003692, cv 0.0887
  bits=8900 sample=96/100
  8900 bits -> avg 0.041374 sec, std 0.001133, cv 0.0274
  bits=9000 sample=96/100
  9000 bits -> avg 0.043727 sec, std 0.002141, cv 0.0490
  bits=9100 sample=96/100
  9100 bits -> avg 0.044622 sec, std 0.001115, cv 0.0250
  bits=9200 sample=96/100
  9200 bits -> avg 0.045536 sec, std 0.001973, cv 0.0433
  bits=9300 sample=96/100
  9300 bits -> avg 0.046657 sec, std 0.003929, cv 0.0842
  bits=9400 sample=96/100
  9400 bits -> avg 0.046804 sec, std 0.002544, cv 0.0544
  bits=9500 sample=96/100
  9500 bits -> avg 0.047214 sec, std 0.001169, cv 0.0248
  bits=9600 sample=96/100
  9600 bits -> avg 0.048010 sec, std 0.001391, cv 0.0290
  bits=9700 sample=96/100
  9700 bits -> avg 0.048878 sec, std 0.002209, cv 0.0452
  bits=9800 sample=96/100
  9800 bits -> avg 0.050522 sec, std 0.004392, cv 0.0869
  bits=9900 sample=96/100
  9900 bits -> avg 0.050666 sec, std 0.001069, cv 0.0211
  bits=10000 sample=96/100
  10000 bits -> avg 0.051501 sec, std 0.003100, cv 0.0602

Fitted polynomial: y = 3.929744e-04 * x^2 + 9.952465e-04 * x + 1.253337e-03
where x is the value in kilobits (kbit, 1 kbit = 1000 bit) and y is the value in seconds (s).
R^2 = 0.99454471

Results saved to benchmark_results.csv
~/MarchBeta2087/binary_collatz $ ./binary_collatz -B                                      
  Starting benchmark: low=10000 bit, high=100000 bit, step=10000, samples=10
  bits=10000 sample=6/10
  10000 bits -> avg 0.047171 sec, std 0.000897, cv 0.0190
  bits=20000 sample=6/10
  20000 bits -> avg 0.129433 sec, std 0.001469, cv 0.0114
  bits=30000 sample=6/10
  30000 bits -> avg 0.236914 sec, std 0.002102, cv 0.0089
  bits=40000 sample=6/10
  40000 bits -> avg 0.384154 sec, std 0.009993, cv 0.0260
  bits=50000 sample=6/10
  50000 bits -> avg 0.565367 sec, std 0.024689, cv 0.0437
  bits=60000 sample=6/10
  60000 bits -> avg 0.755914 sec, std 0.020294, cv 0.0268
  bits=70000 sample=6/10
  70000 bits -> avg 0.982044 sec, std 0.007192, cv 0.0073
  bits=80000 sample=6/10
  80000 bits -> avg 1.253796 sec, std 0.026312, cv 0.0210
  bits=90000 sample=6/10
  90000 bits -> avg 1.538096 sec, std 0.034841, cv 0.0227
  bits=100000 sample=6/10
  100000 bits -> avg 1.814800 sec, std 0.034585, cv 0.0191
                                                                                            
Fitted polynomial: y = 1.375930e-04 * x^2 + 4.766545e-03 * x - 2.112428e-02
where x is the value in kilobits (kbit, 1 kbit = 1000 bit) and y is the value in seconds (s).
R^2 = 0.99973073

Results saved to benchmark_results.csv
```
