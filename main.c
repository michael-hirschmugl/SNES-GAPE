#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gape.h"

int main(int argc, char** argv)
{
	// Declarations
	unsigned char *byte;
	short *word;
	int *int_buffer;
	FILE *image_file;
	FILE *colortable_file;
	FILE *characters_file;
	int i = 0,j = 0,k = 0,l = 0;
	sprite_d *sprite;
	float fbuffer = 0;
	int row = 0,column = 0, half_switch = 0;

	const char color_table_header[] = {"BG_Palette:\n    .db "};
	const char characters_header[] = {"Tiles:"};

	// Initializations
	byte = malloc(sizeof(unsigned char));
	word = malloc(sizeof(short));
	int_buffer = malloc(sizeof(int));

	// First, read filename and expect it as parameter after -f [filename]
	// TODO: check for amount of parameters, "-f", filename length, .bmp ending,
	//       position of filename argument
	bmp_header.filename = malloc(sizeof(char)*strlen(*(argv+2)));
	strcpy(bmp_header.filename, *(argv+2));

	// Read rest of parameters
	// TODO: well... absolutely no error checking done :(
	general_data.number_of_sprites = atoi(*(argv+4));
	general_data.number_of_colors = atoi(*(argv+6));
	general_data.output_filename = malloc(sizeof(char)*strlen(*(argv+8)));
	strcpy(general_data.output_filename, *(argv+8));
	general_data.colortable_filename = malloc(sizeof(char)*strlen(*(argv+10)));
	strcpy(general_data.colortable_filename, *(argv+10));

	// Open file as binary read-only
	if(!(image_file = fopen(bmp_header.filename,"rb")))
	{
		printf("Could not open file %s\n",bmp_header.filename);
		return -1;
	}

	// Read Bitmap Header
	// TODO: Error checking
	//
	// Read "header field"
	bmp_header.header_field = read_header_field(image_file);
	if(!bmp_header.header_field || bmp_header.header_field > 1)
	{
		printf("File is not in BMP format\n");
		return -2;
	}
	// Read size of .bmp file in bytes
	fread(&bmp_header.file_size, 4, 1, image_file);
	for(i = 0; i < 4; i++) fread(byte, 1, 1, image_file);
	// Read offset of image data in bytes
	fread(&bmp_header.image_data_offset, 4, 1, image_file);

	// Read DIB Header
	// TODO: Error checking
	//
	// DIB Header Size
	fread(&dib_header.dib_header_size, 4, 1, image_file);
	// Width and Height of BMP
	// TODO: Does this correlate with amount of sprites and sprite size?
	fread(&dib_header.bmp_width, 4, 1, image_file);
	fread(&dib_header.bmp_height, 4, 1, image_file);

	for(i = 0; i < 2; i++) fread(byte, 1, 1, image_file);
	// Bits per Pixel
	fread(&dib_header.bmp_bits_per_pixel, 2, 1, image_file);
	if((dib_header.bmp_bits_per_pixel == 4) || (dib_header.bmp_bits_per_pixel == 8))
	{
		// Perfect. Thank you.
		// (dib_header.bmp_bits_per_pixel == 2) was deleted, because a 2bpp file cannot be generated as far as I know.
	}
	else
	{
		printf("Unsupported color depth.\nSupported are 2bpp, 4bpp or 8bpp\n");
		printf("Color depth maximum is 8bpp or 256 colors... No chance!\n\n\n\n\n\nSorry!\n");
		return -3;
	}
	// Compression Method
	fread(&dib_header.bmp_compression_method, 4, 1, image_file);
	if(dib_header.bmp_compression_method)
	{
		printf("File is compressed... No chance!\n\n\n\n\n\nSorry!\n");
		return -4;
	}
	for(i = 0; i < 12; i++) fread(byte, 1, 1, image_file);

	// Color Table Size
	fread(&dib_header.bmp_number_of_colors, 4, 1, image_file);
	if(dib_header.bmp_number_of_colors != general_data.number_of_colors)
	{
		printf("Color table size in bmp is %d\n",dib_header.bmp_number_of_colors);
		printf("Sprite should contain %d colors\n",general_data.number_of_colors);
		if((dib_header.bmp_number_of_colors == 16) && (general_data.number_of_colors == 4))
		{
			printf("Going to generate a 4 color image from a 16 color source.\n");
			printf("This is actually the only way to generate a 4 color image.\n");
			printf("But, pay attention to the color table... Only the first four colors are going to be used.\n");
			printf("Following is the color table in the bmp file:\n");
		}
		else
		{
			printf("Should be the same... No chance!\n\n\n\n\n\nSorry!\n");
			return -5;
		}
	}

	// Jump to Color Table
	fseek(image_file, 14 + dib_header.dib_header_size, SEEK_SET);
	// This should be the location of the color table
	// But, if dib_header_size = 52 or 56, this is actually the position of the
	// three bit masks. Each are 4 bytes, so the color table would be 12 bytes ahead.
	// With alpha bitmask, another 4 bytes are added here.

	// Memory allocation for color table in bmp file
	bmp_color_table = malloc(dib_header.bmp_number_of_colors * sizeof(int));
	bmp_color_table_blue = malloc(dib_header.bmp_number_of_colors * sizeof(unsigned char));
	bmp_color_table_green = malloc(dib_header.bmp_number_of_colors * sizeof(unsigned char));
	bmp_color_table_red = malloc(dib_header.bmp_number_of_colors * sizeof(unsigned char));
	// Memory allocations for color table of snes
	snes_color_table = malloc(dib_header.bmp_number_of_colors * sizeof(short));
	snes_color_table_blue = malloc(dib_header.bmp_number_of_colors * sizeof(unsigned char));
	snes_color_table_green = malloc(dib_header.bmp_number_of_colors * sizeof(unsigned char));
	snes_color_table_red = malloc(dib_header.bmp_number_of_colors * sizeof(unsigned char));

	// Reading the Color Table
	for(i = 0; i < dib_header.bmp_number_of_colors; i++)
		fread(bmp_color_table+i, 4, 1, image_file);
	// Transforming the colors in individual tables
	for(i = 0; i < dib_header.bmp_number_of_colors; i++)
	{
		*(bmp_color_table_blue+i) = *(bmp_color_table+i);
		*(bmp_color_table_green+i) = *(bmp_color_table+i) >> 8;
		*(bmp_color_table_red+i) = *(bmp_color_table+i) >> 16;

		printf("Color Nr.: %d: Blue: %d, Green: %d, Red: %d\n",i,*(bmp_color_table_blue+i),*(bmp_color_table_green+i),*(bmp_color_table_red+i));
	}
	// Translate colors for SNES color table
	for(i = 0; i < dib_header.bmp_number_of_colors; i++)
	{
		fbuffer = *(bmp_color_table_blue+i);
		fbuffer= fbuffer / 255;
		fbuffer = fbuffer * 31;
		*(snes_color_table_blue+i) = (unsigned char)fbuffer;
		fbuffer = *(bmp_color_table_green+i);
		fbuffer= fbuffer / 255;
		fbuffer = fbuffer * 31;
		*(snes_color_table_green+i) = (unsigned char)fbuffer;
		fbuffer = *(bmp_color_table_red+i);
		fbuffer= fbuffer / 255;
		fbuffer = fbuffer * 31;
		*(snes_color_table_red+i) = (unsigned char)fbuffer;
		printf("Snes Color Nr.: %d: Blue: %d, Green: %d, Red: %d\n",i,*(snes_color_table_blue+i),*(snes_color_table_green+i),*(snes_color_table_red+i));
	}
	// Set it all together into one color table for the snes
	for(i = 0; i < dib_header.bmp_number_of_colors; i++)
	{
		*(snes_color_table+i) = ((*(bmp_color_table_blue+i)/8) * 1024) + ((*(bmp_color_table_green+i)/8) * 32) + (*(bmp_color_table_red+i)/8);
		//*(snes_color_table+i) = ((*(snes_color_table_blue+i) << 10) & 0b0111110000000000U)  | ((*(snes_color_table_green+i) << 6) & 0b0000001111100000U) | ((*(snes_color_table_red+i) << 0) & 0b0000000000011111U);
	}

	// Print out the colors if bmp color table has 16 values and SNES characters should only contain 4
	if((dib_header.bmp_number_of_colors == 16) && (general_data.number_of_colors == 4))
	{
		for(i = 0; i < dib_header.bmp_number_of_colors; i++)
			printf("Color %d is %x\n",i,*(snes_color_table+i));
		k = 4;	// Only four colors will be in the color table
	}
	else
	{
		k = dib_header.bmp_number_of_colors;	// All colors will be in color table
	}

	// Writing the color table to a file
	colortable_file = fopen(general_data.colortable_filename,"w");
	fprintf(colortable_file,color_table_header);
	for(i = 0; i < k; i++)	// k is defined in the previous if-statement
	{
		fprintf(colortable_file,"$");
		fprintf(colortable_file,"%02x",(0x00FFU & *(snes_color_table+i)));
		fprintf(colortable_file,", ");
		fprintf(colortable_file,"$");
		fprintf(colortable_file,"%02x",*(snes_color_table+i) >> 8);
		if(i < (k-1))
			fprintf(colortable_file,", ");
	}

	k = 0;

	// Some memory allocations
	// For the BMP pixel data
	bmp_pixel_data.color_index = malloc(dib_header.bmp_height * sizeof(unsigned char*));
	for(i = 0; i < dib_header.bmp_height; i++)
		bmp_pixel_data.color_index[i] = malloc(dib_header.bmp_width);

	// For the sprite structures
	sprite = malloc(general_data.number_of_sprites * sizeof(sprite_d));
	for(i = 0; i < general_data.number_of_sprites; i++)
	{
		sprite[i].index = i;
		sprite[i].color_index = malloc(64 * sizeof(unsigned char));
	}

	// Jump to Pixel Data
	// TODO:
	fseek(image_file, bmp_header.image_data_offset, SEEK_SET);

	// To read the pixel data, we'll need three different routines for 2bpp, 4bpp and 8bpp
	// Let's start with 4bpp because that's the bimtap I have right now...
	// The index should be stores like this: 4bpp, first pxel: AAAA, second pixel: BBBB
	// So, one byte are two pixel: AAAABBBB
	if(dib_header.bmp_number_of_colors == 4)
	{
		// Read 2bpp image
		// Actually, a 2bpp file will never exist, so this cannot become true.
		// The 4bpp file is used to generate 2bpp character data.
		printf("\nwtf\n");
	}
	else
	{
		if(dib_header.bmp_number_of_colors == 16)
		{
			// Read 4bpp image
			for(j = (dib_header.bmp_height-1); j >= 0; j--)
			{
				for(i = 0; i < dib_header.bmp_width;i=i)
				{
					fread(byte, 1, 1, image_file);
					bmp_pixel_data.color_index[j][i] = *byte >> 4;
					i++;
					bmp_pixel_data.color_index[j][i] = *byte & 0x0FU;
					i++;
				}
			}
		}
		else
		{
			if(dib_header.bmp_number_of_colors == 256)
			{
				// Read 8bpp image
				for(j = 0; j < dib_header.bmp_height; j++)
				{
					for(i = 0; i < dib_header.bmp_width; i++)
					{
						fread(byte, 1, 1, image_file);
						bmp_pixel_data.color_index[j][i] = *byte;
					}
				}
			}
			else
			{
				// Wrong amount of colors in the bmp file.
				// Actually, this error should have been earlier, but let's show it anyway...
				printf("There was no fitting routine to read pixel data from the bmp file.\n");
				printf("If this error shows, something has gone terribly wrong, because\n");
				printf("this check was already done earlier in the code.\n");
				printf("You might wanna drop me a mail if you encounter this: michael.hirschmugl@gmx.at\n");
				return -66;
			}
		}
	}

	l = 0;
	row = 0;
	column = 0;
	half_switch = 0;
	for(k = 0; k < general_data.number_of_sprites; k=k)
	{
		//printf("%02x: ",k);
		for(j = 0; j < 8; j++)
		{
			for(i = 0; i < 8; i++)
			{
				sprite[k].color_index[l] = bmp_pixel_data.color_index[j+(8*row)][i+(8*column)];
				//printf("%x",sprite[k].color_index[l]);
				l++;
			}
		}
		//printf("\n");
		l = 0;
		k++;
		if(k%16 == 0)
		{
			if(half_switch)
			{
				column = 16;
			}
			else
			{
				column = 0;
			}
			
			row++;
		}
		else
		{
			column++;
		}
		if(((row*8)+1) > dib_header.bmp_height)
		{
			row = 0;
			column = 16;
			half_switch = 1;
		}
	}

	// Last but not least... Generate the bitplanes
	characters_file = fopen(general_data.output_filename,"w");
	fprintf(characters_file,characters_header);
	// For 2bpp data:
	if(general_data.number_of_colors == 4)
	{	
		for(k = 0; k < general_data.number_of_sprites; k++)
		{
			fprintf(characters_file,"\n    .db ");
			for(i = 0; i < 8; i++)
			{
				// First bitplane
				fprintf(characters_file,"$");
				*byte = ((sprite[k].color_index[(i*8)+0] & 0x01U) << 7) | ((sprite[k].color_index[(i*8)+1] & 0x01U) << 6) | ((sprite[k].color_index[(i*8)+2] & 0x01U) << 5)
				      | ((sprite[k].color_index[(i*8)+3] & 0x01U) << 4) | ((sprite[k].color_index[(i*8)+4] & 0x01U) << 3) | ((sprite[k].color_index[(i*8)+5] & 0x01U) << 2)
				      | ((sprite[k].color_index[(i*8)+6] & 0x01U) << 1) | ((sprite[k].color_index[(i*8)+7] & 0x01U) << 0);
				fprintf(characters_file,"%02x",*byte);
				fprintf(characters_file,", ");
				// Second bitplane
				fprintf(characters_file,"$");
				*byte = ((sprite[k].color_index[(i*8)+0] & 0x02U) << 6) | ((sprite[k].color_index[(i*8)+1] & 0x02U) << 5) | ((sprite[k].color_index[(i*8)+2] & 0x02U) << 4)
				      | ((sprite[k].color_index[(i*8)+3] & 0x02U) << 3) | ((sprite[k].color_index[(i*8)+4] & 0x02U) << 2) | ((sprite[k].color_index[(i*8)+5] & 0x02U) << 1)
				      | ((sprite[k].color_index[(i*8)+6] & 0x02U) << 0) | ((sprite[k].color_index[(i*8)+7] & 0x02U) >> 1);
				fprintf(characters_file,"%02x",*byte);
				if(i != 7)
					fprintf(characters_file,", ");
			}
		}
	}

	// For 4bpp data:
	if(general_data.number_of_colors == 16)
	{
		for(k = 0; k < general_data.number_of_sprites; k++)
		{
			fprintf(characters_file,"\n    .db ");
			for(i = 0; i < 8; i++)
			{
				// First bitplane
				fprintf(characters_file,"$");
				*byte = ((sprite[k].color_index[(i*8)+0] & 0x01U) << 7) | ((sprite[k].color_index[(i*8)+1] & 0x01U) << 6) | ((sprite[k].color_index[(i*8)+2] & 0x01U) << 5)
				      | ((sprite[k].color_index[(i*8)+3] & 0x01U) << 4) | ((sprite[k].color_index[(i*8)+4] & 0x01U) << 3) | ((sprite[k].color_index[(i*8)+5] & 0x01U) << 2)
				      | ((sprite[k].color_index[(i*8)+6] & 0x01U) << 1) | ((sprite[k].color_index[(i*8)+7] & 0x01U) << 0);
				fprintf(characters_file,"%02x",*byte);
				fprintf(characters_file,", ");
				// Second bitplane
				fprintf(characters_file,"$");
				*byte = ((sprite[k].color_index[(i*8)+0] & 0x02U) << 6) | ((sprite[k].color_index[(i*8)+1] & 0x02U) << 5) | ((sprite[k].color_index[(i*8)+2] & 0x02U) << 4)
				      | ((sprite[k].color_index[(i*8)+3] & 0x02U) << 3) | ((sprite[k].color_index[(i*8)+4] & 0x02U) << 2) | ((sprite[k].color_index[(i*8)+5] & 0x02U) << 1)
				      | ((sprite[k].color_index[(i*8)+6] & 0x02U) << 0) | ((sprite[k].color_index[(i*8)+7] & 0x02U) >> 1);
				fprintf(characters_file,"%02x",*byte);
				fprintf(characters_file,", ");
			}
			for(i = 0; i < 8; i++)
			{
				// Third bitplane
				fprintf(characters_file,"$");
				*byte = ((sprite[k].color_index[(i*8)+0] & 0x04U) << 5) | ((sprite[k].color_index[(i*8)+1] & 0x04U) << 4) | ((sprite[k].color_index[(i*8)+2] & 0x04U) << 3)
				      | ((sprite[k].color_index[(i*8)+3] & 0x04U) << 2) | ((sprite[k].color_index[(i*8)+4] & 0x04U) << 1) | ((sprite[k].color_index[(i*8)+5] & 0x04U) << 0)
				      | ((sprite[k].color_index[(i*8)+6] & 0x04U) >> 1) | ((sprite[k].color_index[(i*8)+7] & 0x04U) >> 2);
				fprintf(characters_file,"%02x",*byte);
				fprintf(characters_file,", ");
				// Fourth bitplane
				fprintf(characters_file,"$");
				*byte = ((sprite[k].color_index[(i*8)+0] & 0x08U) << 4) | ((sprite[k].color_index[(i*8)+1] & 0x08U) << 3) | ((sprite[k].color_index[(i*8)+2] & 0x08U) << 2)
				      | ((sprite[k].color_index[(i*8)+3] & 0x08U) << 1) | ((sprite[k].color_index[(i*8)+4] & 0x08U) << 0) | ((sprite[k].color_index[(i*8)+5] & 0x08U) >> 1)
				      | ((sprite[k].color_index[(i*8)+6] & 0x08U) >> 2) | ((sprite[k].color_index[(i*8)+7] & 0x08U) >> 3);
				fprintf(characters_file,"%02x",*byte);
				if(i != 7)
					fprintf(characters_file,", ");
			}
		}
	}
	// For 8bpp data:
	if(general_data.number_of_colors == 256)
	{
		for(k = 0; k < general_data.number_of_sprites; k++)
		{
			fprintf(characters_file,"\n    .db ");
			for(i = 0; i < 8; i++)
			{
				fprintf(characters_file,"$");
				*byte = sprite[k].color_index[(i*8)+0];
				fprintf(characters_file,"%02x",*byte);
				fprintf(characters_file,", ");
				fprintf(characters_file,"$");
				*byte = sprite[k].color_index[(i*8)+1];
				fprintf(characters_file,"%02x",*byte);
				fprintf(characters_file,", ");
				fprintf(characters_file,"$");
				*byte = sprite[k].color_index[(i*8)+2];
				fprintf(characters_file,"%02x",*byte);
				fprintf(characters_file,", ");
				fprintf(characters_file,"$");
				*byte = sprite[k].color_index[(i*8)+3];
				fprintf(characters_file,"%02x",*byte);
				fprintf(characters_file,", ");
				fprintf(characters_file,"$");
				*byte = sprite[k].color_index[(i*8)+4];
				fprintf(characters_file,"%02x",*byte);
				fprintf(characters_file,", ");
				fprintf(characters_file,"$");
				*byte = sprite[k].color_index[(i*8)+5];
				fprintf(characters_file,"%02x",*byte);
				fprintf(characters_file,", ");
				fprintf(characters_file,"$");
				*byte = sprite[k].color_index[(i*8)+6];
				fprintf(characters_file,"%02x",*byte);
				fprintf(characters_file,", ");
				fprintf(characters_file,"$");
				*byte = sprite[k].color_index[(i*8)+7];
				fprintf(characters_file,"%02x",*byte);
				if(i != 7)
					fprintf(characters_file,", ");
			}
		}
	}

	printf("Stats:\n------\n");
	if(general_data.number_of_colors == 4)
	{
		printf("Characters need %d bytes of RAM\n",general_data.number_of_sprites*16);
		printf("Colortable needs %d bytes of RAM\n",8);
	}
	if(general_data.number_of_colors == 16)
	{
		printf("Characters need %d bytes of RAM\n",general_data.number_of_sprites*32);
		printf("Colortable needs %d bytes of RAM\n",32);
	}
	if(general_data.number_of_colors == 256)
	{
		printf("Characters need %d bytes of RAM\n",general_data.number_of_sprites*64);
		printf("Colortable needs %d bytes of RAM\n",512);
	}

	// Free
	free(byte);
	free(word);
	free(bmp_header.filename);
	free(int_buffer);
	free(general_data.output_filename);
	free(general_data.colortable_filename);
	for(i = 0; i < dib_header.bmp_height; i++)
		free(*(bmp_pixel_data.color_index+i));
	free(bmp_pixel_data.color_index);
	for(i = 0; i < general_data.number_of_sprites; i++)
		free(sprite[i].color_index);
	free(sprite);
	free(bmp_color_table);
	free(bmp_color_table_blue);
	free(bmp_color_table_green);
	free(bmp_color_table_red);
	free(snes_color_table);
	free(snes_color_table_blue);
	free(snes_color_table_green);
	free(snes_color_table_red);

	// Close file
	fclose(image_file);
	fclose(colortable_file);
	fclose(characters_file);
	return 0;
}
