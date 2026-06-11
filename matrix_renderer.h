// matrix_render.h
#ifndef MATRIX_RENDER_H
#define MATRIX_RENDER_H

#include <stdint.h>

// 分辨率枚举
typedef enum {
    RES_720P = 0,   // 1280x720
    RES_1080P = 1,  // 1920x1080
    RES_2K = 2,     // 2560x1440
    RES_4K = 3      // 3840x2160
} Resolution;

// 插值算法
typedef enum {
    INTERP_NEAREST = 0,  // 最近邻（像素风）
    INTERP_BILINEAR = 1, // 双线性（平滑）
    INTERP_PIXEL = 2     // 像素放大（保持硬边）
} InterpolationMethod;

// 矩阵渲染器
typedef struct {
    int src_width;
    int src_height;
    int dst_width;
    int dst_height;
    float scale_x;
    float scale_y;
    InterpolationMethod method;
} MatrixRenderer;

// 函数声明
void matrix_init(MatrixRenderer* mr, int src_w, int src_h, Resolution res, InterpolationMethod method);
void matrix_upscale(MatrixRenderer* mr, uint8_t src[16][16], uint32_t* dst, uint8_t palette[16][4]);
void matrix_apply_effects(uint32_t* image, int width, int height, float brightness, uint8_t color_shift);
void matrix_save_to_bmp(const char* filename, uint32_t* image, int width, int height);
void matrix_save_to_ppm(const char* filename, uint32_t* image, int width, int height);

// 分辨率尺寸获取
void get_resolution_size(Resolution res, int* width, int* height);

#endif