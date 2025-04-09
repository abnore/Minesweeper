#include <stdbool.h>
#include <string.h>

#include "picasso.h"
#include "logger.h"

#define LCS_GM_BUSINESS          (1<<0) // 0x00000001  // Saturation
#define LCS_GM_GRAPHICS          (1<<1) // 0x00000002  // Relative colorimetric
#define LCS_GM_IMAGES            (1<<2) // 0x00000004  // Perceptual
#define LCS_GM_ABS_COLORIMETRIC  (1<<3) // 0x00000008

#define bits_to_bytes(x) ((x)>>3)
#define bytes_to_bits(x) ((x)<<3)

struct _bmp_load_info;

typedef enum {
    BITMAP_INVALID     = -1,
    BITMAPCOREHEADER   = 12,  // OS/2 1.x — rarely used but stb supports it
    BITMAPINFOHEADER   = 40,  // Most common, basic 24/32-bit BMPs
    BITMAPV3INFOHEADER = 56,  // Unofficial Adds color masks. Rarely seen.
    BITMAPV4HEADER     = 108, // Adds color space and gamma (optional)
    BITMAPV5HEADER     = 124  // Adds ICC profile (optional)
} bmp_header_type;

typedef enum {
    BI_RGB            = 0,  // No compression
    BI_RLE8           = 1,  // RLE 8-bit/pixel
    BI_RLE4           = 2,  // RLE 4-bit/pixel
    BI_BITFIELDS      = 3,  // Bitfields (RGB masks)
    BI_JPEG           = 4,  // JPEG compression (not common)
    BI_PNG            = 5,  // PNG compression (not common)
    BI_ALPHABITFIELDS = 6,  // Bitfields with alpha channel mask
    BI_CMYK           = 11, // CMYK uncompressed
    BI_CMYKRLE8       = 12, // RLE-8 CMYK
    BI_CMYKRLE4       = 13  // RLE-4 CMYK
} bmp_compression;

typedef enum {
    LCS_WINDOWS_COLOR_SPACE = 0x57696E20, // Native Windows color space ('Win ' in little-endian)
    LCS_sRGB                = 0x73524742, // Standard sRGB color space ('sRGB' in little-endian) — this is the most common.
    PROFILE_EMBEDDED        = 0x4D424544, // Custom ICC profile embedded in file ('MBED' in little-endian, displayed as 0x4D424544)
    PROFILE_LINKED          = 0x4C494E4B, // ICC profile is in an external file ('LINK' in little-endian)
} bmp_cs_type;

typedef struct _bmp_load_info{
    bmp image;
    bmp_header_type type;
    int channels, width, height, row_size, row_stride, size_image, comp;
    bool is_flipped, set_all_alpha;
    int rm_shift, gm_shift, bm_shift, am_shift;
    uint32_t rm, gm, bm, am;
}bmp_load_info;

const char *bmp_compression_to_str(uint32_t compression) {
    switch (compression) {
        case BI_RGB:            return "BI_RGB";
        case BI_RLE8:           return "BI_RLE8";
        case BI_RLE4:           return "BI_RLE4";
        case BI_BITFIELDS:      return "BI_BITFIELDS";
        case BI_JPEG:           return "BI_JPEG";
        case BI_PNG:            return "BI_PNG";
        case BI_ALPHABITFIELDS: return "BI_ALPHABITFIELDS";
        case BI_CMYK:           return "BI_CMYK";
        case BI_CMYKRLE8:       return "BI_CMYKRLE8";
        case BI_CMYKRLE4:       return "BI_CMYKRLE4";
        default:                return "Unknown";
    }
}
char * _print_cs_type( bmp_cs_type type)
{
    switch(type) {
        case LCS_sRGB: return "LCS_sRBG = 0x73524742";
        case LCS_WINDOWS_COLOR_SPACE: return "LCS_WINDOWS_COLOR_SPACE = 0x57696E20";
        case PROFILE_EMBEDDED: return "PROFILE_EMBEDDED = 0x4D424544";
        case PROFILE_LINKED: return "PROFILE_LINKED = 0x4C494E4B";
        default: return "Unknown";
    }
}
char *_print_header_type(bmp_header_type type)
{
    switch(type) {
        case BITMAPCOREHEADER: return "bitmapcoreheader";
        case BITMAPINFOHEADER: return "bitmapinfoheader";
        case BITMAPV3INFOHEADER: return "bitmapv3infoheader";
        case BITMAPV4HEADER: return "bitmapv4header";
        case BITMAPV5HEADER: return "bitmapv5header";
        default: return "Not supported";
    }
}


