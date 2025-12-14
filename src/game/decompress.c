#ifndef _LANGUAGE_C
#define _LANGUAGE_C
#endif

#include <ultra64.h>
#include "decompress.h"

// Helper to read Big Endian u32 from memory regardless of host endianness
static u32 read_u32_be(void *ptr) {
    u8 *b = (u8 *)ptr;
    return ((u32)b[0] << 24) | ((u32)b[1] << 16) | ((u32)b[2] << 8) | (u32)b[3];
}

/**
 * Portable C implementation of the Mio0 decompression algorithm.
 * Based on the assembly implementation in asm/decompress.s.
 *
 * @param mio0 Pointer to the compressed Mio0 data (Big Endian).
 * @param dest Pointer to the destination buffer.
 */
void decompress(void *mio0, void *dest) {
    u32 *header = (u32 *)mio0;

    // Read header values using big-endian safe helper
    u32 destSize = read_u32_be(&header[1]);
    u32 compressedOffset = read_u32_be(&header[2]);
    u32 uncompressedOffset = read_u32_be(&header[3]);

    u8 *layoutBits = (u8 *)mio0 + 0x10;
    u8 *compressedData = (u8 *)mio0 + compressedOffset;
    u8 *uncompressedData = (u8 *)mio0 + uncompressedOffset;
    u8 *out = (u8 *)dest;
    u8 *outEnd = out + destSize;

    // Current bits from the layout section
    u32 currentBits = 0;
    // Number of bits remaining in currentBits
    s32 bitsLeft = 0;

    while (out < outEnd) {
        if (bitsLeft == 0) {
            // Ensure we don't read past the end of the layout bits section.
            // The compressed data section immediately follows the layout bits.
            if (layoutBits + 4 > compressedData) {
                // Corrupted data, abort.
                return;
            }
            currentBits = (u32)(*layoutBits++) << 24;
            currentBits |= (u32)(*layoutBits++) << 16;
            currentBits |= (u32)(*layoutBits++) << 8;
            currentBits |= (u32)(*layoutBits++);
            bitsLeft = 32;
        }

        if (currentBits & 0x80000000) {
            // 1-bit: Uncompressed data
            *out++ = *uncompressedData++;
        } else {
            // 0-bit: Compressed data (LZSS-like reference)
            // Read 16-bit big-endian value from compressed data
            u16 data = (u16)(*compressedData++) << 8;
            data |= (u16)(*compressedData++);

            // Format: 4 bits length, 12 bits offset
            u32 length = (data >> 12) + 3;
            u32 offset = (data & 0xFFF) + 1;

            // In Mio0, offset is relative to the current output position
            u8 *lookback = out - offset;

            while (length-- > 0) {
                *out++ = *lookback++;
            }
        }

        currentBits <<= 1;
        bitsLeft--;
    }
}
