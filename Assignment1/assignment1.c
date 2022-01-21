#include <stdio.h>
#include <stdlib.h>


typedef unsigned short WORD;
typedef unsigned int DWORD;
typedef unsigned int LONG;
typedef unsigned char BYTE;

#define manual "Manual:\n\nSpecify the following parameters:\n\t1. Input File 1\n\t2. Input File 2\n\t3. Blend ratio (0.0-1.0)\n\t4. Output File Name\n\n"


typedef struct tagBITMAPFILEHEADER
{
    WORD bfType;  //specifies the file type
    DWORD bfSize;  //specifies the size in bytes of the bitmap file
    WORD bfReserved1;  //reserved; must be 0
    WORD bfReserved2;  //reserved; must be 0
    DWORD bfOffBits;  //specifies the offset in bytes from the bitmapfileheader to the bitmap bits
}BITMAPFILEHEADER;

typedef struct tagBITMAPINFOHEADER
{
    DWORD biSize;  //specifies the number of bytes required by the struct
    LONG biWidth;  //specifies width in pixels
    LONG biHeight;  //specifies height in pixels
    WORD biPlanes;  //specifies the number of color planes, must be 1
    WORD biBitCount;  //specifies the number of bits per pixel
    DWORD biCompression;  //specifies the type of compression
    DWORD biSizeImage;  //size of image in bytes
    LONG biXPelsPerMeter;  //number of pixels per meter in x axis
    LONG biYPelsPerMeter;  //number of pixels per meter in y axis
    DWORD biClrUsed;  //number of colors used by the bitmap
    DWORD biClrImportant;  //number of colors that are important
}BITMAPINFOHEADER;

typedef struct tagBMP
{
    BITMAPFILEHEADER FileHead;
    BITMAPINFOHEADER InfoHead;
    BYTE* data;
    int WIDTH;
    int HEIGHT;
    int pixelWidth;
}BMP;

BYTE checkFile(char* filename){
    FILE *temp;
    if(temp = fopen(filename, "rb")){
        fclose(temp);
        return 1;
    }
    return 0;
}

unsigned char getBlue(int x, int y, BMP bmp){
    if(x>bmp.WIDTH) x = bmp.WIDTH;
    if(x<0) x = 0;
    if(y>bmp.HEIGHT) y = bmp.HEIGHT;
    if(y<0) y = 0;
    return bmp.data[x*3+y*bmp.pixelWidth+0];
}

unsigned char getGreen(int x, int y, BMP bmp){
    if(x>bmp.WIDTH) x = bmp.WIDTH;
    if(x<0) x = 0;
    if(y>bmp.HEIGHT) y = bmp.HEIGHT;
    if(y<0) y = 0;
    return bmp.data[x*3+y*bmp.pixelWidth+1];
}

unsigned char getRed(int x, int y, BMP bmp){
    if(x>bmp.WIDTH) x = bmp.WIDTH;
    if(x<0) x = 0;
    if(y>bmp.HEIGHT) y = bmp.HEIGHT;
    if(y<0) y = 0;
    return bmp.data[x*3+y*bmp.pixelWidth+2];
}

void setBlue(int x, int y, BMP bmp, unsigned char val){
    bmp.data[x*3+y*bmp.pixelWidth+0]=val;return;
}

void setGreen(int x, int y, BMP bmp, unsigned char val){
    bmp.data[x*3+y*bmp.pixelWidth+1]=val;return;
}

void setRed(int x, int y, BMP bmp, unsigned char val){
    bmp.data[x*3+y*bmp.pixelWidth+2]=val;return;
}

BMP readBMP(const char* name){
    BMP bmp;
    FILE *fp = fopen(name, "rb");

    fread(&bmp.FileHead.bfType, 2, 1, fp);
    fread(&bmp.FileHead.bfSize, 4, 1, fp);
    fread(&bmp.FileHead.bfReserved1, 2, 1, fp);
    fread(&bmp.FileHead.bfReserved2, 2, 1, fp);
    fread(&bmp.FileHead.bfOffBits, 4, 1, fp);
    fread(&bmp.InfoHead, sizeof(bmp.InfoHead), 1, fp);

    bmp.data = (BYTE*) malloc(bmp.InfoHead.biSizeImage);

    fread(bmp.data, bmp.InfoHead.biSizeImage,1,fp);
    fclose(fp);   

    bmp.WIDTH=bmp.InfoHead.biWidth;
    bmp.HEIGHT=bmp.InfoHead.biHeight;
    bmp.pixelWidth = 3*bmp.WIDTH;

    if(4-bmp.pixelWidth%4 != 4){
            bmp.pixelWidth = bmp.pixelWidth+ 4-bmp.pixelWidth%4;
    }
    return bmp;
}

