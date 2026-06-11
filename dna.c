#include "dna.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

void dna_random(PixelDNA* dna) {
    dna->head_shape = rand() % 3;
    dna->body_color = rand() % 4;
    dna->eyes = rand() % 3;
    dna->mouth = rand() % 3;
    dna->hat = rand() % 3;
    dna->accessory = rand() % 4;
}

void dna_from_text(PixelDNA* dna, const char* desc) {
    // 默认值
    dna->head_shape = 0;
    dna->body_color = 1;
    dna->eyes = 0;
    dna->mouth = 0;
    dna->hat = 0;
    dna->accessory = 0;
    
    if (strstr(desc, "方头")) dna->head_shape = 0;
    if (strstr(desc, "圆头")) dna->head_shape = 1;
    if (strstr(desc, "三角")) dna->head_shape = 2;
    
    if (strstr(desc, "浅色")) dna->body_color = 0;
    if (strstr(desc, "深色")) dna->body_color = 1;
    if (strstr(desc, "金属")) dna->body_color = 2;
    if (strstr(desc, "鲜艳")) dna->body_color = 3;
    
    if (strstr(desc, "大眼")) dna->eyes = 1;
    if (strstr(desc, "眯眯")) dna->eyes = 2;
    
    if (strstr(desc, "笑")) dna->mouth = 1;
    if (strstr(desc, "怒")) dna->mouth = 2;
    
    if (strstr(desc, "尖帽")) dna->hat = 1;
    if (strstr(desc, "平顶")) dna->hat = 2;
    
    if (strstr(desc, "剑")) dna->accessory = 1;
    if (strstr(desc, "盾")) dna->accessory = 2;
    if (strstr(desc, "法杖")) dna->accessory = 3;
}

void dna_print(PixelDNA* dna) {
    const char* shapes[] = {"方头", "圆头", "三角"};
    const char* colors[] = {"浅色", "深色", "金属", "鲜艳"};
    const char* eyeses[] = {"点点眼", "大眼", "眯眯眼"};
    const char* mouths[] = {"平嘴", "笑", "怒"};
    const char* hats[] = {"无帽", "尖帽", "平顶帽"};
    const char* accs[] = {"无武器", "剑", "盾", "法杖"};
    
    printf("DNA: %s %s %s %s %s %s\n", 
           shapes[dna->head_shape], colors[dna->body_color], eyeses[dna->eyes],
           mouths[dna->mouth], hats[dna->hat], accs[dna->accessory]);
}