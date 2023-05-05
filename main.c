#include "lodepng.h"
#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <stdlib.h>

char* loadPng(const char* filename, int* width, int* height) {
    unsigned char* image = NULL;
//  unsigned int width, height;

    int error = lodepng_decode32_file(&image, width, height, filename);
    if(error)
    {
        printf("error %u: %s\n", error, lodepng_error_text(error));
    }

    return (image);
}



void writePng(const char* filename, const unsigned char* image, unsigned width, unsigned height) {
    unsigned char* png;
    int png_size;

    int error = lodepng_encode32(&png, &png_size, image, width, height);
    if(!error) {
        lodepng_save_file(png, png_size, filename);
    }

      /*if there's an error, display it*/
    if(error) printf("error %u: %s\n", error, lodepng_error_text(error));

    free(png);
}

void get_pixel(int x, int y, int *r, int *g, int *b, int *a, char* image, int width )
{
    *r =  image[4 * width * y + 4 * x + 0];
    *g =  image[4 * width * y + 4 * x + 1];
    *b =  image[4 * width * y + 4 * x + 2];
    *a =  image[4 * width * y + 4 * x + 3];

   return;
}

int IsBlack(int r, int g, int b){
    int gray = (r+g+b)/3;
    if(gray<128) return 1;
    else return 0;
}

void preparation(unsigned char *M_i, int w, int h){
    int i, j;
    for(i=0; i<h; i++){
        for(j=0; j<w; j++){
            if(M_i[w*i+j]<100) M_i[w*i+j] = 0;
            if(M_i[w*i+j]>160) M_i[w*i+j] = 255;
        }
    }
}

void Gauss(unsigned char *M_i, unsigned char *M_p, int w, int h){
    int i, j;
    for(i=2; i<h; i++){
        for(j=2; j<w; j++){
            M_p[w*i+j] = 0.12*M_i[w*i+j]+0.12*M_i[w*(i+1)+j]+0.12*M_i[w*(i-1)+j];
            M_p[w*i+j] = M_p[i*w+j]+0.12*M_i[i*w+(j+1)]+0.12*M_i[i*w+(j-1)];
            M_p[w*i+j] = M_p[i*w+j]+0.09*M_i[w*(i+1)+(j+1)]+0.09*M_i[w*(i+1)+(j-1)];
            M_p[w*i+j] = M_p[i*w+j]+0.09*M_i[w*(i-1)+(j+1)]+0.09*M_i[w*(i-1)+(j-1)];
        }
    }
    return;
}

void Colouring(unsigned char *M_p, unsigned char *M_f, int w, int h){
    int i, j;
    int c[w*h];
    for(i=0; i<w*h; i++){
        c[i] = M_p[i];
    }
    for(i=1; i<w*h*4; i++){
        M_f[i*4] = 80+c[i]+0.5*c[i-1];
        M_f[i*4+1] = 45+c[i];
        M_f[i*4+2] = 150+c[i];
        M_f[i*4+3] =255;
    }
    return;
}

int main() {

    char *filename = "skull.png";
    int w, h, n, i, j, k=0;
    char *picture = loadPng(filename, &w, &h);
    if (picture == NULL){
        printf("I can not read the picture from the file %s. Error.\n", filename);
        return -1;
    }


    unsigned char *Mat_init = (unsigned char*)malloc(w*h*sizeof(unsigned char));
    unsigned char *Mat_process = (unsigned char*)malloc(w*h*sizeof(unsigned char));
    unsigned char *Mat_final = (unsigned char*)malloc(w*h*4*sizeof(unsigned char));

    for(i=0; i<w*h*4; i+=4){
        Mat_init[k] = 0.299*Mat_init[i]+0.587*Mat_init[i+1]+0.114*Mat_init[i+2];
        k++;
    }
    preparation(Mat_init, w, h);
    Gauss(Mat_init, Mat_process, w, h);
    Colouring(Mat_process, Mat_final, w, h);

    char * new_image = "skull-modified.png";
    writePng(new_image, picture, w, h);

    return 0;
}

