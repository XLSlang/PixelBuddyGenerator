#ifndef DNA_H
#define DNA_H

#include <stdint.h>

typedef struct {
    int head_shape;   // 0=方头 1=圆头 2=三角头
    int body_color;   // 0=浅 1=深 2=金属 3=鲜艳
    int eyes;         // 0=点点 1=大眼 2=眯眯
    int mouth;        // 0=平 1=笑 2=怒
    int hat;          // 0=无 1=尖帽 2=平顶
    int accessory;    // 0=无 1=剑 2=盾 3=法杖
} PixelDNA;

void dna_random(PixelDNA* dna);
void dna_from_text(PixelDNA* dna, const char* desc);
void dna_print(PixelDNA* dna);

#endif