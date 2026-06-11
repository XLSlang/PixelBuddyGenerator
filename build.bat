# build.bat
@echo off
echo Compiling Pixel Buddy Generator with AI Model...

gcc -c assets.c -o assets.o
gcc -c dna.c -o dna.o
gcc -c renderer.c -o renderer.o
gcc -c exporter.c -o exporter.o
gcc -c config.c -o config.o
gcc -c matrix_renderer.c -o matrix_renderer.o
gcc -c cli.c -o cli.o
gcc -c main.c -o main.o

REM 链接时添加数学库
gcc assets.o dna.o renderer.o exporter.o config.o matrix_renderer.o cli.o main.o -o pixel_buddy.exe -lm

echo Done! Run pixel_buddy.exe