BMP cloneBMP(BMP bmp1, BMP bmp2){
    BMP new;
    BMP *bmp = &bmp1;

    if( bmp2.InfoHead.biWidth > bmp1.InfoHead.biWidth ){
       bmp = &bmp2; 
    }

    new.FileHead = bmp -> FileHead;
    new.InfoHead = bmp -> InfoHead;
    new.data = (BYTE*) malloc(new.InfoHead.biSizeImage);
    new.WIDTH = bmp -> WIDTH;
    new.HEIGHT = bmp -> HEIGHT;
    new.pixelWidth = bmp -> pixelWidth;
    return new;
}

void writeBMP(const char* name, BMP bmp){
    FILE *fp = fopen(name,"wb");
    fwrite(&bmp.FileHead.bfType, 2, 1, fp);
    fwrite(&bmp.FileHead.bfSize, 4, 1, fp);
    fwrite(&bmp.FileHead.bfReserved1, 2, 1, fp);
    fwrite(&bmp.FileHead.bfReserved2, 2, 1, fp);
    fwrite(&bmp.FileHead.bfOffBits, 4, 1, fp);
    fwrite(&bmp.InfoHead, sizeof(bmp.InfoHead), 1, fp);
    fwrite(bmp.data, bmp.InfoHead.biSizeImage,1,fp);
    fclose(fp);
}

BYTE blur(BYTE c1, BYTE c2, float ratio){
    return((c1*ratio)+(c2*(1-ratio)));
}

void interpolate(int x, int y, BMP bmp1, BMP bmp2, BYTE* color){
    
    double xdub = x * bmp2.WIDTH / bmp1.WIDTH;
    double ydub = y * bmp2.HEIGHT / bmp1.HEIGHT;

    int colorX = (int)xdub;
    float xDif = xdub-colorX;

    int colorY = (int)ydub;
    float yDif = ydub-colorY;

    BYTE trr = getRed(colorX, colorY+1, bmp2); // Top Right 
    BYTE trg = getGreen(colorX, colorY+1, bmp2);
    BYTE trb = getBlue(colorX, colorY+1, bmp2);

    BYTE brr = getRed(colorX, colorY, bmp2); // Bottom Right
    BYTE brg = getGreen(colorX, colorY, bmp2);
    BYTE brb = getBlue(colorX, colorY, bmp2);

    BYTE tlr = getRed(colorX+1, colorY+1, bmp2); // Top Left
    BYTE tlg = getGreen(colorX+1, colorY+1, bmp2);
    BYTE tlb = getBlue(colorX+1, colorY+1, bmp2);

    BYTE blr = getRed(colorX+1, colorY, bmp2); // Bottom Left
    BYTE blg = getGreen(colorX+1, colorY, bmp2);
    BYTE blb = getBlue(colorX+1, colorY, bmp2);

    BYTE leftR = blur(tlr, blr, yDif);
    BYTE leftG = blur(tlg, blg, yDif);
    BYTE leftB = blur(tlb, blb, yDif);

    BYTE rightR = blur(trr, brr, yDif);
    BYTE rightG = blur(trg, brg, yDif);
    BYTE rightB = blur(trb, brb, yDif);

    color[0] = blur(leftB, rightB, xDif);
    color[1] = blur(leftG, rightG, xDif);
    color[2] = blur(leftR, rightR, xDif);
}


void main(int argc, char *argv[]){

    if (argc != 5){
        printf(manual);
        return;
    }

    if (((float)atof(argv[3]) < 0) || ((float)atof(argv[3]) > 1)){
        printf("Error: Invalid Ratio\nRatio must be in range 0-1\n");
        return;
    }

    struct tagBMP bmp1, bmp2, bmpOut;

    for(int i = 1; i<=2; i++){
        if(!checkFile(argv[i])){
            printf("Error: Input file %d does not exis\n", i);
            return;
        }
    }

    bmp1 = readBMP(argv[1]);
    bmp2 = readBMP(argv[2]);
    

    if(bmp1.InfoHead.biWidth < bmp2.InfoHead.biWidth){
        BMP temp = bmp2;
        bmp2 = bmp1;
        bmp1 = temp;
    }

    bmpOut = cloneBMP(bmp1, bmp2);

    float ratio = (float)atof(argv[3]);
    
    for(int y = 0; y < bmpOut.HEIGHT; y++){
        for(int x = 0; x < bmpOut.WIDTH; x++){
            
            BYTE color[3];
            interpolate(x,y, bmp1, bmp2, color);

            BYTE b1 = getBlue(x, y, bmp1);
            setBlue(x, y, bmpOut, blur(b1, color[0], ratio));
            
            BYTE g1 = getGreen(x, y, bmp1);
            setGreen(x, y, bmpOut, blur(g1, color[1], ratio));

            BYTE r1 = getRed(x, y, bmp1);
            setRed(x, y, bmpOut, blur(r1, color[2], ratio));
        }
    }

    writeBMP(argv[4],bmpOut);
    printf("\nSuccess: Files Blended\n\n");
    free(bmp1.data);
    free(bmp2.data);
    free(bmpOut.data);
}