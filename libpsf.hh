#pragma once
#include <stdint.h>

namespace {
namespace psf {

#define lsbint32(b, i) ((b)[i] | (b)[i + 1] << 8 | (b)[i + 2] << 16 | (b)[i + 3] << 24)
#define psf_assert(x) \
        if(!(x)) return -1;

class file {
public:
        uint32_t headersize; // offset of bitmaps in file
        uint32_t flags;
        uint32_t length;    // in glyphs
        uint32_t glyphsize; // number of bytes for each character
        uint32_t height;
        uint32_t width;
        uint8_t *bytes;

        constexpr inline file(uint8_t *_bytes) : bytes(_bytes) {}

        /**
         * Currently only supports PSF2, TODO: PSF 1 support
         */
        constexpr inline int parse_header(file *psf) {
                // if(bytes[0] == 0x36 && bytes[1] == 0x04):
                //         psf1

                uint8_t *bytes = psf->bytes;

                // check header
                psf_assert(bytes[0] == 0x72);
                psf_assert(bytes[1] == 0xb5);
                psf_assert(bytes[2] == 0x4a);
                psf_assert(bytes[3] == 0x86);

                // check version
                psf_assert(bytes[4] == 0);
                psf_assert(bytes[5] == 0);
                psf_assert(bytes[6] == 0);
                psf_assert(bytes[7] == 0);

                psf->headersize = lsbint32(bytes, 8);
                psf->flags      = lsbint32(bytes, 12);
                psf->length     = lsbint32(bytes, 16);
                psf->glyphsize  = lsbint32(bytes, 20);
                psf->height     = lsbint32(bytes, 24);
                psf->width      = lsbint32(bytes, 28);

                // only valid flag 1 is PSF2_HAS_UNICODE_TABLE and it isnt obvious to me
                // how to parse that table
                psf_assert(psf->flags == 0);

                psf_assert(psf->glyphsize == psf->height * ((psf->width + 7) / 8));

                return 0;
        }

        constexpr inline uint8_t *get_glyph(file *psf, uint8_t *bytes, uint32_t c) {
                return &bytes[psf->headersize + c * psf->glyphsize];
        }
};

#undef psf_assert

}
}
