// matrix_renderer.c
#include "matrix_renderer.h"
#include "assets.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

void get_resolution_size(Resolution res, int* width, int* height) {
    switch(res) {
        case RES_720P:
            *width = 1280;
            *height = 720;
            break;
        case RES_1080P:
            *width = 1920;
            *height = 1080;
            break;
        case RES_2K:
            *width = 2560;
            *height = 1440;
            break;
        case RES_4K:
            *width = 3840;
            *height = 2160;
            break;
    }
}

void matrix_init(MatrixRenderer* mr, int src_w, int src_h, Resolution res, InterpolationMethod method) {
    mr->src_width = src_w;
    mr->src_height = src_h;
    get_resolution_size(res, &mr->dst_width, &mr->dst_height);
    mr->scale_x = (float)mr->dst_width / src_w;
    mr->scale_y = (float)mr->dst_height / src_h;
    mr->method = method;
    
    printf("矩阵渲染器初始化: %dx%d -> %dx%d (缩放: %.2fx, 插值: %d)\n",
           src_w, src_h, mr->dst_width, mr->dst_height, mr->scale_x, method);
}

// 最近邻插值（像素风效果）
void upscale_nearest(MatrixRenderer* mr, uint8_t src[16][16], uint32_t* dst, uint8_t palette[16][4]) {
    for (int dy = 0; dy < mr->dst_height; dy++) {
        int sy = (int)(dy / mr->scale_y);
        if (sy >= mr->src_height) sy = mr->src_height - 1;
        
        for (int dx = 0; dx < mr->dst_width; dx++) {
            int sx = (int)(dx / mr->scale_x);
            if (sx >= mr->src_width) sx = mr->src_width - 1;
            
            uint8_t color_idx = src[sy][sx];
            dst[dy * mr->dst_width + dx] = 
                (palette[color_idx][3] << 24) |
                (palette[color_idx][0] << 16) |
                (palette[color_idx][1] << 8) |
                palette[color_idx][2];
        }
    }
}

// 双线性插值（平滑效果）
void upscale_bilinear(MatrixRenderer* mr, uint8_t src[16][16], uint32_t* dst, uint8_t palette[16][4]) {
    for (int dy = 0; dy < mr->dst_height; dy++) {
        float fy = dy / mr->scale_y;
        int sy = (int)fy;
        float ty = fy - sy;
        if (sy >= mr->src_height - 1) sy = mr->src_height - 2;
        
        for (int dx = 0; dx < mr->dst_width; dx++) {
            float fx = dx / mr->scale_x;
            int sx = (int)fx;
            float tx = fx - sx;
            if (sx >= mr->src_width - 1) sx = mr->src_width - 2;
            
            // 获取四个相邻像素的颜色
            uint8_t c00 = src[sy][sx];
            uint8_t c10 = src[sy][sx+1];
            uint8_t c01 = src[sy+1][sx];
            uint8_t c11 = src[sy+1][sx+1];
            
            // 对RGB三个通道分别插值
            for (int c = 0; c < 3; c++) {
                float v00 = palette[c00][c];
                float v10 = palette[c10][c];
                float v01 = palette[c01][c];
                float v11 = palette[c11][c];
                
                float v0 = v00 * (1-tx) + v10 * tx;
                float v1 = v01 * (1-tx) + v11 * tx;
                float val = v0 * (1-ty) + v1 * ty;
                
                uint8_t result = (uint8_t)val;
                uint32_t* pixel = &dst[dy * mr->dst_width + dx];
                *pixel |= (result << (16 - c*8));
            }
            dst[dy * mr->dst_width + dx] |= 0xFF000000;
        }
    }
}

