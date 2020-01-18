/*
 * gape.c
 */

#include "gape.h"

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