// This counts how many bits are set to 1 in the mask.
static inline int mask_bit_count(uint32_t mask) {
    int count = 0;
    while (mask) {
        count += mask & 1;
        mask >>= 1;
    }
    return count;
}
// This counts how far right the mask needs to be
// shifted to align its least significant bit to bit 0.
static inline int mask_bit_shift(uint32_t mask) {
    if (!mask) return 0;
    int shift = 0;
    while ((mask & 1) == 0) {
        mask >>= 1;
        shift++;
    }
    return shift;
}
// A pixel decoder is the part of your BMP loader that interprets raw pixel
// data using the bit masks — especially for 16-bit or 32-bit images using
// BI_BITFIELDS or BI_ALPHABITFIELDS.
static inline uint8_t decode_channel(color p, uint32_t mask)
{
    if (!mask) return 0;

    uint32_t pixel = color_to_u32(p);
    int shift = mask_bit_shift(mask);
    int bits  = mask_bit_count(mask);

    uint32_t value = (pixel & mask) >> shift;

    // Normalize if bits < 8 (e.g. 5-bit color)
    if (bits == 8) return (uint8_t)value;
    else if (bits == 0) return 0;

    // Scale up to 8-bit
    return (uint8_t)((value * 255) / ((1 << bits) - 1));
}
// Call this to decore and write the pixel, if you give it the pixel
#define decode_and_write_pixel_32bit(pixel, pixel_p) do {     \
    memcpy(&(pixel), (pixel_p), 4);                           \
    (pixel_p)[0] = decode_channel((pixel), bmp.rm);           \
    (pixel_p)[1] = decode_channel((pixel), bmp.gm);           \
    (pixel_p)[2] = decode_channel((pixel), bmp.bm);           \
    (pixel_p)[3] = decode_channel((pixel), bmp.am);           \
} while (0)



void picasso_flip_buffer_vertical(uint8_t *buffer, int width, int height, int channels)
{
    int row_size = ((width * channels + 3) / 4) * 4; // include padding!

    TRACE("Flipping buffer vertically (%dx%d) channels: %d, row_size: %d", width, height, channels, row_size);

    uint8_t temp_row[row_size];

    for (int y = 0; y < height / 2; y++) {
        int top_index = y * row_size;
        int bottom_index = (height - y - 1) * row_size;

        memcpy(temp_row, &buffer[top_index], row_size);
        memcpy(&buffer[top_index], &buffer[bottom_index], row_size);
        memcpy(&buffer[bottom_index], temp_row, row_size);
    }

    TRACE("Finished vertical flip");
}

static void picasso__extract_bitmasks(bmp_load_info *bmp) {
    if (!bmp->rm) bmp->rm = bmp->image.ih.red_mask;
    if (!bmp->gm) bmp->gm = bmp->image.ih.green_mask;
    if (!bmp->bm) bmp->bm = bmp->image.ih.blue_mask;
    if (!bmp->am) bmp->am = bmp->image.ih.alpha_mask;

    bmp->rm_shift = mask_bit_shift(bmp->rm);
    bmp->gm_shift = mask_bit_shift(bmp->gm);
    bmp->bm_shift = mask_bit_shift(bmp->bm);
    bmp->am_shift = mask_bit_shift(bmp->am);
}
static bmp_header_type picasso__decide_bmp_format(size_t *read, bmp_load_info *b, int offset, FILE *fp)
{
    *read += fread(&b->image.ih, 1, offset, fp);
    if (*read != (size_t)offset + sizeof(bmp_fh)) {
        ERROR("Corrupted BMP, aborting load read %zu", *read);
        return BITMAP_INVALID;
    }

    TRACE("header type is %s", _print_header_type(b->image.ih.size));

    return (bmp_header_type)b->image.ih.size;
}

static bmp_header_type picasso__validate_bmp(size_t *read, bmp_load_info *b, FILE *fp)
{
    bmp_header_type type;
    *read += fread(&b->image.fh, 1, sizeof(bmp_fh), fp);

    if (b->image.fh.file_type != 0x4D42) {
        ERROR("Not a valid BMP");
        fclose(fp);
        return BITMAP_INVALID;
    }

    TRACE("file size    = %u", b->image.fh.file_size);
    TRACE("data offset  = %u", b->image.fh.offset_data);

    // Peek at the DIB header size
    uint32_t dib_size = 0;
    if (fread(&dib_size, sizeof(dib_size), 1, fp) != 1) {
        ERROR("Failed to read DIB header size");
        fclose(fp);
        return BITMAP_INVALID;
    }
    fseek(fp, -4, SEEK_CUR);  // Rewind so the entire header can be read properly

    TRACE("DIB header size = %u", dib_size);

    // Decide header type based on actual size
    type = picasso__decide_bmp_format(read, b, dib_size, fp);
    if (type < 0) {
        fclose(fp);
        return BITMAP_INVALID;
    }

    return type;
}

