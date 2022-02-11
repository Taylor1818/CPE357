#include <stdio.h>
#include <stdlib.h>

typedef unsigned short SHORT;
typedef unsigned long LONG;
typedef long LONG1;
typedef unsigned char BYTE;

typedef struct col
{
    int r, g, b; //red, green and blue, in that order. The values will not exceed 255!
} col;

typedef struct compressedformat
{
    int width, height;      //width and height of the image, with one byte for each color, blue, green and red
    int rowbyte_quarter[4]; //for parallel algorithms! That’s the location in bytes which exactly splits the result image after decompression into 4 equal parts!
    int palettecolors;      //how many colors does the picture have?
    col colors[11];         //all participating colors of the image. Further below is the structure “col” described
} compressedformat;

//It uses the structure “col”

//After the header  “compressedformat” follows the data part.
//The data part consists of chunks:
typedef struct chunk
{
    BYTE color_index; //which of the color of the palette
    short count;      //How many pixel of the same color from color_index are continuously appearing
} chunk;

typedef struct tagBITMAPFILEHEADER
{
    SHORT bfType;      //specifies the file type
    LONG bfSize;       //specifies the size in bytes of the bitmap file
    SHORT bfReserved1; //reserved; must be 0
    SHORT bfReserved2; //reserved; must be 0
    LONG bfOffBits;    //specifies the offset in bytes from the bitmapfileheader to the bitmap bits
} BITMAPFILEHEADER;

typedef struct tagBITMAPINFOHEADER
{
    LONG biSize;           //specifies the number of bytes required by the struct
    LONG1 biWidth;         //specifies width in pixels
    LONG1 biHeight;        //specifies height in pixels
    SHORT biPlanes;        //specifies the number of color planes, must be 1
    SHORT biBitCount;      //specifies the number of bits per pixel
    LONG biCompression;    //specifies the type of compression
    LONG biSizeImage;      //size of image in bytes
    LONG1 biXPelsPerMeter; //number of pixels per meter in x axis
    LONG1 biYPelsPerMeter; //number of pixels per meter in y axis
    LONG biClrUsed;        //number of colors used by the bitmap
    LONG biClrImportant;   //number of colors that are important
} BITMAPINFOHEADER;

typedef struct tagBIN
{
    BITMAPFILEHEADER FileHead;
    BITMAPINFOHEADER InfoHead;
    compressedformat compressedHead;
    chunk chunkHead;
    chunk *pairs;
    BYTE *data;
} BIN;

BYTE checkFile(char *filename)
{
    FILE *temp;
    if (temp = fopen(filename, "rb"))
    {
        fclose(temp);
        return 1;
    }
    return 0;
}

BIN readBIN(const char *name)
{
    BIN bin;
    FILE *fp = fopen(name, "rb");

    fread(&bin.compressedHead.height, 4, 1, fp);
    fread(&bin.compressedHead.width, 4, 1, fp);
    fread(&bin.compressedHead.rowbyte_quarter, 4, 4, fp);
    fread(&bin.compressedHead.palettecolors, 4, 1, fp);

    for (int i = 0; i < bin.compressedHead.palettecolors; i++)
    {
        fread(&bin.compressedHead.colors[i].r, 4, 1, fp);
        fread(&bin.compressedHead.colors[i].g, 4, 1, fp);
        fread(&bin.compressedHead.colors[i].b, 4, 1, fp);
    }

    bin.pairs = (chunk *)malloc(bin.compressedHead.width * bin.compressedHead.height * sizeof(chunk));

    int j = 0;
    while (fread(&bin.pairs[j].color_index, 1, 1, fp) == 1)
    {
        fread(&bin.pairs[j].count, 2, 1, fp);
        j++;
    }

    fread(&bin.InfoHead, sizeof(bin.InfoHead), 1, fp);

    bin.data = (BYTE *)malloc(3 * bin.compressedHead.width * bin.compressedHead.height);

    fread(bin.data, 3 * bin.compressedHead.width * bin.compressedHead.height, 1, fp);

    fclose(fp);
    
    return bin;
}

BIN createBMP(BIN new)
{
    new.FileHead.bfType = 19778;
    new.FileHead.bfSize = 4320054;
    new.FileHead.bfReserved1 = 0;
    new.FileHead.bfReserved2 = 0;
    new.FileHead.bfOffBits = 54;

    new.InfoHead.biSize = 40;
    new.InfoHead.biWidth = 1200;
    new.InfoHead.biHeight = 1200;
    new.InfoHead.biPlanes = 1;
    new.InfoHead.biBitCount = 24;
    new.InfoHead.biCompression = 0;
    new.InfoHead.biSizeImage = 4320000;
    new.InfoHead.biXPelsPerMeter = 3780;
    new.InfoHead.biYPelsPerMeter = 3780;
    new.InfoHead.biClrUsed = 0;
    new.InfoHead.biClrImportant = 0;

    return new;
}

void writeBIN(const char *name, BIN bin)
{
    FILE *fp = fopen(name, "wb");

    fwrite(&bin.FileHead.bfType, 2, 1, fp);
    fwrite(&bin.FileHead.bfSize, 4, 1, fp);
    fwrite(&bin.FileHead.bfReserved1, 2, 1, fp);
    fwrite(&bin.FileHead.bfReserved2, 2, 1, fp);
    fwrite(&bin.FileHead.bfOffBits, 4, 1, fp);

    fwrite(&bin.InfoHead.biSize, 4, 1, fp);
    fwrite(&bin.InfoHead.biWidth, 4, 1, fp);
    fwrite(&bin.InfoHead.biHeight, 4, 1, fp);
    fwrite(&bin.InfoHead.biPlanes, 2, 1, fp);
    fwrite(&bin.InfoHead.biBitCount, 2, 1, fp);
    fwrite(&bin.InfoHead.biCompression, 4, 1, fp);
    fwrite(&bin.InfoHead.biSizeImage, 4, 1, fp);
    fwrite(&bin.InfoHead.biXPelsPerMeter, 4, 1, fp);
    fwrite(&bin.InfoHead.biYPelsPerMeter, 4, 1, fp);
    fwrite(&bin.InfoHead.biClrUsed, 4, 1, fp);
    fwrite(&bin.InfoHead.biClrImportant, 4, 1, fp);

    fwrite(bin.data, bin.InfoHead.biSizeImage, 1, fp);

    fclose(fp);
}

void decompress(BIN bin)
{
    int x, n = 0;
    col color;

    while (x < bin.compressedHead.rowbyte_quarter[3] / 3)
    {
        color = bin.compressedHead.colors[bin.pairs[x].color_index];
        
        for (int i = 0; i < bin.pairs[x].count; i++)
        {
            bin.data[2 + n] = color.r;
            bin.data[1 + n] = color.g;
            bin.data[0 + n] = color.b;
            n += 3;
        }
        x++;
    }
}

void main(int argc, char *argv[])
{
    checkFile(argv[1]);
    struct tagBIN bin = readBIN(argv[1]);
    struct tagBIN bmpout = createBMP(bin);
    decompress(bin);
    writeBIN("Test.bmp", bmpout);
    free(bin.pairs);
    free(bin.data);
}