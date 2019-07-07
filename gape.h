const char color_table_header[] = {"BG_Palette:\n    .db "};
const char characters_header[] = {"Tiles:"};

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

//
// Reads the header field of a bmp and codes it as follows: BM=1, BA=2, CI=3, CP=4, IC=5, PT=6
// Must be first call after opening of image file.
// Return 0 if nothing was found.
//
unsigned char read_header_field(FILE *image_file)
{
	unsigned char header_field = 0;
	unsigned char byte[1];

	fread(byte, 1, 1, image_file);
	if(*byte == 'B')
	{
		fread(byte, 1, 1, image_file);
		if(*byte == 'M')
		{
			// BM
			header_field = 1;
		}
		else
		{
			if(*byte == 'A')
			{
				// BA
				header_field = 2;
			}
			else
			{
				// Nothing found
				return 0;
			}
		}
	}
	else
	{
		if(*byte == 'C')
		{
			fread(byte, 1, 1, image_file);
			if(*byte == 'I')
			{
				// CI
				header_field = 3;
			}
			else
			{
				if(*byte == 'P')
				{
					// CP
					header_field = 4;
				}
				else
				{
					// Nothing found
					return 0;
				}
			}
		}
		else
		{
			if(*byte == 'I')
			{
				fread(byte, 1, 1, image_file);
				if(*byte == 'C')
				{
					// IC
					header_field = 5;
				}
				else
				{
					// Nothing found
					return 0;
				}
			}
			else
			{
				if(*byte == 'P')
				{
					fread(byte, 1, 1, image_file);
					if(*byte == 'T')
					{
						// PT
						header_field = 6;

					}
					else
					{
						// Nothing found
						return 0;
					}
				}
				else
				{
					// Nothing found
					return 0;
				}
			}
		}
	}

	return header_field;
}
