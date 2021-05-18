#pragma once
#include <stdint.h>

namespace {
namespace psf {

#define lsbint32(b, i) ((b)[i] | (b)[i + 1] << 8 | (b)[i + 2] << 16 | (b)[i + 3] << 24)
#define psf_assert(x) \
        if(!(x)) return -1;

class font {
public:
        uint32_t headersize; // offset of bitmaps in file
        uint32_t flags;
        uint32_t length;    // in glyphs
        uint32_t glyphsize; // number of bytes for each character
        uint32_t height;
        uint32_t width;
        uint8_t *bytes;

        constexpr inline font(uint8_t *_bytes) : bytes(_bytes) {}

private:
        constexpr inline bool parse_psf1() {
                headersize = 4;
                flags      = bytes[2];
                // bit 1 is the 512 mode
                length    = flags & 1 ? 512 : 256;
                glyphsize = bytes[3];
                height    = glyphsize;
                width     = 8;

                psf_assert(!(flags & ~3));

                return false;
        }

        constexpr inline bool parse_psf2() {
                // check version
                psf_assert(bytes[4] == 0);
                psf_assert(bytes[5] == 0);
                psf_assert(bytes[6] == 0);
                psf_assert(bytes[7] == 0);

                headersize = lsbint32(bytes, 8);
                flags      = lsbint32(bytes, 12);
                length     = lsbint32(bytes, 16);
                glyphsize  = lsbint32(bytes, 20);
                height     = lsbint32(bytes, 24);
                width      = lsbint32(bytes, 28);

                // only valid flag is 1 aka PSF2_HAS_UNICODE_TABLE
                // and it isnt obvious to me how to parse that table
                // TODO: support the unicode table
                psf_assert(flags == 0);

                psf_assert(glyphsize == height * ((width + 7) / 8));

                return false;
        }

public:
        /**
         * Parses the PSF 1 or 2 header.
         *
         * Doesn't support all modes/flags yet.
         *
         * Returns `true` on error.
         */
        constexpr inline bool parse() {
                if(bytes[0] == 0x36 && bytes[1] == 0x04) return parse_psf1();
                else if(bytes[0] == 0x72 && bytes[1] == 0xb5 && bytes[2] == 0x4a &&
                        bytes[3] == 0x86)
                        return parse_psf1();
                else
                        return true;
        }

        constexpr inline uint8_t *glyph(uint32_t c) { return &bytes[headersize + c * glyphsize]; }
};

#undef psf_assert

}
}