// 像素放大（硬边，复古游戏风格）
void upscale_pixel(MatrixRenderer* mr, uint8_t src[16][16], uint32_t* dst, uint8_t palette[16][4]) {
    int block_w = mr->dst_width / mr->src_width;
    int block_h = mr->dst_height / mr->src_height;
    
    for (int sy = 0; sy < mr->src_height; sy++) {
        for (int sx = 0; sx < mr->src_width; sx++) {
            uint32_t color = (palette[src[sy][sx]][3] << 24) |
                            (palette[src[sy][sx]][0] << 16) |
                            (palette[src[sy][sx]][1] << 8) |
                            palette[src[sy][sx]][2];
            
            for (int dy = 0; dy < block_h; dy++) {
                for (int dx = 0; dx < block_w; dx++) {
                    int y = sy * block_h + dy;
                    int x = sx * block_w + dx;
                    if (y < mr->dst_height && x < mr->dst_width) {
                        dst[y * mr->dst_width + x] = color;
                    }
                }
            }
        }
    }
}

void matrix_upscale(MatrixRenderer* mr, uint8_t src[16][16], uint32_t* dst, uint8_t palette[16][4]) {
    switch(mr->method) {
        case INTERP_NEAREST:
            upscale_nearest(mr, src, dst, palette);
            break;
        case INTERP_BILINEAR:
            upscale_bilinear(mr, src, dst, palette);
            break;
        case INTERP_PIXEL:
            upscale_pixel(mr, src, dst, palette);
            break;
    }
}

void matrix_apply_effects(uint32_t* image, int width, int height, float brightness, uint8_t color_shift) {
    for (int i = 0; i < width * height; i++) {
        uint32_t pixel = image[i];
        uint8_t r = (pixel >> 16) & 0xFF;
        uint8_t g = (pixel >> 8) & 0xFF;
        uint8_t b = pixel & 0xFF;
        
        // 应用亮度
        r = (uint8_t)(r * brightness);
        g = (uint8_t)(g * brightness);
        b = (uint8_t)(b * brightness);
        
        // 应用颜色偏移（色调旋转）
        if (color_shift > 0) {
            uint8_t temp = r;
            r = g;
            g = b;
            b = temp;
        }
        
        image[i] = 0xFF000000 | (r << 16) | (g << 8) | b;
    }
}

void matrix_save_to_bmp(const char* filename, uint32_t* image, int width, int height) {
    int stride = (width * 3 + 3) & ~3;
    int image_size = stride * height;
    
    uint8_t file_header[14] = {
        0x42, 0x4D,
        (image_size + 54) & 0xFF, (image_size + 54) >> 8 & 0xFF,
        (image_size + 54) >> 16 & 0xFF, (image_size + 54) >> 24 & 0xFF,
        0, 0, 0, 0,
        54, 0, 0, 0
    };
    
    uint8_t info_header[40] = {
        40, 0, 0, 0,
        width & 0xFF, (width >> 8) & 0xFF, (width >> 16) & 0xFF, (width >> 24) & 0xFF,
        height & 0xFF, (height >> 8) & 0xFF, (height >> 16) & 0xFF, (height >> 24) & 0xFF,
        1, 0, 24, 0, 0, 0, 0, 0,
        image_size & 0xFF, (image_size >> 8) & 0xFF,
        (image_size >> 16) & 0xFF, (image_size >> 24) & 0xFF,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    };
    
    uint8_t* pixel_data = (uint8_t*)malloc(image_size);
    memset(pixel_data, 0, image_size);
    
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            uint32_t color = image[(height - 1 - y) * width + x];
            int offset = y * stride + x * 3;
            pixel_data[offset + 0] = (color >> 16) & 0xFF;
            pixel_data[offset + 1] = (color >> 8) & 0xFF;
            pixel_data[offset + 2] = color & 0xFF;
        }
    }
    
    FILE* f = fopen(filename, "wb");
    if (f) {
        fwrite(file_header, 1, 14, f);
        fwrite(info_header, 1, 40, f);
        fwrite(pixel_data, 1, image_size, f);
        fclose(f);
        printf("✓ 已保存: %s (%dx%d)\n", filename, width, height);
    }
    free(pixel_data);
}