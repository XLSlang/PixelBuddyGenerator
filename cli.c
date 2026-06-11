// cli.c
#include "cli.h"
#include "renderer.h"
#include "matrix_renderer.h"
#include "exporter.h"
#include "assets.h"
#include "dna_inference.h"  // 新增：AI模型推理
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>  // 用于_getch()检测ESC

void cli_init(CLIContext* ctx) {
    memset(ctx, 0, sizeof(CLIContext));
    config_load_from_bin("styles.bin", &ctx->config);
    ctx->selected_style = 0;
    ctx->selected_res = RES_1080P;
    ctx->selected_method = INTERP_PIXEL;
    strcpy(ctx->user_text, "");
    
    // 加载AI模型（静默加载）
    dna_model_global_init("../dlls/dna_model.bin");
}

void cli_show_menu(void) {
    system("cls");  // 清屏
    printf("\n");
    printf("  ╔════════════════════════════════════════════════════╗\n");
    printf("  ║         Pixel Buddy Generator v2.0 - AI版          ║\n");
    printf("  ║     输入自然语言描述，AI自动生成像素角色              ║\n");
    printf("  ╚════════════════════════════════════════════════════╝\n");
    printf("\n");
}

void cli_show_status(CLIContext* ctx) {
    int w, h;
    get_resolution_size(ctx->selected_res, &w, &h);
    const char* methods[] = {"像素风", "平滑", "复古"};
    
    printf("  ┌─────────────────────────────────────────────────┐\n");
    printf("  │ 当前配置                                        │\n");
    printf("  │   风格: %-20s                          │\n", 
           ctx->config.styles[ctx->selected_style].style_name);
    printf("  │   分辨率: %dx%d%-14s                          │\n", w, h, "");
    printf("  │   算法: %-22s                          │\n", methods[ctx->selected_method]);
    printf("  │   AI模型: %-22s                          │\n", 
           dna_model_is_loaded() ? "已加载" : "未加载(使用默认)");
    printf("  └─────────────────────────────────────────────────┘\n");
    printf("\n");
}

int cli_select_style(CLIContext* ctx) {
    printf("\n  选择风格:\n");
    for (int i = 0; i < ctx->config.style_count; i++) {
        printf("    %d. %s\n", i + 1, ctx->config.styles[i].style_name);
    }
    printf("    (按ESC返回上级)\n");
    printf("  请输入 (1-%d): ", ctx->config.style_count);
    
    int ch = _getch();
    if (ch == 27) return 0;  // ESC
    
    int choice = ch - '0';
    if (choice >= 1 && choice <= ctx->config.style_count) {
        ctx->selected_style = choice - 1;
        StyleConfig* style = &ctx->config.styles[ctx->selected_style];
        
        ctx->current_dna.head_shape = style->head_shape;
        ctx->current_dna.body_color = style->body_color;
        ctx->current_dna.eyes = style->eyes;
        ctx->current_dna.mouth = style->mouth;
        ctx->current_dna.hat = style->hat;
        ctx->current_dna.accessory = style->accessory;
        
        printf("%d\n", choice);
        printf("  ✓ 已选择: %s\n", style->style_name);
        return 1;
    }
    printf("\n  ❌ 无效选择\n");
    return 0;
}

int cli_select_resolution(CLIContext* ctx) {
    printf("\n  选择分辨率:\n");
    printf("    1. 720p (1280x720)\n");
    printf("    2. 1080p (1920x1080) [推荐]\n");
    printf("    3. 2K (2560x1440)\n");
    printf("    4. 4K (3840x2160)\n");
    printf("    (按ESC返回上级)\n");
    printf("  请输入 (1-4): ");
    
    int ch = _getch();
    if (ch == 27) return 0;
    
    int choice = ch - '0';
    if (choice >= 1 && choice <= 4) {
        ctx->selected_res = (Resolution)(choice - 1);
        int w, h;
        get_resolution_size(ctx->selected_res, &w, &h);
        printf("%d\n", choice);
        printf("  ✓ 已选择: %dx%d\n", w, h);
        return 1;
    }
    printf("\n  ❌ 无效选择\n");
    return 0;
}

int cli_select_method(CLIContext* ctx) {
    printf("\n  选择插值算法:\n");
    printf("    1. 最近邻插值 - 像素风 [推荐]\n");
    printf("    2. 双线性插值 - 平滑\n");
    printf("    3. 像素放大 - 复古游戏风格\n");
    printf("    (按ESC返回上级)\n");
    printf("  请输入 (1-3): ");
    
    int ch = _getch();
    if (ch == 27) return 0;
    
    int choice = ch - '0';
    if (choice >= 1 && choice <= 3) {
        ctx->selected_method = (InterpolationMethod)(choice - 1);
        const char* methods[] = {"最近邻插值", "双线性插值", "像素放大"};
        printf("%d\n", choice);
        printf("  ✓ 已选择: %s\n", methods[ctx->selected_method]);
        return 1;
    }
    printf("\n  ❌ 无效选择\n");
    return 0;
}

