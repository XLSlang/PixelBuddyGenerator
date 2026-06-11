// renderer.h
#ifndef RENDERER_H
#define RENDERER_H

#include <stdint.h>
#include "dna.h"

// 渲染缓冲区结构体
typedef struct {
    uint8_t pixels[16][16];
    char ascii[16][16];
} RenderBuffer;

// 新版本的渲染函数
void renderer_init(RenderBuffer* buffer);
void renderer_apply_template(RenderBuffer* buffer);
void renderer_apply_body_color(RenderBuffer* buffer, int body_color);
void renderer_apply_hat(RenderBuffer* buffer, int hat);
void renderer_apply_eyes(RenderBuffer* buffer, int eyes);
void renderer_apply_mouth(RenderBuffer* buffer, int mouth);
void renderer_apply_weapon(RenderBuffer* buffer, int accessory);
void renderer_render_all(RenderBuffer* buffer, const PixelDNA* dna);
void renderer_to_ascii(RenderBuffer* buffer, int body_color);

// 旧版本兼容（可选）
void render_to_pixels(PixelDNA* dna, uint8_t pixels[16][16]);
void render_to_ascii(PixelDNA* dna, char ascii[16][16]);

#endif