#include <ultra64.h>

void decompress(void *src, void *dest) {
    u8 *src_bytes = (u8 *)src;
    u8 *dest_bytes = (u8 *)dest;

    // Read header (Big Endian in ROM, ensure portable read if needed)
    u32 dest_size     = ((u32)src_bytes[4] << 24) | ((u32)src_bytes[5] << 16) | ((u32)src_bytes[6] << 8) | src_bytes[7];
    u32 comp_offset   = ((u32)src_bytes[8] << 24) | ((u32)src_bytes[9] << 16) | ((u32)src_bytes[10] << 8) | src_bytes[11];
    u32 uncomp_offset = ((u32)src_bytes[12] << 24) | ((u32)src_bytes[13] << 16) | ((u32)src_bytes[14] << 8) | src_bytes[15];

    u8 *comp_ptr = src_bytes + comp_offset;
    u8 *uncomp_ptr = src_bytes + uncomp_offset;
    u8 *map_ptr = src_bytes + 0x10;

    u32 dest_pos = 0;
    u32 map_bits = 0;
    u32 map_bit_count = 0;

    while (dest_pos < dest_size) {
        // Refill map bits
        if (map_bit_count == 0) {
            map_bits = ((u32)map_ptr[0] << 24) | ((u32)map_ptr[1] << 16) | ((u32)map_ptr[2] << 8) | map_ptr[3];
            map_ptr += 4;
            map_bit_count = 32;
        }

        // Check MSB of map
        if (map_bits & 0x80000000) {
            // Bit 1: Copy uncompressed byte
            dest_bytes[dest_pos++] = *uncomp_ptr++;
        } else {
            // Bit 0: LZ Copy
            u16 val = ((u16)comp_ptr[0] << 8) | comp_ptr[1];
            comp_ptr += 2;

            u32 length = (val >> 12) + 3;
            u32 offset = (val & 0xFFF) + 1;

            // Copy with RLE support (src overlaps dest)
            u8 *copy_src = dest_bytes + dest_pos - offset;
            for (u32 i = 0; i < length; i++) {
                dest_bytes[dest_pos++] = copy_src[i];
            }
        }
        map_bits <<= 1;
        map_bit_count--;
    }
}