void cli_generate(CLIContext* ctx) {
    printf("\n  🎨 正在生成角色图...\n");
    
    StyleConfig* style = &ctx->config.styles[ctx->selected_style];
    
    // 渲染
    RenderBuffer buffer;
    renderer_render_all(&buffer, &ctx->current_dna);
    
    // ASCII预览
    renderer_to_ascii(&buffer, ctx->current_dna.body_color);
    printf("\n  ═══════════════════════════════════════════════\n");
    printf("  ASCII预览:\n\n");
    export_ascii_to_stdout(buffer.ascii);
    printf("\n  ═══════════════════════════════════════════════\n");
    
    // 矩阵放大
    MatrixRenderer mr;
    matrix_init(&mr, 16, 16, ctx->selected_res, ctx->selected_method);
    
    uint32_t* upscaled = (uint32_t*)malloc(mr.dst_width * mr.dst_height * sizeof(uint32_t));
    if (!upscaled) {
        printf("  ❌ 内存分配失败\n");
        return;
    }
    
    // 构建调色板
    uint8_t palette_rgb[16][4];
    for (int i = 0; i < 16; i++) {
        uint32_t color = get_color(i);
        palette_rgb[i][0] = (color >> 16) & 0xFF;
        palette_rgb[i][1] = (color >> 8) & 0xFF;
        palette_rgb[i][2] = color & 0xFF;
        palette_rgb[i][3] = (color >> 24) & 0xFF;
    }
    
    matrix_upscale(&mr, buffer.pixels, upscaled, palette_rgb);
    matrix_apply_effects(upscaled, mr.dst_width, mr.dst_height, style->brightness, style->color_shift);
    
    // 保存
    char filename[256];
    sprintf(filename, "pixel_buddy_%dx%d.bmp", mr.dst_width, mr.dst_height);
    matrix_save_to_bmp(filename, upscaled, mr.dst_width, mr.dst_height);
    
    free(upscaled);
    printf("\n  ✨ 生成完成！保存为: %s\n", filename);
}

// 新增：显示DNA详情
void cli_show_dna(PixelDNA* dna) {
    const char* shapes[] = {"方头", "圆头", "三角"};
    const char* colors[] = {"浅色", "深色", "金属", "鲜艳"};
    const char* eyeses[] = {"点点眼", "大眼", "眯眯眼"};
    const char* mouths[] = {"平嘴", "微笑", "愤怒"};
    const char* hats[] = {"无帽", "尖帽", "平顶帽"};
    const char* accs[] = {"无武器", "剑", "盾", "法杖"};
    
    printf("\n  🧬 AI生成的DNA参数:\n");
    printf("     头型: %s\n", shapes[dna->head_shape]);
    printf("     肤色: %s\n", colors[dna->body_color]);
    printf("     眼睛: %s\n", eyeses[dna->eyes]);
    printf("     嘴巴: %s\n", mouths[dna->mouth]);
    printf("     帽子: %s\n", hats[dna->hat]);
    printf("     武器: %s\n", accs[dna->accessory]);
}

