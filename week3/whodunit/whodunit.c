#include <stdio.h>
#include <stdlib.h>
#include<cs50.h>

#include "bmp.h"

int main(int argc, char* argv[])
{
    // ensure proper usage
    if (argc != 3)
    {
        printf("Usage: ./copy inputfile outputfile\n");
        return 1;
    }

    // remember filenames
    char* inputfile = argv[1];
    char* outputfile = argv[2];

    // open input file 
    FILE* inputtr = fopen(inputfile, "r");
    if (inputtr == NULL)
    {
        printf("Could not open %s.\n", inputfile);
        return 2;
    }

    // open output file
    FILE* outputptr = fopen(outputfile, "w");
    if (outputptr == NULL)
    {
        fclose(inputtr);
        fprintf(stderr, "Could not create %s.\n", outputfile);
        return 3;
    }

    // read inputfile's BITMAPFILEHEADER
    BITMAPFILEHEADER bf;
    fread(&bf, sizeof(BITMAPFILEHEADER), 1, inputtr);

    // read inputfile's BITMAPINFOHEADER
    BITMAPINFOHEADER bi;
    fread(&bi, sizeof(BITMAPINFOHEADER), 1, inputtr);

    // ensure inputfile is (likely) a 24-bit uncompressed BMP 4.0
    if (bf.bfType != 0x4d42 || bf.bfOffBits != 54 || bi.biSize != 40 || 
        bi.biBitCount != 24 || bi.biCompression != 0)
    {
        fclose(outputptr);
        fclose(inputtr);
        fprintf(stderr, "Unsupported file format.\n");
        return 4;
    }

    // write outputfile's BITMAPFILEHEADER
    fwrite(&bf, sizeof(BITMAPFILEHEADER), 1, outputptr);

    // write outputfile's BITMAPINFOHEADER
    fwrite(&bi, sizeof(BITMAPINFOHEADER), 1, outputptr);

    // determine padding for scanlines
    int padding =  (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;

    // iterate over inputfile's scanlines
    for (int i = 0, biHeight = abs(bi.biHeight); i < biHeight; i++)
    {
        // iterate over pixels in scanline
        for (int j = 0; j < bi.biWidth; j++)
        {
            // temporary storage
            RGBTRIPLE triple;

            // read RGB triple from inputfile
            fread(&triple, sizeof(RGBTRIPLE), 1, inputtr);

            if (triple.rgbtRed == 0xff)
            {
                triple.rgbtRed = 0x00;
            }

            // write RGB triple to outputfile
            fwrite(&triple, sizeof(RGBTRIPLE), 1, outputptr);
        }

        // skip over padding, if any
        fseek(inputtr, padding, SEEK_CUR);

        // then add it back (to demonstrate how)
        for (int k = 0; k < padding; k++)
        {
            fputc(0x00, outputptr);
        }
    }

    // close inputfile
    fclose(inputtr);

    // close outputfile
    fclose(outputptr);

    // that's all folks
    return 0;
}
