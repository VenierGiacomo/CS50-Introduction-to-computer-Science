#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    // check namefile
    if (argc != 2)
    {
        fprintf(stderr, "Usage: ./recover image\n");
        return 1;
    }

    // check memory 
    FILE *inputfile = fopen(argv[1], "r");
    if (inputfile == NULL)
    {
        fprintf(stderr, "Could not open inputfile %s.\n", argv[1]);
        return 1;
    }

    //create vars and allocate memory 
    FILE *img;
    char name[7];
    unsigned char *bf = malloc(512);
    int end = 1000;
    int counter = 0;

    while (fread(bf, 512, 1, inputfile))
    {
        // new jpg found
        if (bf[0] == 0xff && bf[1] == 0xd8 && bf[2] == 0xff && (bf[3] & 0xf0) == 0xe0)
        {
            // close previous jpg 
            if (counter > 0)
            {
                fclose(img);
            }

            // generate name
            printf(name, "%03d.jpg", counter);
            // open new image 
            img = fopen(name, "w");

            // check if jpg is created
            if (img == NULL)
            {
                fclose(inputfile);
                free(bf);
                fprintf(stderr, "Could not create output JPG %s", name);
                return 3;
            }

            counter++;
        }

        //if any jpg, writes on the file opened
        if (counter > 0)
        {
            fwrite(bf, 512, 1, img);
        }
    }

    //frees memory and closes files
    fclose(img);
    fclose(inputfile);
    free(bf);
    return 0;
}
