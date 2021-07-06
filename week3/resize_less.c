// Copies a BMP file

#include <stdio.h>
#include <stdlib.h>

#include "bmp.h"

int main(int argc, char *argv[])
{
    // ensure proper usage
    if (argc != 4)
    {
        fprintf(stderr, "Usage: copy inputfile outputfile\n");
        return 1;
    }

    //Get n from user input
    int n = atoi(argv[1]);

    //Make sure n is between 0 and 100
    if (n < 0 || n > 100)
    {
        printf("Resize value must be between 0 and 100\n");

        return 5;
    }

    // remember filenames
    char *inputfile = argv[2];
    char *outputfile = argv[3];

    // open input file
    FILE *inputtr = fopen(inputfile, "r");
    if (inputtr == NULL)
    {
        fprintf(stderr, "Could not open %s.\n", inputfile);
        return 2;
    }

    // open output file
    FILE *outputtr = fopen(outputfile, "w");
    if (outputtr == NULL)
    {
        fclose(inputtr);
        fprintf(stderr, "Could not create %s.\n", outputfile);
        return 3;
    }

    // read inputfile's BITMAPFILEHEADER and set outputfile BITMAPHEADERFILE equal to it.
    BITMAPFILEHEADER bf, bfR;
    fread(&bf, sizeof(BITMAPFILEHEADER), 1, inputtr);
    bfR = bf;

    // read inputfile's BITMAPINFOHEADER and set outputfile BITMAPINFOHEADER equal to it.
    BITMAPINFOHEADER bi, biR;
    fread(&bi, sizeof(BITMAPINFOHEADER), 1, inputtr);
    biR = bi;

    //Set the outputfile header files to adjust by resize value.
    biR.biWidth = bi.biWidth * n;
    biR.biHeight = bi.biHeight * n;

    // ensure inputfile is (likely) a 24-bit uncompressed BMP 4.0
    if (bf.bfType != 0x4d42 || bf.bfOffBits != 54 || bi.biSize != 40 ||
        bi.biBitCount != 24 || bi.biCompression != 0)
    {
        fclose(outputtr);
        fclose(inputtr);
        fprintf(stderr, "Unsupported file format.\n");
        return 4;
    }

    // determine padding for scanlines for inputfile and outputfile
    int padding = (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;
    int outPadding = (4 - (biR.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;

    //New image size
    bfR.bfSize = 54 + biR.biWidth * abs(biR.biHeight) * 3 + abs(biR.biHeight) *  outPadding;
	biR.biSizeImage = ((((biR.biWidth * biR.biBitCount) + 31) & ~31) / 8) * abs(biR.biHeight);

    // write outputfile's BITMAPFILEHEADER
    fwrite(&bfR, sizeof(BITMAPFILEHEADER), 1, outputtr);

    // write outputfile's BITMAPINFOHEADER
    fwrite(&biR, sizeof(BITMAPINFOHEADER), 1, outputtr);

    // iterate over inputfile's scanlines
    for (int i = 0, biHeight = abs(bi.biHeight); i < biHeight; i++)
    {
        // iterate over pixels in scanline
        for (int j = 0; j < n; j++)
        {

            for (int k = 0; k < bi.biWidth; k++)
            {
                // temporary storage
                RGBTRIPLE triple;

                // read RGB triple from inputfile
                fread(&triple, sizeof(RGBTRIPLE), 1, inputtr);

                // write RGB triple to outputfile n times
                for (int l = 0; l < n; l++)
                {
                    fwrite(&triple, sizeof(RGBTRIPLE), 1, outputtr);
                }


            }

            // add padding
            for (int m = 0; m < outPadding; m++)
            {
                fputc(0x00, outputtr);
            }

            if (j < n - 1)
            {
                fseek(inputtr, -bi.biWidth * sizeof(RGBTRIPLE), SEEK_CUR);
            }

        }

        // skip over padding, if any
        fseek(inputtr, padding, SEEK_CUR);


    }

    // close inputfile
    fclose(inputtr);

    // close outputfile
    fclose(outputtr);

    // success
    return 0;
}
