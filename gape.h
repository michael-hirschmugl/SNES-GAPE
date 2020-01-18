/*
 * gape.h
 */

#ifndef GAPE_H
#define GAPE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct {
	char *filename;
	unsigned char header_field;
	int file_size;
	int image_data_offset;
} bmp_header;

struct {
	// This value determins the bitmap header variant and offset to the color table
	int dib_header_size;
	// Other values are self explained
	int bmp_width;
	int bmp_height;
	short bmp_bits_per_pixel;
	// This isn't actually used
	int bmp_compression_method;
	// This is important
	int bmp_number_of_colors;
} dib_header;

struct {
	unsigned char **color_index;
} bmp_pixel_data;

struct {
	int number_of_sprites;
	int number_of_colors;
	char *output_filename;
	char *colortable_filename;
} general_data;

typedef struct sprite_data {
	int index;
	unsigned char *color_index;
} sprite_d;

int *bmp_color_table;
unsigned char *bmp_color_table_blue;
unsigned char *bmp_color_table_green;
unsigned char *bmp_color_table_red;
short *snes_color_table;
unsigned char *snes_color_table_blue;
unsigned char *snes_color_table_green;
unsigned char *snes_color_table_red;

unsigned char *bitplane;

extern unsigned char read_header_field(FILE *);

#endif //GAPE_H