void cli_run(CLIContext* ctx) {
    int running = 1;
    int step = 0;  // 0=选风格, 1=选分辨率, 2=选算法, 3=AI输入或手动输入
    
    cli_show_menu();
    
    while (running) {
        cli_show_status(ctx);
        
        switch(step) {
            case 0:
                printf("  ▶ 步骤1: 选择风格模板\n");
                printf("     (AI会基于此风格进行调整)\n");
                if (cli_select_style(ctx)) {
                    step = 1;
                    cli_show_menu();
                }
                break;
                
            case 1:
                printf("  ▶ 步骤2: 选择分辨率\n");
                if (cli_select_resolution(ctx)) {
                    step = 2;
                    cli_show_menu();
                } else {
                    step = 0;
                    cli_show_menu();
                }
                break;
                
            case 2:
                printf("  ▶ 步骤3: 选择插值算法\n");
                if (cli_select_method(ctx)) {
                    step = 3;
                    cli_show_menu();
                } else {
                    step = 1;
                    cli_show_menu();
                }
                break;
                
            case 3:
                printf("\n  ▶ 步骤4: 描述你想要的角色\n");
                printf("  ┌─────────────────────────────────────────────────┐\n");
                printf("  │ 示例:                                          │\n");
                printf("  │   • 一个悲伤的恐龙                              │\n");
                printf("  │   • 快乐的法师                                  │\n");
                printf("  │   • 愤怒的战士拿着剑                            │\n");
                printf("  │   • 可爱的圆头小人                              │\n");
                printf("  │   • 戴尖帽的巫师                                │\n");
                printf("  └─────────────────────────────────────────────────┘\n");
                printf("  (直接回车使用当前风格, 输入0手动设置, ESC退出)\n");
                printf("  > ");
                
                // 读取输入
                if (fgets(ctx->user_text, sizeof(ctx->user_text), stdin) == NULL) {
                    continue;
                }
                
                size_t len = strlen(ctx->user_text);
                if (len > 0 && ctx->user_text[len-1] == '\n') {
                    ctx->user_text[len-1] = '\0';
                    len--;
                }
                
                // 检查ESC
                if (len == 1 && ctx->user_text[0] == 27) {
                    running = 0;
                    break;
                }
                
                // 检查是否退出
                if (strcmp(ctx->user_text, "exit") == 0 || strcmp(ctx->user_text, "quit") == 0) {
                    running = 0;
                    break;
                }
                
                // 处理输入
                if (len == 0) {
                    // 直接回车，使用当前DNA（已从风格加载）
                    printf("\n  📌 使用当前风格模板\n");
                } else if (strcmp(ctx->user_text, "0") == 0) {
                    // 手动设置DNA
                    printf("\n  🔧 手动设置DNA参数:\n");
                    printf("    头型 (0=方头,1=圆头,2=三角): ");
                    scanf("%d", &ctx->current_dna.head_shape);
                    printf("    肤色 (0=浅,1=深,2=金属,3=鲜艳): ");
                    scanf("%d", &ctx->current_dna.body_color);
                    printf("    眼睛 (0=点点,1=大眼,2=眯眯): ");
                    scanf("%d", &ctx->current_dna.eyes);
                    printf("    嘴巴 (0=平,1=笑,2=怒): ");
                    scanf("%d", &ctx->current_dna.mouth);
                    printf("    帽子 (0=无,1=尖,2=平顶): ");
                    scanf("%d", &ctx->current_dna.hat);
                    printf("    武器 (0=无,1=剑,2=盾,3=法杖): ");
                    scanf("%d", &ctx->current_dna.accessory);
                    getchar(); // 清除换行符
                } else {
                    // AI解析自然语言
                    printf("\n  🤖 AI正在分析: %s\n", ctx->user_text);
                    
                    // 提取特征并推理
                    float features[6];
                    extract_features(ctx->user_text, features);
                    
                    float dna_raw[6];
                    if (dna_model_predict_global(features, dna_raw) == 0) {
                        // 转换为整数参数
                        ctx->current_dna.head_shape = (int)(dna_raw[0] * 3);
                        ctx->current_dna.body_color = (int)(dna_raw[1] * 4);
                        ctx->current_dna.eyes = (int)(dna_raw[2] * 3);
                        ctx->current_dna.mouth = (int)(dna_raw[3] * 3);
                        ctx->current_dna.hat = (int)(dna_raw[4] * 3);
                        ctx->current_dna.accessory = (int)(dna_raw[5] * 4);
                        
                        // 边界检查
                        if (ctx->current_dna.head_shape > 2) ctx->current_dna.head_shape = 2;
                        if (ctx->current_dna.body_color > 3) ctx->current_dna.body_color = 3;
                        if (ctx->current_dna.eyes > 2) ctx->current_dna.eyes = 2;
                        if (ctx->current_dna.mouth > 2) ctx->current_dna.mouth = 2;
                        if (ctx->current_dna.hat > 2) ctx->current_dna.hat = 2;
                        if (ctx->current_dna.accessory > 3) ctx->current_dna.accessory = 3;
                        
                        // 显示解析结果
                        cli_show_dna(&ctx->current_dna);
                    } else {
                        printf("  ⚠️ AI模型未加载，使用默认参数\n");
                    }
                }
                
                // 生成角色
                cli_generate(ctx);
                
                printf("\n  ═══════════════════════════════════════════════\n");
                printf("  按任意键继续生成，按ESC退出...\n");
                
                int ch = _getch();
                if (ch == 27) {
                    running = 0;
                } else {
                    // 重置到步骤1，但保持风格选择
                    step = 1;
                    cli_show_menu();
                }
                break;
        }
    }
    
    printf("\n  👋 Goodbye! Stay pixel.\n");
}