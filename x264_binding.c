#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include "x264.h"

static char *X264_PRESET_TABLE[] = {
    "ultrafast", "superfast", "veryfast", "faster", "fast",
    "medium", "slow", "slower", "veryslow", "placebo"
};

x264_param_t *x264_encoder_param_create (int width, int height, int fps_num, int fps_den,
                                         int preset)
{
    x264_param_t *param = (x264_param_t*)malloc(sizeof(x264_param_t));
    x264_param_default (param);
    if (x264_param_default_preset (param, X264_PRESET_TABLE[preset], 0 ) < 0)
        return 0;
    if (x264_param_apply_profile (param, "baseline") < 0)
        return 0;
    param->i_width = width;
    param->i_height = height;
    param->i_fps_num = fps_num;
    param->i_fps_den = fps_den;
    return param;
}

void x264_encoder_param_free (x264_param_t *p)
{
    free (p);
}

x264_t *x264_encoder_open2 (x264_param_t *param)
{
    return x264_encoder_open (param);
}

x264_picture_t *x264_picture_create()
{
    return (x264_picture_t*)malloc(sizeof(x264_picture_t));
}

void x264_picture_free(x264_picture_t *pic)
{
    free(pic);
}

void x264_picture_setup (x264_picture_t *pic, int i_csp, int i_pts, int stride_0, int stride_1, int stride_2,
                         uint8_t *y, uint8_t *u, uint8_t *v)
{
    pic->i_pts = i_pts;
    pic->img.i_csp = i_csp;
    pic->img.i_plane = 3;
    pic->img.i_stride[0] = stride_0;
    pic->img.i_stride[1] = stride_1;
    pic->img.i_stride[2] = stride_2;
    pic->img.plane[0] = y;
    pic->img.plane[1] = u;
    pic->img.plane[2] = v;
    pic->img.plane[3] = 0;
}

void rgba_to_yuv420 (int width, int height, int frame_idx, uint8_t *rgba, uint8_t *y, uint8_t *u, uint8_t *v)
{
    frame_idx = frame_idx & 1;
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            uint8_t r = rgba[j * 4 + 0], g = rgba[j * 4 + 1], b = rgba[j * 4 + 2];
            y[j] = (uint8_t)(0.257f * r + 0.504f * g + 0.098f * b + 16);
        }
        if ((i & 1) == 0) {
            for (int j = 0; j < width / 2; ++j) {
                uint8_t r0 = rgba[j * 4 * 2 + 0], g0 = rgba[j * 4 * 2 + 1], b0 = rgba[j * 4 * 2 + 2];
                uint8_t r1 = rgba[j * 4 * 2 + 4], g1 = rgba[j * 4 * 2 + 5], b1 = rgba[j * 4 * 2 + 6];
                u[j] = (uint8_t)(-0.148f * r0 - 0.291f * g0 + 0.439f * b0 + 128);
                v[j] = (uint8_t)( 0.439f * r1 - 0.368f * g1 - 0.071f * b1 + 128);
            }
            u += width / 2;
            v += width / 2;
        }
        rgba += width * 4;
        y += width;
    }
}

int madvise(void *addr, size_t length, int advice) {
    return 0;
}