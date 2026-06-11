// dna_inference.h
#ifndef DNA_INFERENCE_H
#define DNA_INFERENCE_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

// 模型结构
typedef struct {
    float* W1;  // [6][32]
    float* b1;  // [32]
    float* W2;  // [32][16]
    float* b2;  // [16]
    float* W3;  // [16][6]
    float* b3;  // [6]
    int input_dim;
    int hidden1_dim;
    int hidden2_dim;
    int output_dim;
    int is_loaded;
} DNAModel;

// 全局模型实例
static DNAModel g_model = {0};
static int g_model_loaded = 0;

// 从bin文件加载模型
int dna_model_load(const char* bin_path, DNAModel* model) {
    FILE* f = fopen(bin_path, "rb");
    if (!f) {
        return -1;
    }
    
    // 读取魔数
    char magic[5] = {0};
    fread(magic, 1, 4, f);
    if (strcmp(magic, "DNAM") != 0) {
        fclose(f);
        return -1;
    }
    
    // 读取版本
    uint32_t version;
    fread(&version, 1, 4, f);
    
    // 读取网络结构
    fread(&model->input_dim, 4, 1, f);
    fread(&model->hidden1_dim, 4, 1, f);
    fread(&model->hidden2_dim, 4, 1, f);
    model->output_dim = 6;
    
    // 分配内存
    int w1_size = model->input_dim * model->hidden1_dim;
    int w2_size = model->hidden1_dim * model->hidden2_dim;
    int w3_size = model->hidden2_dim * model->output_dim;
    
    model->W1 = (float*)malloc(w1_size * sizeof(float));
    model->b1 = (float*)malloc(model->hidden1_dim * sizeof(float));
    model->W2 = (float*)malloc(w2_size * sizeof(float));
    model->b2 = (float*)malloc(model->hidden2_dim * sizeof(float));
    model->W3 = (float*)malloc(w3_size * sizeof(float));
    model->b3 = (float*)malloc(model->output_dim * sizeof(float));
    
    if (!model->W1 || !model->b1 || !model->W2 || !model->b2 || !model->W3 || !model->b3) {
        fclose(f);
        return -1;
    }
    
    // 读取权重
    fread(model->W1, sizeof(float), w1_size, f);
    fread(model->b1, sizeof(float), model->hidden1_dim, f);
    fread(model->W2, sizeof(float), w2_size, f);
    fread(model->b2, sizeof(float), model->hidden2_dim, f);
    fread(model->W3, sizeof(float), w3_size, f);
    fread(model->b3, sizeof(float), model->output_dim, f);
    
    fclose(f);
    model->is_loaded = 1;
    return 0;
}

// 全局初始化
int dna_model_global_init(const char* bin_path) {
    if (!g_model_loaded) {
        int ret = dna_model_load(bin_path, &g_model);
        if (ret == 0) {
            g_model_loaded = 1;
        }
    }
    return g_model_loaded ? 0 : -1;
}

// 检查模型是否已加载
int dna_model_is_loaded(void) {
    return g_model_loaded;
}

// 前向推理
int dna_model_predict(DNAModel* model, const float* input, float* output) {
    if (!model->is_loaded) return -1;
    
    // 临时缓冲区
    float h1[32];
    float h2[16];
    
    // 第一层: input(6) -> h1(32)
    for (int i = 0; i < model->hidden1_dim; i++) {
        float sum = model->b1[i];
        for (int j = 0; j < model->input_dim; j++) {
            sum += input[j] * model->W1[j * model->hidden1_dim + i];
        }
        h1[i] = tanhf(sum);
    }
    
    // 第二层: h1(32) -> h2(16)
    for (int i = 0; i < model->hidden2_dim; i++) {
        float sum = model->b2[i];
        for (int j = 0; j < model->hidden1_dim; j++) {
            sum += h1[j] * model->W2[j * model->hidden2_dim + i];
        }
        h2[i] = tanhf(sum);
    }
    
    // 输出层: h2(16) -> output(6)
    for (int i = 0; i < model->output_dim; i++) {
        float sum = model->b3[i];
        for (int j = 0; j < model->hidden2_dim; j++) {
            sum += h2[j] * model->W3[j * model->output_dim + i];
        }
        output[i] = sum;
        if (output[i] < 0) output[i] = 0;
        if (output[i] > 1) output[i] = 1;
    }
    
    return 0;
}

// 全局推理
int dna_model_predict_global(const float* input, float* output) {
    if (!g_model_loaded) return -1;
    return dna_model_predict(&g_model, input, output);
}

// 从文本提取特征（6维）
void extract_features(const char* text, float* features) {
    memset(features, 0, 6 * sizeof(float));
    
    char text_lower[256];
    strcpy(text_lower, text);
    // 转小写（简化版）
    for (char* p = text_lower; *p; p++) {
        if (*p >= 'A' && *p <= 'Z') *p = *p + 32;
    }
    
    // 头型特征
    if (strstr(text_lower, "方") || strstr(text_lower, "战士") || strstr(text_lower, "square")) 
        features[0] = 1.0f;
    if (strstr(text_lower, "圆") || strstr(text_lower, "法师") || strstr(text_lower, "可爱") ||
        strstr(text_lower, "round") || strstr(text_lower, "circle"))
        features[1] = 1.0f;
    if (strstr(text_lower, "三角") || strstr(text_lower, "恐龙") || strstr(text_lower, "尖") ||
        strstr(text_lower, "triangle"))
        features[2] = 1.0f;
    
    // 颜色特征
    if (strstr(text_lower, "浅") || strstr(text_lower, "白") || strstr(text_lower, "可爱") ||
        strstr(text_lower, "light") || strstr(text_lower, "white"))
        features[3] = 1.0f;
    if (strstr(text_lower, "深") || strstr(text_lower, "黑") || strstr(text_lower, "悲伤") ||
        strstr(text_lower, "dark") || strstr(text_lower, "black") || strstr(text_lower, "sad"))
        features[4] = 1.0f;
    
    // 情感/装备特征
    if (strstr(text_lower, "快乐") || strstr(text_lower, "笑") || strstr(text_lower, "法师") ||
        strstr(text_lower, "happy") || strstr(text_lower, "joy"))
        features[5] = 1.0f;
    if (strstr(text_lower, "悲伤") || strstr(text_lower, "哭") || strstr(text_lower, "sad") ||
        strstr(text_lower, "cry"))
        features[5] = 0.5f;
    if (strstr(text_lower, "愤怒") || strstr(text_lower, "战士") || strstr(text_lower, "怒") ||
        strstr(text_lower, "angry"))
        features[5] = 0.0f;
    if (strstr(text_lower, "剑") || strstr(text_lower, "sword"))
        features[5] = 0.3f;
    
    // 归一化
    float sum = 0;
    for (int i = 0; i < 6; i++) sum += features[i];
    if (sum > 0) {
        for (int i = 0; i < 6; i++) features[i] /= sum;
    }
}

// 释放模型
void dna_model_free(DNAModel* model) {
    if (model && model->is_loaded) {
        free(model->W1);
        free(model->b1);
        free(model->W2);
        free(model->b2);
        free(model->W3);
        free(model->b3);
        model->is_loaded = 0;
    }
}

#endif