#include <stdio.h>
#include <stdlib.h>

typedef unsigned short WORD;
typedef unsigned int DWORD;
typedef unsigned int LONG;
typedef unsigned char BYTE;
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
    col colors[3];          //all participating colors of the image. Further below is the structure “col” described
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
    WORD bfType;      //specifies the file type
    DWORD bfSize;     //specifies the size in bytes of the bitmap file
    WORD bfReserved1; //reserved; must be 0
    WORD bfReserved2; //reserved; must be 0
    DWORD bfOffBits;  //specifies the offset in bytes from the bitmapfileheader to the bitmap bits
} BITMAPFILEHEADER;

typedef struct tagBITMAPINFOHEADER
{
    DWORD biSize;         //specifies the number of bytes required by the struct
    LONG biWidth;         //specifies width in pixels
    LONG biHeight;        //specifies height in pixels
    WORD biPlanes;        //specifies the number of color planes, must be 1
    WORD biBitCount;      //specifies the number of bits per pixel
    DWORD biCompression;  //specifies the type of compression
    DWORD biSizeImage;    //size of image in bytes
    LONG biXPelsPerMeter; //number of pixels per meter in x axis
    LONG biYPelsPerMeter; //number of pixels per meter in y axis
    DWORD biClrUsed;      //number of colors used by the bitmap
    DWORD biClrImportant; //number of colors that are important
} BITMAPINFOHEADER;

typedef struct tagBMP
{
    BITMAPFILEHEADER FileHead;
    BITMAPINFOHEADER InfoHead;
    BYTE *data;
    int WIDTH;
    int HEIGHT;
    int pixelWidth;
} BMP;

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

unsigned char getBlue(int x, int y, BMP bmp)
{
    if (x > bmp.WIDTH)
        x = bmp.WIDTH;
    if (x < 0)
        x = 0;
    if (y > bmp.HEIGHT)
        y = bmp.HEIGHT;
    if (y < 0)
        y = 0;
    return bmp.data[x * 3 + y * bmp.pixelWidth + 0];
}

unsigned char getGreen(int x, int y, BMP bmp)
{
    if (x > bmp.WIDTH)
        x = bmp.WIDTH;
    if (x < 0)
        x = 0;
    if (y > bmp.HEIGHT)
        y = bmp.HEIGHT;
    if (y < 0)
        y = 0;
    return bmp.data[x * 3 + y * bmp.pixelWidth + 1];
}

unsigned char getRed(int x, int y, BMP bmp)
{
    if (x > bmp.WIDTH)
        x = bmp.WIDTH;
    if (x < 0)
        x = 0;
    if (y > bmp.HEIGHT)
        y = bmp.HEIGHT;
    if (y < 0)
        y = 0;
    return bmp.data[x * 3 + y * bmp.pixelWidth + 2];
}

void setBlue(int x, int y, BMP bmp, unsigned char val)
{
    bmp.data[x * 3 + y * bmp.pixelWidth + 0] = val;
    return;
}

void setGreen(int x, int y, BMP bmp, unsigned char val)
{
    bmp.data[x * 3 + y * bmp.pixelWidth + 1] = val;
    return;
}

void setRed(int x, int y, BMP bmp, unsigned char val)
{
    bmp.data[x * 3 + y * bmp.pixelWidth + 2] = val;
    return;
}

BMP readBMP(const char *name)
{
    BMP bmp;
    FILE *fp = fopen(name, "rb");

    fread(&bmp.FileHead.bfType, 2, 1, fp);
    fread(&bmp.FileHead.bfSize, 4, 1, fp);
    fread(&bmp.FileHead.bfReserved1, 2, 1, fp);
    fread(&bmp.FileHead.bfReserved2, 2, 1, fp);
    fread(&bmp.FileHead.bfOffBits, 4, 1, fp);
    fread(&bmp.InfoHead, sizeof(bmp.InfoHead), 1, fp);

    bmp.data = (BYTE *)malloc(bmp.InfoHead.biSizeImage);

    fread(bmp.data, bmp.InfoHead.biSizeImage, 1, fp);
    fclose(fp);

    bmp.WIDTH = bmp.InfoHead.biWidth;
    bmp.HEIGHT = bmp.InfoHead.biHeight;
    bmp.pixelWidth = 3 * bmp.WIDTH;

    if (4 - bmp.pixelWidth % 4 != 4)
    {
        bmp.pixelWidth = bmp.pixelWidth + 4 - bmp.pixelWidth % 4;
    }
    return bmp;
}

BMP cloneBMP(BMP bmp1, BMP bmp2)
{
    BMP new;
    BMP *bmp = &bmp1;

    if (bmp2.InfoHead.biWidth > bmp1.InfoHead.biWidth)
    {
        bmp = &bmp2;
    }

    new.FileHead = bmp->FileHead;
    new.InfoHead = bmp->InfoHead;
    new.data = (BYTE *)malloc(new.InfoHead.biSizeImage);
    new.WIDTH = bmp->WIDTH;
    new.HEIGHT = bmp->HEIGHT;
    new.pixelWidth = bmp->pixelWidth;
    return new;
}

void writeBMP(const char *name, BMP bmp)
{
    FILE *fp = fopen(name, "wb");
    fwrite(&bmp.FileHead.bfType, 2, 1, fp);
    fwrite(&bmp.FileHead.bfSize, 4, 1, fp);
    fwrite(&bmp.FileHead.bfReserved1, 2, 1, fp);
    fwrite(&bmp.FileHead.bfReserved2, 2, 1, fp);
    fwrite(&bmp.FileHead.bfOffBits, 4, 1, fp);
    fwrite(&bmp.InfoHead, sizeof(bmp.InfoHead), 1, fp);
    fwrite(bmp.data, bmp.InfoHead.biSizeImage, 1, fp);
    fclose(fp);
}


void decompress()
{

}

void main(int argc, char *argv[])
{
    BMP bmp;
    FILE *fp = fopen(argv[1], "rb");

    fread(&bmp.FileHead.bfType, 2, 1, fp);
    fread(&bmp.FileHead.bfSize, 4, 1, fp);
    fread(&bmp.FileHead.bfReserved1, 2, 1, fp);
    fread(&bmp.FileHead.bfReserved2, 2, 1, fp);
    fread(&bmp.FileHead.bfOffBits, 4, 1, fp);
    fread(&bmp.InfoHead, sizeof(bmp.InfoHead), 1, fp);


    bmp.data = (BYTE *)malloc(bmp.InfoHead.biSizeImage);

    fread(bmp.data, bmp.InfoHead.biSizeImage, 1, fp);
    printf(bmp.FileHead.bfSize);
    fclose(fp);

    bmp.WIDTH = bmp.InfoHead.biWidth;
    bmp.HEIGHT = bmp.InfoHead.biHeight;
    bmp.pixelWidth = 3 * bmp.WIDTH;


    free(bmp.data);
    
    }