## 2024-05-24 - Unchecked sprintf in Goddard Renderer
**Vulnerability:** `sprintf` used to format `memtrackerName` with user-supplied `name` without bounds check in `src/goddard/renderer.c`.
**Learning:** Legacy code (decompilation) often lacks bounds checks standard in modern C. N64 environment lacks `snprintf` in standard libs.
**Prevention:** Use manual bounds checking with `strlen` when `snprintf` is unavailable.
