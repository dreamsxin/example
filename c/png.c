// LibPNG example
// A.Greensted
// http://www.labbookpages.co.uk

// Version 2.0
// With some minor corrections to Mandlebrot code (thanks to Jan-Oliver)

// Version 1.0 - Initial release

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <png.h>

void draw_circle(png_bytep* buffer, int c_x, int c_y, int r);
void draw_pixel(png_bytep* buffer, int x, int y);
void set_bckgrd_color(png_bytep* buffer, int height, int width, int color);

int main(int argc, char *argv[])
{
	// Make sure that the output filename argument has been provided
	if (argc != 2) {
		fprintf(stderr, "Please specify output file\n");
		return 1;
	}
	
	// Specify an output image size
	int width = 500;
	int height = 300;
	
	int code = 0;
	FILE *fp = NULL;
	png_structp png_ptr = NULL;
	png_infop info_ptr = NULL;
	png_bytep row = NULL;
	char* filename = argv[1];
	char* title = "Image";
	
	png_bytep* buffer = (png_bytep*)malloc(sizeof(png_bytep) * height);
		for (int y=0; y<height; y++)
				buffer[y] = (png_byte*) malloc(3 * width * sizeof(png_byte));
	
	// Open file for writing (binary mode)
	fp = fopen(filename, "wb");
	if (fp == NULL) {
		fprintf(stderr, "Could not open file %s for writing\n", filename);
		code = 1;
		goto finalise;
	}

	// Initialize write structure
	png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (png_ptr == NULL) {
		fprintf(stderr, "Could not allocate write struct\n");
		code = 1;
		goto finalise;
	}

	// Initialize info structure
	info_ptr = png_create_info_struct(png_ptr);
	if (info_ptr == NULL) {
		fprintf(stderr, "Could not allocate info struct\n");
		code = 1;
		goto finalise;
	}

	// Setup Exception handling
	if (setjmp(png_jmpbuf(png_ptr))) {
		fprintf(stderr, "Error during png creation\n");
		code = 1;
		goto finalise;
	}

	png_init_io(png_ptr, fp);

	// Write header (8 bit colour depth)
	
	png_set_IHDR(png_ptr, info_ptr, width, height,
			8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
			PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

	// Set title
	if (title != NULL) {
		png_text title_text;
		title_text.compression = PNG_TEXT_COMPRESSION_NONE;
		title_text.key = "Title";
		title_text.text = title;
		png_set_text(png_ptr, info_ptr, &title_text, 1);
	}

	png_write_info(png_ptr, info_ptr);
	
	set_bckgrd_color(buffer, height, width, 100);
	draw_circle(buffer, 200, 150, 30);


	// Allocate memory for one row (3 bytes per pixel - RGB)
//	row = (png_bytep) malloc(3 * width * sizeof(png_byte));

	// Write image data
	int x, y;
	for (y=0 ; y<height ; y++) {
//		for (x=0 ; x<width ; x++) {
//			row[x*3] = x*y;
//			row[x*3 + 1] = 0;
//			row[x*3 + 2] = 0;
//		}
		png_write_row(png_ptr, buffer[y]);
	}

	// End write
	png_write_end(png_ptr, NULL);

	finalise:
	if (fp != NULL) fclose(fp);
	if (info_ptr != NULL) png_free_data(png_ptr, info_ptr, PNG_FREE_ALL, -1);
	if (png_ptr != NULL) png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
	if (row != NULL) free(row);

	return code;
}

void set_bckgrd_color(png_bytep* buffer, int height, int width, int color){
	
	int x = 0;
	int y = 0;
	for (x=0 ; x<height ; x++) {
		for (y=0 ; y<width ; y++) {
			for (int k = 0; k < 3; k++){
				buffer[x][3*y+k] = color;
			}
		}
	}
}

void draw_pixel(png_bytep* buffer, int x, int y){
	buffer[x][3*y]   = 255;
	buffer[x][3*y+1] = 255;
	buffer[x][3*y+2] = 255;
}

void draw_circle(png_bytep* buffer, int c_x, int c_y, int r){
	int x = 0;
	int y = r;
	int d = r-1;
	
	while(y >= x){
		draw_pixel(buffer, c_x+x, c_y+y);
		draw_pixel(buffer, c_x-x, c_y+y);
		draw_pixel(buffer, c_x+x, c_y-y);
		draw_pixel(buffer, c_x-x, c_y-y);
		draw_pixel(buffer, c_x+y, c_y+x);
		draw_pixel(buffer, c_x-y, c_y+x);
		draw_pixel(buffer, c_x+y, c_y-x);
		draw_pixel(buffer, c_x-y, c_y-x);
		
		
		if (d >= 2*x){
			d -= 2*x - 1;
			x++;
		}
		
		else if (d < 2*(r-y)){
			d += 2*y - 1;
			y--;
		}
		
		else {
			d += 2*(y-x-1);
			y--;
			x++;
		}
	}
}