static void picasso__parse_coreheader_fields(bmp_load_info *bmp)
{
    if (bmp->type == BITMAPCOREHEADER) {
        typedef struct {
            uint32_t size;
            uint16_t width;
            uint16_t height;
            uint16_t planes;
            uint16_t bit_count;
        } bmp_core_t;

        bmp_core_t *core = (bmp_core_t *)&bmp->image.ih;

        bmp->is_flipped = false;  // BITMAPCOREHEADER is *always* bottom-up
        bmp->width      = core->width;
        bmp->height     = core->height;
        bmp->channels   = bits_to_bytes(core->bit_count);

        TRACE("BITMAPCOREHEADER detected");
        TRACE("width         = %d", bmp->width);
        TRACE("height        = %d", bmp->height);
        TRACE("bit_count     = %d", core->bit_count);
    } else {
        // Normal parsing path for BITMAPINFOHEADER and beyond
        bmp->is_flipped = bmp->image.ih.height > 0;
        bmp->width      = bmp->image.ih.width;
        bmp->height     = PICASSO_ABS(bmp->image.ih.height);
        TRACE("width         = %d", bmp->width);
        TRACE("height        = %d", bmp->height);
        TRACE("is_flipped    = %s", bmp->is_flipped ? "true" : "false");
    }
}

static void picasso__parse_infoheader_fields(bmp_load_info *bmp, FILE *fp, size_t *read)
{
    bmp->channels    = bits_to_bytes(bmp->image.ih.bit_count);
    bmp->comp        = bmp->image.ih.compression;
    bmp->row_stride  = bmp->width * bmp->channels;
    // According to BMP spec: row_size must be aligned to 4 bytes
    bmp->row_size    = (bmp->row_stride + 3) & ~3u;
    bmp->size_image  = bmp->image.ih.size_image;

    // If BI_RGB (or BI_BITFIELDS) and size_image is 0, we must calculate it
    if (bmp->size_image == 0 && (bmp->comp == BI_RGB || bmp->comp == BI_BITFIELDS)) {
        bmp->size_image = bmp->row_size * bmp->height;
    }

    TRACE("bit_count     = %d", bmp->image.ih.bit_count);
    TRACE("compression   = %u", bmp->comp);
    TRACE("row_stride    = %u", bmp->row_stride);
    TRACE("row_size      = %u", bmp->row_size);
    TRACE("size_image    = %u", bmp->size_image);

    int mask_bytes = bmp->image.fh.offset_data - (BITMAPINFOHEADER + sizeof(bmp_fh)) ;

    if(bmp->type == BITMAPINFOHEADER){
        switch (bmp->comp) {
            case BI_RGB:
                break;

            case BI_BITFIELDS:
            case BI_ALPHABITFIELDS:
                TRACE("Offset data is %d", mask_bytes);

                *read += fread(&bmp->image.ih.red_mask,sizeof(uint32_t), 1, fp);
                *read += fread(&bmp->image.ih.green_mask,sizeof(uint32_t), 1, fp);
                *read += fread(&bmp->image.ih.blue_mask,sizeof(uint32_t), 1, fp);
                if (mask_bytes == 16) {
                    *read += fread(&bmp->image.ih.alpha_mask,sizeof(uint32_t), 1, fp);
                }
                picasso__extract_bitmasks(bmp);

                TRACE("red mask:    0x%08x", bmp->rm);
                TRACE("green mask:  0x%08x", bmp->gm);
                TRACE("blue mask:   0x%08x", bmp->bm);
                TRACE("alpha mask:  0x%08x", bmp->am);
                TRACE("red mask   shift is %d", bmp->rm_shift);
                TRACE("green mask shift is %d", bmp->gm_shift);
                TRACE("blue mask  shift is %d", bmp->bm_shift);

                if (bmp->am) {
                    TRACE("bmp am  shift is %d", bmp->am_shift);
                }
                break;

            default:
                ERROR("Compression %s not supported yet", bmp_compression_to_str(bmp->comp));
                break;
        }
    }
}

static void picasso__parse_v3_fields(bmp_load_info *bmp)
{
    picasso__extract_bitmasks(bmp);

    TRACE("red mask:    0x%08x", bmp->image.ih.red_mask);
    TRACE("green mask:  0x%08x", bmp->image.ih.green_mask);
    TRACE("blue mask:   0x%08x", bmp->image.ih.blue_mask);
    TRACE("alpha mask:  0x%08x", bmp->image.ih.alpha_mask);
}

static void picasso__parse_v4_fields(bmp_load_info *bmp)
{
    TRACE("cs_type: %s", _print_cs_type(bmp->image.ih.cs_type));
    TRACE("gamma_red:      %u", bmp->image.ih.gamma_red);
    TRACE("gamma_green:    %u", bmp->image.ih.gamma_green);
    TRACE("gamma_blue:     %u", bmp->image.ih.gamma_blue);

    // Optional: print endpoints
    for (int i = 0; i < 9; ++i) {
        TRACE("endpoint[%d]:    %d", i, bmp->image.ih.endpoints[i]);
    }
}

