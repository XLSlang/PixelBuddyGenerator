#include "exporter.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const uint32_t palette[16] = {
    0x00000000,  // 0: 透明
    0xFFD2B48C,  // 1: 皮肤
    0xFF4169E1,  // 2: 蓝色
    0xFF8B4513,  // 3: 棕色
    0xFFFFD700,  // 4: 金色
    0xFFDC143C,  // 5: 红色
    0xFF00FF7F,  // 6: 绿色
    0xFFFFFFFF,  // 7: 白色
    0xFF000000,  // 8: 黑色
    0xFF808080,  // 9: 灰色
    0xFFFFA500,  // 10: 橙色
    0xFF9370DB,  // 11: 紫色
    0xFF00CED1,  // 12: 青色
    0xFFFF69B4,  // 13: 粉色
    0xFFA0522D,  // 14: 棕色
    0xFFF0E68C,  // 15: 卡其色
};

void export_to_bmp(const char* filename, uint8_t pixels[16][16]) {
    int width = 16, height = 16;
    int stride = (width * 4 + 3) & ~3;
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
        16, 0, 0, 0,
        16, 0, 0, 0,
        1, 0, 32, 0, 0, 0, 0, 0,
        image_size & 0xFF, image_size >> 8 & 0xFF,
        image_size >> 16 & 0xFF, image_size >> 24 & 0xFF,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    };
    
    uint8_t* pixel_data = (uint8_t*)malloc(image_size);
    if (!pixel_data) return;
    memset(pixel_data, 0, image_size);
    
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            uint32_t color = palette[pixels[height - 1 - y][x]];
            int offset = y * stride + x * 4;
            pixel_data[offset + 0] = (color >> 16) & 0xFF;
            pixel_data[offset + 1] = (color >> 8) & 0xFF;
            pixel_data[offset + 2] = (color >> 0) & 0xFF;
            pixel_data[offset + 3] = (color >> 24) & 0xFF;
        }
    }
    
    FILE* f = fopen(filename, "wb");
    if (f) {
        fwrite(file_header, 1, 14, f);
        fwrite(info_header, 1, 40, f);
        fwrite(pixel_data, 1, image_size, f);
        fclose(f);
        printf("Exported: %s\n", filename);
    }
    free(pixel_data);
}

void export_ascii_to_stdout(char ascii[16][16]) {
    for (int y = 0; y < 16; y++) {
        for (int x = 0; x < 16; x++) {
            printf("%c", ascii[y][x]);
        }
        printf("\n");
    }
}