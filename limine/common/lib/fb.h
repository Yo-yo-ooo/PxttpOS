#ifndef LIB__FB_H__
#define LIB__FB_H__

#include <stdint.h>
#include <stddef.h>
#include <drivers/edid.h>

struct resolution {
    uint64_t width;
    uint64_t height;
    uint16_t bpp;
};

struct fb_info {
    uint64_t framebuffer_pitch;
    uint64_t framebuffer_width;
    uint64_t framebuffer_height;
    uint16_t framebuffer_bpp;
    uint8_t memory_model;
    uint8_t red_mask_size;
    uint8_t red_mask_shift;
    uint8_t green_mask_size;
    uint8_t green_mask_shift;
    uint8_t blue_mask_size;
    uint8_t blue_mask_shift;

    uint64_t framebuffer_addr;

    struct edid_info_struct *edid;

    uint64_t mode_count;
    struct fb_info *mode_list;
};

void fb_init(struct fb_info **ret, size_t *_fbs_count,
             uint64_t target_width, uint64_t target_height, uint16_t target_bpp);

void fb_clear(struct fb_info *fb);

#endif