static void picasso__parse_v5_fields(bmp_load_info *bmp)
{
    TRACE("intent:         %u", bmp->image.ih.intent);
    TRACE("profile_data:   %u", bmp->image.ih.profile_data);
    TRACE("profile_size:   %u", bmp->image.ih.profile_size);

    if (bmp->image.ih.intent == LCS_GM_IMAGES)
        TRACE("Rendering intent: LCS_GM_IMAGES");
    else if (bmp->image.ih.intent == LCS_GM_GRAPHICS)
        TRACE("Rendering intent: LCS_GM_GRAPHICS");
    else if (bmp->image.ih.intent == LCS_GM_BUSINESS)
        TRACE("Rendering intent: LCS_GM_BUSINESS");
    else if (bmp->image.ih.intent == LCS_GM_ABS_COLORIMETRIC)
        TRACE("Rendering intent: LCS_GM_ABS_COLORIMETRIC");

    // Optional: validate ICC offset
    if (bmp->image.ih.profile_size > 0) {
        uint32_t end_of_profile = bmp->image.ih.profile_data + bmp->image.ih.profile_size;
        if (end_of_profile > bmp->image.fh.file_size) {
            WARN("Embedded profile overflows file size — ignoring");
        }
    }
}
/* Robust, and should handle all format now.. */
picasso_image *picasso_load_bmp(const char *filename)
{
    bmp_load_info bmp = {0};
    picasso_image *img = NULL;
    size_t read = 0;

    FILE *fp = fopen(filename, "rb");
    if (!fp) return NULL;

    bmp.type = picasso__validate_bmp(&read, &bmp, fp);
    if (bmp.type == BITMAP_INVALID) {
        fclose(fp);
        return img;
    }
    picasso__parse_coreheader_fields(&bmp);

    if (bmp.width > PICASSO_MAX_DIM || bmp.height > PICASSO_MAX_DIM) {
        ERROR("File too large, most likely corrupted");
        fclose(fp);
        return NULL;
    }

    if (bmp.type >= BITMAPINFOHEADER)   picasso__parse_infoheader_fields(&bmp, fp, &read);
    if (bmp.type >= BITMAPV3INFOHEADER) picasso__parse_v3_fields(&bmp);
    if (bmp.type >= BITMAPV4HEADER)     picasso__parse_v4_fields(&bmp);
    if (bmp.type >= BITMAPV5HEADER)     picasso__parse_v5_fields(&bmp);

    TRACE("Header size: %zu (fh) + %d (ih) = %zu", sizeof(bmp.image.fh), bmp.type, sizeof(bmp.image.fh) + bmp.type);
    TRACE("Actual header size %zu bytes", read);

    if(!(bmp.channels == 3 || bmp.channels == 4)) WARN("Only support bpp of 3 or 4");

    img = picasso_malloc(sizeof(picasso_image));
    {
        img->width      = bmp.width;
        img->height     = bmp.height;
        img->channels   = bmp.channels;
        img->row_stride = bmp.row_stride;
        img->pixels     = picasso_malloc(bmp.row_stride * bmp.height);
    }
    TRACE("CHANNELS = %d", img->channels);

    uint8_t *row_buf = malloc(bmp.row_size);

    for (int y = 0; y < bmp.height; ++y) {
        if ((read = fread(row_buf, 1, bmp.row_size, fp)) != (size_t)bmp.row_size) {
            ERROR("Failed to read row %d", y);
            free(row_buf);
            picasso_free(img->pixels);
            picasso_free(img);
            fclose(fp);
            return NULL;
        }

        int dest_y = bmp.is_flipped ? (bmp.height - 1 - y) : y;
        memcpy(img->pixels + dest_y * img->row_stride, row_buf, img->row_stride);
    }

    free(row_buf);
    /* Finally done reading the file */
    fclose(fp);

    bmp.set_all_alpha = true;

    color c;
    foreach_pixel_image(img, {
            if (bmp.comp == BI_BITFIELDS && bmp.channels == 4)
            {
            decode_and_write_pixel_32bit(c, pixels); // Decode from 32-bit pixel using bitmasks
            if (pixels[3] != 0) bmp.set_all_alpha = false;
            }
            else
            {
            PICASSO_SWAP(pixels[0], pixels[2]); // BGR → RGB
            }
            });

    if (bmp.set_all_alpha && img->channels == 4)
    {
        TRACE("All alpha values were zero — setting to 0xff");
        foreach_pixel_image(img, {
                pixels[3] = 0xFF;
                });
    }
    return img;
}
