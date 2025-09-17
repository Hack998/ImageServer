#define STB_IMAGE_IMPLEMENTATION
#include "../include/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../include/stb_image_write.h"

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "../include/image_proc.h"

static void hist_equalize_gray(unsigned char *buf, int w, int h)
{
    uint32_t hist[256] = {0};
    uint32_t cdf[256] = {0};
    int npix = w * h;
    for (int i = 0; i < npix; ++i)
        hist[buf[i]]++;
    uint32_t accum = 0;
    for (int k = 0; k < 256; k++)
    {
        accum += hist[k];
        cdf[k] = accum;
    }
    for (int i = 0; i < npix; i++)
    {
        unsigned char p = buf[i];
        buf[i] = (unsigned char)((double)cdf[p] * 255.0 / npix + 0.5);
    }
}

int process_image(const char *data, size_t size, const char *filename,
                  char *category, size_t clen, char *outpath, size_t olen)
{
    int w, h, c;
    unsigned char *img = stbi_load_from_memory((const unsigned char *)data, size, &w, &h, &c, 3);
    if (!img)
        return -1;

    // Separar canales
    int npix = w * h;
    unsigned long long sumR = 0, sumG = 0, sumB = 0;
    for (int i = 0; i < npix; i++)
    {
        sumR += img[3 * i + 0];
        sumG += img[3 * i + 1];
        sumB += img[3 * i + 2];
    }
    // Ecualizar canal Y aproximado = gris
    unsigned char *Y = malloc(npix);
    for (int i = 0; i < npix; i++)
    {
        Y[i] = (unsigned char)(0.299 * img[3 * i + 0] + 0.587 * img[3 * i + 1] + 0.114 * img[3 * i + 2]);
    }
    hist_equalize_gray(Y, w, h);
    for (int i = 0; i < npix; i++)
    {
        int diff = Y[i] - (0.299 * img[3 * i + 0] + 0.587 * img[3 * i + 1] + 0.114 * img[3 * i + 2]);
        int r = img[3 * i + 0] + diff;
        int g = img[3 * i + 1] + diff;
        int b = img[3 * i + 2] + diff;
        if (r < 0)
            r = 0;
        else if (r > 255)
            r = 255;
        if (g < 0)
            g = 0;
        else if (g > 255)
            g = 255;
        if (b < 0)
            b = 0;
        else if (b > 255)
            b = 255;
        img[3 * i + 0] = r;
        img[3 * i + 1] = g;
        img[3 * i + 2] = b;
    }
    free(Y);

    // Clasificación
    if (sumR >= sumG && sumR >= sumB)
        strncpy(category, "rojo", clen);
    else if (sumG >= sumR && sumG >= sumB)
        strncpy(category, "verde", clen);
    else
        strncpy(category, "azul", clen);

    // Guardar
    char ts[64];
    time_t now = time(NULL);
    strftime(ts, sizeof(ts), "%Y%m%d_%H%M%S", localtime(&now));
    snprintf(outpath, olen, "/var/lib/image_server/output/%s/%s.png", category, ts);

    stbi_write_png(outpath, w, h, 3, img, w * 3);

    stbi_image_free(img);
    return 0;
}