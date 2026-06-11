// cli.h
#ifndef CLI_H
#define CLI_H

#include "config.h"
#include "dna.h"
#include "matrix_renderer.h"

typedef struct {
    AppConfig config;
    PixelDNA current_dna;
    int selected_style;
    Resolution selected_res;
    InterpolationMethod selected_method;
    char user_text[256];
} CLIContext;

// 函数声明
void cli_init(CLIContext* ctx);
void cli_show_menu(void);
void cli_show_status(CLIContext* ctx);
int cli_select_style(CLIContext* ctx);
int cli_select_resolution(CLIContext* ctx);
int cli_select_method(CLIContext* ctx);
void cli_generate(CLIContext* ctx);
void cli_run(CLIContext* ctx);
void cli_show_dna(PixelDNA* dna);  // 新增

#endif