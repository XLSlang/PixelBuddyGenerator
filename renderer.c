// renderer.c
#include "renderer.h"
#include "assets.h"  // ⬅️ 添加这一行
#include <string.h>

static const uint8_t base_template[16][16] = {
    {0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0},
    {0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0},
    {0,0,0,0,1,1,2,2,2,2,1,1,0,0,0,0},
    {0,0,0,0,1,2,2,2,2,2,2,1,0,0,0,0},
    {0,0,0,0,1,2,2,0,0,2,2,1,0,0,0,0},
    {0,0,0,0,1,2,2,0,0,2,2,1,0,0,0,0},
    {0,0,0,0,1,2,2,2,2,2,2,1,0,0,0,0},
    {0,0,0,0,0,1,2,2,2,2,1,0,0,0,0,0},
    {0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0},
    {0,0,0,0,0,1,0,0,0,0,1,0,0,0,0,0},
    {0,0,0,0,1,1,0,0,0,0,1,1,0,0,0,0},
    {0,0,0,0,1,0,0,0,0,0,0,1,0,0,0,0},
    {0,0,0,0,1,0,0,0,0,0,0,1,0,0,0,0},
    {0,0,0,0,1,0,0,0,0,0,0,1,0,0,0,0},
    {0,0,0,0,0,1,0,0,0,0,1,0,0,0,0,0},
    {0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0},
};

// 新函数：初始化缓冲区
void renderer_init(RenderBuffer* buffer) {
    memset(buffer->pixels, 0, sizeof(buffer->pixels));
    memset(buffer->ascii, 0, sizeof(buffer->ascii));
}

// 新函数：应用模板
void renderer_apply_template(RenderBuffer* buffer) {
    for (int y = 0; y < 16; y++) {
        for (int x = 0; x < 16; x++) {
            buffer->pixels[y][x] = base_template[y][x];
        }
    }
}

// 新函数：应用身体颜色
void renderer_apply_body_color(RenderBuffer* buffer, int body_color) {
    uint8_t body_color_idx = 2 + body_color;
    if (body_color_idx > 15) body_color_idx = 15;
    
    for (int y = 0; y < 16; y++) {
        for (int x = 0; x < 16; x++) {
            if (buffer->pixels[y][x] == 2) {
                buffer->pixels[y][x] = body_color_idx;
            }
        }
    }
}

// 新函数：应用帽子
void renderer_apply_hat(RenderBuffer* buffer, int hat) {
    if (hat == 1) {
        for (int y = 0; y < 5; y++) {
            for (int x = 6 - y/2; x <= 9 + y/2; x++) {
                if (x >= 0 && x < 16 && y < 16) buffer->pixels[y][x] = 7;
            }
        }
    } else if (hat == 2) {
        for (int x = 4; x < 12; x++) buffer->pixels[1][x] = 9;
        for (int y = 2; y < 4; y++) {
            buffer->pixels[y][4] = 9;
            buffer->pixels[y][11] = 9;
        }
    }
}

// 新函数：应用眼睛
void renderer_apply_eyes(RenderBuffer* buffer, int eyes) {
    int eye_y = 7;
    if (eyes == 0) {
        buffer->pixels[eye_y][6] = 1;
        buffer->pixels[eye_y][9] = 1;
    } else if (eyes == 1) {
        buffer->pixels[eye_y][5] = 1;
        buffer->pixels[eye_y][6] = 7;
        buffer->pixels[eye_y][8] = 7;
        buffer->pixels[eye_y][9] = 1;
    } else {
        buffer->pixels[eye_y][6] = 8;
        buffer->pixels[eye_y][9] = 8;
    }
}

// 新函数：应用嘴巴
void renderer_apply_mouth(RenderBuffer* buffer, int mouth) {
    int mouth_y = 9;
    if (mouth == 0) {
        buffer->pixels[mouth_y][7] = 8;
        buffer->pixels[mouth_y][8] = 8;
    } else if (mouth == 1) {
        buffer->pixels[mouth_y][6] = 1;
        buffer->pixels[mouth_y][7] = 8;
        buffer->pixels[mouth_y][8] = 8;
        buffer->pixels[mouth_y][9] = 1;
    } else {
        buffer->pixels[mouth_y][6] = 8;
        buffer->pixels[mouth_y][7] = 1;
        buffer->pixels[mouth_y][8] = 1;
        buffer->pixels[mouth_y][9] = 8;
    }
}

// 新函数：应用武器
void renderer_apply_weapon(RenderBuffer* buffer, int accessory) {
    if (accessory == 1) {
        for (int y = 8; y < 15; y++) buffer->pixels[y][12] = 9;
        buffer->pixels[7][12] = 7;
    } else if (accessory == 2) {
        for (int y = 9; y < 13; y++) {
            for (int x = 11; x < 14; x++) {
                buffer->pixels[y][x] = 9;
            }
        }
    } else if (accessory == 3) {
        buffer->pixels[5][12] = 7;
        for (int y = 6; y < 14; y++) buffer->pixels[y][12] = 9;
    }
}

// 新函数：渲染全部
void renderer_render_all(RenderBuffer* buffer, const PixelDNA* dna) {
    renderer_apply_template(buffer);
    renderer_apply_body_color(buffer, dna->body_color);
    renderer_apply_hat(buffer, dna->hat);
    renderer_apply_eyes(buffer, dna->eyes);
    renderer_apply_mouth(buffer, dna->mouth);
    renderer_apply_weapon(buffer, dna->accessory);
}

// 新函数：转ASCII
void renderer_to_ascii(RenderBuffer* buffer, int body_color) {
    const char body_chars[] = {'.', '#', '@', '%'};
    
    for (int y = 0; y < 16; y++) {
        for (int x = 0; x < 16; x++) {
            int idx = buffer->pixels[y][x];
            if (idx == 0) {
                buffer->ascii[y][x] = ' ';
            } else if (idx == 1) {
                buffer->ascii[y][x] = '.';
            } else if (idx >= 2 && idx <= 5) {
                buffer->ascii[y][x] = body_chars[body_color % 4];
            } else if (idx == 7) {
                buffer->ascii[y][x] = '^';
            } else if (idx == 8) {
                buffer->ascii[y][x] = 'o';
            } else if (idx == 9) {
                buffer->ascii[y][x] = '#';
            } else {
                buffer->ascii[y][x] = '?';
            }
        }
    }
}

// 旧函数（保持兼容）
void render_to_pixels(PixelDNA* dna, uint8_t pixels[16][16]) {
    RenderBuffer buffer;
    renderer_render_all(&buffer, dna);
    memcpy(pixels, buffer.pixels, sizeof(buffer.pixels));
}

void render_to_ascii(PixelDNA* dna, char ascii[16][16]) {
    RenderBuffer buffer;
    renderer_render_all(&buffer, dna);
    renderer_to_ascii(&buffer, dna->body_color);
    memcpy(ascii, buffer.ascii, sizeof(buffer.ascii));
}