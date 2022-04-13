#pragma once
#include <stdint.h>

namespace {
namespace psf {

class font {
public:
        uint_fast32_t headersize;
        uint_fast32_t flags;
        uint_fast32_t nglyphs;
        uint_fast32_t bytesperglyph;
        uint_fast32_t height;
        uint_fast32_t width;
        uint8_t      *bytes;

        constexpr inline font(uint8_t *_bytes) : bytes(_bytes) {}

private:
        constexpr inline bool parse_psf1() {
                flags  = bytes[2];
                height = bytesperglyph = bytes[3];
                // 512 if flag bit 1 is set, 256 otherwise
                nglyphs    = (flags & 1) << 8 | 0xff;
                headersize = 4;
                width      = 8;

                return (flags & ~3);
        }

        constexpr inline uint_fast32_t lsbint32(uint_fast16_t i) {
                i *= 4;
                // I checked the C++ Operator Precedence and this works
                return bytes[i] | bytes[i + 1] << 8 | bytes[i + 2] << 16 | bytes[i + 3] << 24;
        }

        constexpr inline bool parse_psf2() {
                // check version
                if(bytes[4] || bytes[5] || bytes[6] || bytes[7]) return true;

                headersize    = lsbint32(2);
                flags         = lsbint32(3);
                nglyphs       = lsbint32(4);
                bytesperglyph = lsbint32(5);
                height        = lsbint32(6);
                width         = lsbint32(7);

                // only valid flag is 1 aka PSF2_HAS_UNICODE_TABLE
                // and it isnt obvious to me how to parse that table
                // TODO: support the unicode table
                if(flags) return true;

                return bytesperglyph != height * ((width + 7) / 8);
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

        constexpr inline uint8_t *operator[](uint32_t c) {
                return &bytes[headersize + c * bytesperglyph];
        }
};

}
}
