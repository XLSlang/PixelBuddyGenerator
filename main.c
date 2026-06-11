// main.c
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "cli.h"

int main(void) {
    // 设置控制台编码
    system("chcp 65001 > nul");
    srand(time(NULL));
    
    CLIContext ctx;
    cli_init(&ctx);
    cli_run(&ctx);
    
    return 0;
}