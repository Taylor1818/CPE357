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
    col *colors;          //all participating colors of the image. Further below is the structure “col” described
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
    LONG bfSize;     //specifies the size in bytes of the bitmap file
    SHORT bfReserved1; //reserved; must be 0
    SHORT bfReserved2; //reserved; must be 0
    LONG bfOffBits;  //specifies the offset in bytes from the bitmapfileheader to the bitmap bits
} BITMAPFILEHEADER;

typedef struct tagBITMAPINFOHEADER
{
    LONG biSize;         //specifies the number of bytes required by the struct
    LONG1 biWidth;         //specifies width in pixels
    LONG1 biHeight;        //specifies height in pixels
    SHORT biPlanes;        //specifies the number of color planes, must be 1
    SHORT biBitCount;      //specifies the number of bits per pixel
    LONG biCompression;  //specifies the type of compression
    LONG biSizeImage;    //size of image in bytes
    LONG1 biXPelsPerMeter; //number of pixels per meter in x axis
    LONG1 biYPelsPerMeter; //number of pixels per meter in y axis
    LONG biClrUsed;      //number of colors used by the bitmap
    LONG biClrImportant; //number of colors that are important
} BITMAPINFOHEADER;

typedef struct tagBIN
{
    BITMAPFILEHEADER FileHead;
    BITMAPINFOHEADER InfoHead;
    compressedformat compressedHead;
    chunk chunkHead;
    BYTE *data;
    int WIDTH;
    int HEIGHT;
    int pixelWidth;
} BIN;

// BYTE checkFile(char *filename)
// {
//     FILE *temp;
//     if (temp = fopen(filename, "rb"))
//     {
//         fclose(temp);
//         return 1;
//     }
//     return 0;
// }

// unsigned char getBlue(int x, int y, BMP bmp)
// {
//     if (x > bmp.WIDTH)
//         x = bmp.WIDTH;
//     if (x < 0)
//         x = 0;
//     if (y > bmp.HEIGHT)
//         y = bmp.HEIGHT;
//     if (y < 0)
//         y = 0;
//     return bmp.data[x * 3 + y * bmp.pixelWidth + 0];
// }

// unsigned char getGreen(int x, int y, BMP bmp)
// {
//     if (x > bmp.WIDTH)
//         x = bmp.WIDTH;
//     if (x < 0)
//         x = 0;
//     if (y > bmp.HEIGHT)
//         y = bmp.HEIGHT;
//     if (y < 0)
//         y = 0;
//     return bmp.data[x * 3 + y * bmp.pixelWidth + 1];
// }

// unsigned char getRed(int x, int y, BMP bmp)
// {
//     if (x > bmp.WIDTH)
//         x = bmp.WIDTH;
//     if (x < 0)
//         x = 0;
//     if (y > bmp.HEIGHT)
//         y = bmp.HEIGHT;
//     if (y < 0)
//         y = 0;
//     return bmp.data[x * 3 + y * bmp.pixelWidth + 2];
// }

// void setBlue(int x, int y, BMP bmp, unsigned char val)
// {
//     bmp.data[x * 3 + y * bmp.pixelWidth + 0] = val;
//     return;
// }

// void setGreen(int x, int y, BMP bmp, unsigned char val)
// {
//     bmp.data[x * 3 + y * bmp.pixelWidth + 1] = val;
//     return;
// }

// void setRed(int x, int y, BMP bmp, unsigned char val)
// {
//     bmp.data[x * 3 + y * bmp.pixelWidth + 2] = val;
//     return;
// }

BIN readBIN(const char *name)
{
    BIN bin;
    FILE *fp = fopen(name, "rb");

    fread(&bin.compressedHead.height, 4, 1, fp);
    fread(&bin.compressedHead.width, 4, 1, fp);
    fread(&bin.compressedHead.rowbyte_quarter, 4, 4, fp);
    fread(&bin.compressedHead.palettecolors, 4, 1, fp);

    bin.compressedHead.colors = (col*)malloc(bin.compressedHead.palettecolors* sizeof(col));

    for(int i = 0; i < bin.compressedHead.palettecolors; i++){
        fread(&bin.compressedHead.colors[i].r, 4, 1, fp);
        fread(&bin.compressedHead.colors[i].g, 4, 1, fp);
        fread(&bin.compressedHead.colors[i].b, 4, 1, fp);
    }

    chunk *pairs = (chunk*)malloc(bin.compressedHead.width*bin.compressedHead.height*sizeof(chunk));
    
    int j = 0; 
    while(fread(&pairs[j].color_index, 1, 1, fp) == 1){
        fread(&pairs[j].count, 2, 1, fp);
        j++;
    }


    printf("%u", bin.chunkHead.count);

    fclose(fp);

    return bin;
}

// BMP cloneBMP(BMP bmp1, BMP bmp2)
// {
//     BMP new;
//     BMP *bmp = &bmp1;

//     if (bmp2.InfoHead.biWidth > bmp1.InfoHead.biWidth)
//     {
//         bmp = &bmp2;
//     }

//     new.FileHead = bmp->FileHead;
//     new.InfoHead = bmp->InfoHead;
//     new.data = (BYTE *)malloc(new.InfoHead.biSizeImage);
//     new.WIDTH = bmp->WIDTH;
//     new.HEIGHT = bmp->HEIGHT;
//     new.pixelWidth = bmp->pixelWidth;
//     return new;
// }

void writeBIN(const char *name, BIN bin)
{
    FILE *fp = fopen(name, "wb");
    fwrite(&bin.FileHead.bfType, 2, 1, fp);
    fwrite(&bin.FileHead.bfSize, 8, 1, fp);
    fwrite(&bin.FileHead.bfReserved1, 2, 1, fp);
    fwrite(&bin.FileHead.bfReserved2, 2, 1, fp);
    fwrite(&bin.FileHead.bfOffBits, 8, 1, fp);
    fwrite(&bin.InfoHead, sizeof(bin.InfoHead), 1, fp);
    fwrite(bin.data, bin.InfoHead.biSizeImage, 1, fp);
    fclose(fp);
}


// void decompress()
// {

// }

void main(int argc, char *argv[])
{
    readBIN(argv[1]);
    
}