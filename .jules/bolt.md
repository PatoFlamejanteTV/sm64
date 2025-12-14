## 2024-12-14 - Aliasing optimization in Matrix Multiplication
**Learning:** In C, especially with old compilers or strict aliasing disabled, avoiding matrix copies when source and destination don't overlap (which is common) yields significant performance gains.
**Action:** When implementing matrix operations, check for aliasing (dest != source) and write directly to destination to avoid unnecessary copy loops. Be careful about row/column dependencies.
