// config.c
#include "config.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void config_init_default(AppConfig* config) {
    memset(config, 0, sizeof(AppConfig));
    
    // 风格1: 经典战士
    strcpy(config->styles[0].style_name, "经典战士");
    config->styles[0].head_shape = 0;
    config->styles[0].body_color = 1;
    config->styles[0].eyes = 1;
    config->styles[0].mouth = 1;
    config->styles[0].hat = 1;
    config->styles[0].accessory = 1;
    config->styles[0].color_shift = 0;
    config->styles[0].brightness = 1.0;
    
    // 风格2: 可爱法师
    strcpy(config->styles[1].style_name, "可爱法师");
    config->styles[1].head_shape = 1;
    config->styles[1].body_color = 3;
    config->styles[1].eyes = 1;
    config->styles[1].mouth = 1;
    config->styles[1].hat = 2;
    config->styles[1].accessory = 3;
    config->styles[1].color_shift = 2;
    config->styles[1].brightness = 1.2;
    
    // 风格3: 暗黑骑士
    strcpy(config->styles[2].style_name, "暗黑骑士");
    config->styles[2].head_shape = 2;
    config->styles[2].body_color = 1;
    config->styles[2].eyes = 2;
    config->styles[2].mouth = 2;
    config->styles[2].hat = 0;
    config->styles[2].accessory = 2;
    config->styles[2].color_shift = 1;
    config->styles[2].brightness = 0.8;
    
    config->style_count = 3;
    
    // 默认渲染配置
    config->render.width = 1920;
    config->render.height = 1080;
    config->render.scale_factor = 120; // 16x16缩放到1920x1080
    strcpy(config->render.output_format, "BMP");
    strcpy(config->last_output, "output.bmp");
}

int config_load_from_bin(const char* filename, AppConfig* config) {
    FILE* f = fopen(filename, "rb");
    if (!f) {
        printf("配置文件不存在，使用默认配置\n");
        config_init_default(config);
        return 0;
    }
    
    // 读取文件头
    char magic[4];
    fread(magic, 1, 4, f);
    if (memcmp(magic, "PBCF", 4) != 0) {
        printf("无效的配置文件格式\n");
        fclose(f);
        return -1;
    }
    
    uint32_t version;
    fread(&version, 1, 4, f);
    
    fread(&config->style_count, 1, 4, f);
    for (int i = 0; i < config->style_count && i < 10; i++) {
        fread(&config->styles[i], sizeof(StyleConfig), 1, f);
    }
    
    fread(&config->render, sizeof(RenderConfig), 1, f);
    fread(config->last_output, 1, 256, f);
    
    fclose(f);
    printf("✓ 加载配置文件: %s (版本: %d, 风格数: %d)\n", filename, version, config->style_count);
    return 0;
}

int config_save_to_bin(const char* filename, AppConfig* config) {
    FILE* f = fopen(filename, "wb");
    if (!f) return -1;
    
    // 写入文件头
    fwrite("PBCF", 1, 4, f);
    uint32_t version = 1;
    fwrite(&version, 1, 4, f);
    
    fwrite(&config->style_count, 1, 4, f);
    for (int i = 0; i < config->style_count; i++) {
        fwrite(&config->styles[i], sizeof(StyleConfig), 1, f);
    }
    
    fwrite(&config->render, sizeof(RenderConfig), 1, f);
    fwrite(config->last_output, 1, 256, f);
    
    fclose(f);
    printf("✓ 保存配置文件: %s\n", filename);
    return 0;
}

void config_apply_style(StyleConfig* style, void* dna_ptr) {
    // 这个函数需要包含dna.h，实际使用时在调用方处理
    // 这里仅作为接口示例
}