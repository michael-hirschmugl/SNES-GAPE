# SNES-GAPE

Migrating to CMAKE environment.

**Super NES (Super Famicom) Graphics And Palette Enabler**

## Introduction
This little command line tool translates .bmp files into SNES (SFC) spritesheets.
The output can be a 2bpp, 4bpp or 8bpp set or bytes in ASCII that can be copied into the SNES' VRAM.
A color map is also generated with either 4, 16 or 256 colors. Also in ASCII format.

As input, this tool only accepts .bmp files with either 4bpp or 8bpp. A color map has to be present and no compression is allowed. Programs like Irfaniew usually store a color map (MS Paint doesn't!!).

Also, the image has to be a pixel width, which is a multiple of 8, because 8x8 pixel is the size of one tile in the SNES. Other sizes like 16x16, 32x32 or 64x64 are just made up from multiple 8x8 tiles.

An image with maximum width of 128 pixel will be stored from top down in VRAM "as is". What I mean by that is, VRAM references tiles in rows of 16. 16x8 is 128. Now you see?
BUT, an image that is 256 pixel in width (which is the maximum resolution of the SNES), will also be converted, but the right half of the image will be stored AFTER the whole left half of the image in memory.

In other words, this tool enables you to store full screen (256x224) for the SNES. Basically, one could make a slide show viewer with this (which is the reason I created this tool).

## Usage
I am a lazy person and that is why I did not include much error checking...
It is mandatory, that the parameters look exactly like this when calling the program:
`-f [filename].bmp -a [number of sprites] -c [number of colors] -o [filename_sprites].inc -t [filename_colortable].inc`
* filename: That's the name of the .bmp file to read.
* number of sprites: The amount of sprites (8x8 pixel each) in the image.
* number of colors: The amount of colors for the resulting file. This can be 4, 16 or 256. BUT, 4 color images cannot be generated (with any software that I could find). So, to generate 4 color data, the image has to be stored with 16 colors, while the user has to make sure that only 4 colors are used! SNES GAPE will remind you of that...
* filename_sprites: Output filename for the sprite sheet data.
* filename_colortable: Output filename for the colortable data.

## What else
This tool was written in c on a linux machine. You can compile it simply by writing "gcc main.c". Works everytime, most of the time. If you feeld like expanding this or at least adding some error management, I would love you for that.

Yours, Michael

![alt text](https://github.com/hirschmensch/SNES-GAPE/blob/master/snes_gape_screen.png "Example")
