#include <stdio.h>
#include <stdlib.h>

void buildTree(int height, FILE *file);
void writeFile(FILE *file, char c);

int main(int argc, char *argv[]){

    int height; 
    FILE *file = NULL;

    if(argc == 1 ){
        printf( "Enter the Height of the Tree: " );
        scanf( "%d", &height );
        printf("%c", '\n');
    }
    else if( argc > 2 ){
        file = fopen( argv[2], "w" );
        height = atoi(argv[1]);
    }
    else{
        height = atoi(argv[1]);
    }

    if( height >= 0 && height <= 15){
        buildTree(height, file);
        return 0;
    }
    
    return -1;
    
}

void buildTree(int height, FILE *file){
    for(int i = 1; i <= height; i++){
        if( height - i >= 3){
            for(int j = 0; j < height - i - 3; j++){
                writeFile(file, ' ');
            }
            for(int j = 0; j < 2*i - 1; j++){
                writeFile(file, '*');
            }
        }
        else{
            for(int i = 0; i < height - 4; i++){
                writeFile(file, ' ');
            }
                writeFile(file, '*');
        }
        writeFile(file, '\n');
    }
}

void writeFile(FILE *file, char c)
{
    if( file == NULL ){
        putchar(c);
    }
    else{
        fprintf( file, "%c", c);

    }
}

