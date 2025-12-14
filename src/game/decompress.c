#include <ultra64.h>
#include "sm64.h"

/**
 * Portable C implementation of Mio0 decompression.
 * Replaces asm/decompress.s
 */
void decompress(void *src_void, void *dst_void) {
    u8 *src = (u8 *)src_void;
    u8 *dst = (u8 *)dst_void;

    u32 uncompressed_size = ((u32)src[4] << 24) | ((u32)src[5] << 16) | ((u32)src[6] << 8) | src[7];
    u32 layout_offset = ((u32)src[8] << 24) | ((u32)src[9] << 16) | ((u32)src[10] << 8) | src[11];
    u32 data_offset = ((u32)src[12] << 24) | ((u32)src[13] << 16) | ((u32)src[14] << 8) | src[15];

    u8 *layout_bits = src + 16;
    u8 *data_ptr = src + data_offset;
    u8 *layout_ptr = src + layout_offset;

    u32 dst_pos = 0;
    u32 current_layout_bit = 0;
    u32 layout_bits_buffer = 0;

    while (dst_pos < uncompressed_size) {
        // Refill layout bits buffer if empty
        if (current_layout_bit == 0) {
            layout_bits_buffer = ((u32)layout_bits[0] << 24) | ((u32)layout_bits[1] << 16) | ((u32)layout_bits[2] << 8) | layout_bits[3];
            layout_bits += 4;
            current_layout_bit = 32;
        }

        // Check the next bit (MSB first)
        u8 bit = (layout_bits_buffer >> 31) & 1;
        layout_bits_buffer <<= 1;
        current_layout_bit--;

        if (bit) {
            // 1: Copy 1 byte from data to dest
            dst[dst_pos++] = *data_ptr++;
        } else {
            // 0: Copy from dictionary (previously decompressed data)
            // Read 2 bytes from layout_ptr
            u16 val = (layout_ptr[0] << 8) | layout_ptr[1];
            layout_ptr += 2;

            // Format: 4 bits length, 12 bits offset
            // Length is (val >> 12) + 3
            // Offset is (val & 0xFFF) + 1 (backwards from current position)

            u32 length = (val >> 12) + 3;
            u32 offset = (val & 0xFFF) + 1;

            u32 copy_src = dst_pos - offset;

            for (u32 i = 0; i < length; i++) {
                dst[dst_pos++] = dst[copy_src + i];
            }
        }
    }
}
