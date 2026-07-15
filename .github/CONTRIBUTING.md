# Contributing to Binary Collatz

Thanks for your interest! Here’s how you can contribute.

## How to Report Bugs / Propose Features
- Check existing Issues first.
- Use the provided Issue templates.
- For bugs, always include your **exact input string** and **compilation flags**.

## Development Setup
1. Clone the repo.
2. Compile with the standard flags:
   ```bash
   gcc -std=c99 -O3 -Wall -Wextra -s -o binary_collatz main.c binary_bigint.c output.c benchmark.c -lm
   ```

(Note: -lm is required for math.h used in benchmark fitting).

## Coding Standards

- Language: Strict C99 (no GNU extensions unless absolutely necessary).
- Style: Keep functions focused. Use the provided BinaryBigint API.
- Indentation: 4 spaces. No tabs.
- Error Handling: Check malloc/calloc returns; avoid silent failures.

## Submitting Changes (Pull Requests)

1. Describe your change: Clearly state what and why.
2. Update Docs: If you add a new CLI option, update README.md and the --help text in main.c.
3. Performance Check: Run the built-in benchmark (./binary_collatz -B) to ensure you haven't regressed performance. Include your benchmark results in the PR description if it's a performance-related patch.
4. Sign-off: By submitting a PR, you agree to license your contribution under the MIT License (matching the project).

## Final Checklist

- Code compiles without warnings (-Wall -Wextra).
- Tested with echo "100101" | ./binary_collatz -a -t.
- Updated relevant sections in the documentation.
