/*

SPDX-License-Identifier: MIT

Copyright (c) 2026 MarchBeta2087

main.c

*/

#include "binary_bigint.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ---------------- 主程序 ---------------- */
int main(void) {
    char* line = read_input();
    if (!line) return 1;

    BinaryBigint* Y = parse_binary_string(line);
    free(line);

    printf("Initial X: ");
    print_binary(Y);
    printf("\n");

    step1_remove_trailing_zeros(Y);
    printf("Step1 Y: ");
    print_binary(Y);
    printf("\n");

    for (;;) {
        BinaryBigint* Z = BinaryBigint_init();
        step2_remove_trailing_01(Y, Z);
        printf("Step2 Z: ");
        print_binary(Z);
        printf("\n");

        BinaryBigint* W = BinaryBigint_init();
        step3_mult3(Z, W);
        BinaryBigint_delete(Z);
        printf("Step3 W: ");
        print_binary(W);
        printf("\n");

        if (step4_is_11(W)) {
            printf("Terminate: W is 11\n");
            BinaryBigint_delete(W);
            break;
        }

        BinaryBigint* Y_new = BinaryBigint_init();
        step4_transform(W, Y_new);
        BinaryBigint_delete(W);
        BinaryBigint_delete(Y);
        Y = Y_new;
        printf("Step4 new Y: ");
        print_binary(Y);
        printf("\n");
    }

    BinaryBigint_delete(Y);
    return 0;
}
