#ifndef EXPORTER_H
#define EXPORTER_H

#include <stdint.h>

void export_to_bmp(const char* filename, uint8_t pixels[16][16]);
void export_ascii_to_stdout(char ascii[16][16]);

#endif