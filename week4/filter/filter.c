#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>

#include "helpers.h"

int main(int argc, char *argv[])
{

    // Define allowable filters
    char *filters = "bgrs";

    // Get filter flag and check validity
    char filter = getopt(argc, argv, filters);
    if (filter == '?')
    {
        fprintf(stderr, "Invalid filter.\n");
        return 1;
    }

    // Ensure only one filter
    if (getopt(argc, argv, filters) != -1)
    {
        fprintf(stderr, "Only one filter allowed.\n");
        return 2;
    }

    // Ensure proper usage
    if (argc != optind + 2)
    {
        fprintf(stderr, "Usage: filter [flag] inputfile outputfile\n");
        return 3;
    }

    // Remember filenames
    char *inputfile = argv[optind];
    char *outputfile = argv[optind + 1];

    // Open input file
    FILE *inputtr = fopen(inputfile, "r");
    if (inputtr == NULL)
    {
        fprintf(stderr, "Could not open %s.\n", inputfile);
        return 4;
    }

    // Open output file
    FILE *outputtr = fopen(outputfile, "w");
    if (outputtr == NULL)
    {
        fclose(inputtr);
        fprintf(stderr, "Could not create %s.\n", outputfile);
        return 5;
    }

    // Read inputfile's BITMAPFILEHEADER
    BITMAPFILEHEADER bf;
    fread(&bf, sizeof(BITMAPFILEHEADER), 1, inputtr);

    // Read inputfile's BITMAPINFOHEADER
    BITMAPINFOHEADER bi;
    fread(&bi, sizeof(BITMAPINFOHEADER), 1, inputtr);

    // Ensure inputfile is (likely) a 24-bit uncompressed BMP 4.0
    if (bf.bfType != 0x4d42 || bf.bfOffBits != 54 || bi.biSize != 40 ||
        bi.biBitCount != 24 || bi.biCompression != 0)
    {
        fclose(outputtr);
        fclose(inputtr);
        fprintf(stderr, "Unsupported file format.\n");
        return 6;
    }

    int height = abs(bi.biHeight);
    int width = bi.biWidth;

    // Allocate memory for image
    RGBTRIPLE(*image)[width] = calloc(height, width * sizeof(RGBTRIPLE));
    if (image == NULL)
    {
        fprintf(stderr, "Not enough memory to store image.\n");
        fclose(outputtr);
        fclose(inputtr);
        return 7;
    }

    // Determine padding for scanlines
    int padding = (4 - (width * sizeof(RGBTRIPLE)) % 4) % 4;

    // Iterate over inputfile's scanlines
    for (int i = 0; i < height; i++)
    {
        // Read row into pixel array
        fread(image[i], sizeof(RGBTRIPLE), width, inputtr);

        // Skip over padding
        fseek(inputtr, padding, SEEK_CUR);
    }

    // Filter image
    switch (filter)
    {
        // Blur
        case 'b':
            blur(height, width, image);
            break;

        // Grayscale
        case 'g':
            grayscale(height, width, image);
            break;

        // Reflection
        case 'r':
            reflect(height, width, image);
            break;

        // Sepia
        case 's':
            sepia(height, width, image);
            break;
    }

    // Write outputfile's BITMAPFILEHEADER
    fwrite(&bf, sizeof(BITMAPFILEHEADER), 1, outputtr);

    // Write outputfile's BITMAPINFOHEADER
    fwrite(&bi, sizeof(BITMAPINFOHEADER), 1, outputtr);

    // Write new pixels to outputfile
    for (int i = 0; i < height; i++)
    {
        // Write row to outputfile
        fwrite(image[i], sizeof(RGBTRIPLE), width, outputtr);

        // Write padding at end of row
        for (int k = 0; k < padding; k++)
        {
            fputc(0x00, outputtr);
        }
    }

    // Free memory for image
    free(image);

    // Close inputfile
    fclose(inputtr);

    // Close outputfile
    fclose(outputtr);

    return 0;
}
