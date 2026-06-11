// config.h
#ifndef CONFIG_H
#define CONFIG_H

#include <stdint.h>

// 风格配置
typedef struct {
    char style_name[32];
    int head_shape;      // 0-2
    int body_color;      // 0-3
    int eyes;            // 0-2
    int mouth;           // 0-2
    int hat;             // 0-2
    int accessory;       // 0-3
    uint8_t color_shift; // 颜色偏移量
    float brightness;    // 亮度系数 0.5-1.5
} StyleConfig;

// 渲染配置
typedef struct {
    int width;
    int height;
    int scale_factor;
    char output_format[8]; // BMP, PNG, ASCII
} RenderConfig;

// 全局配置
typedef struct {
    StyleConfig styles[10];  // 最多10种风格
    int style_count;
    RenderConfig render;
    char last_output[256];
} AppConfig;

// 函数声明
int config_load_from_bin(const char* filename, AppConfig* config);
int config_save_to_bin(const char* filename, AppConfig* config);
void config_init_default(AppConfig* config);
void config_apply_style(StyleConfig* style, void* dna); // 应用到DNA

#endif