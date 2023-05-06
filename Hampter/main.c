#include <stdio.h>
#include <stdlib.h>
#include "lodepng.h"
#include "lodepng.c"

char *loadPng(const char *filename, unsigned int *width, unsigned int *height) {
    unsigned char *image = NULL;
    int error = lodepng_decode32_file(&image, width, height, filename);
    if (error) {
        printf("error %u: %s\n", error, lodepng_error_text(error));
    }
    return (image);
}

void writePng(const char *filename, const unsigned char *image, unsigned width, unsigned height) {
    unsigned char *png;
    long unsigned int png_size;
    int error = lodepng_encode32(&png, &png_size, image, width, height);
    if (!error) {
        lodepng_save_file(png, png_size, filename);
    }
    if (error)
        printf("error %u: %s\n", error, lodepng_error_text(error));
    free(png);
}

void preparation(unsigned char *M_i, int w, int h) {
    int i, j;
    for (i = 0; i < h; i++) {
        for (j = 0; j < w; j++) {
            if (M_i[w * i + j] < 40)
                M_i[w * i + j] = 0;
            if (M_i[w * i + j] > 200)
                M_i[w * i + j] = 255;
        }
    }
    return;
}

void Gauss(unsigned char *M_i, unsigned char *M_p, int w, int h) {
    int i, j;
    for (i = 1; i < h - 1; i++) {
        for (j = 1; j < w - 1; j++) {
            M_p[w * i + j] = 0.12 * M_i[w * i + j] + 0.12 * M_i[w * (i + 1) + j] + 0.12 * M_i[w * (i - 1) + j];
            M_p[w * i + j] = M_p[i * w + j] + 0.12 * M_i[w * i + (j + 1)] + 0.12 * M_i[w * i + (j - 1)];
            M_p[w * i + j] = M_p[i * w + j] + 0.09 * M_i[w * (i + 1) + (j + 1)] + 0.09 * M_i[w * (i + 1) + (j - 1)];
            M_p[w * i + j] = M_p[i * w + j] + 0.09 * M_i[w * (i - 1) + (j + 1)] + 0.09 * M_i[w * (i - 1) + (j - 1)];
        }
    }
    return;
}

void Colouring(unsigned char *M_p, unsigned char *M_f, int w, int h) {
    int i;
    for (i = 0; i < w * h - 1; i++) {
        M_f[i * 4] = 80 + M_p[i + 1] + 0.5 * M_p[i];
        M_f[i * 4 + 1] = 20 + M_p[i];
        M_f[i * 4 + 2] = 98 + M_p[i];
        M_f[i * 4 + 3] = 255;
    }
    return;
}

int main() {
    char *filename = "skull.png";
    unsigned int w, h;
    int n, i, j, k = 0;
    unsigned char *picture = loadPng(filename, &w, &h);
    if (picture == NULL) {
        printf("I can not read the picture from the file %s. Error.\n", filename);
        return -1;
    }
    unsigned char *Mat_init = (unsigned char *) malloc(w * h * sizeof(unsigned char));
    unsigned char *Mat_process = (unsigned char *) malloc(w * h * sizeof(unsigned char));
    unsigned char *Mat_final = (unsigned char *) malloc(w * h * 4 * sizeof(unsigned char));
    for (i = 0; i < w * h * 4; i = i + 4) {
        Mat_init[k] = 0.299 * picture[i] + 0.587 * picture[i + 1] + 0.114 * picture[i + 2];
        k++;
    }
    preparation(Mat_init, w, h);
    Gauss(Mat_init, Mat_process, w, h);
    Colouring(Mat_process, Mat_final, w, h);
    char *new_image = "skull-modified.png";
    writePng(new_image, Mat_final, w, h);
    return 0;
}