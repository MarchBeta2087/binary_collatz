# Security Policy

## Supported Versions
Currently, only the latest stable release (v0.5 and above) receives security updates.

| Version | Supported          |
| ------- | ------------------ |
| 0.5+    | :white_check_mark: |
| < 0.5   | :x:                |

## Reporting a Vulnerability

We take the security of Binary Collatz seriously. Since this program handles arbitrary large integers via dynamic memory (`BinaryBigint`), memory safety is a primary concern.

**If you discover a vulnerability (e.g., buffer overflow, heap corruption, or integer overflow):**

1. **Do NOT** open a public issue.
2. Please send a detailed report via **GitHub Private Vulnerability Reporting** (preferred) or directly email the maintainer.
3. Provide the input binary string that triggers the issue and the compilation environment (OS/Compiler version).

We aim to acknowledge reports within **48 hours** and release a patch as soon as possible.
