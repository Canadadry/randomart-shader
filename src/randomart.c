#include "../vendor/raylib/raylib.h"
#include <stdio.h>
#include <stdlib.h>

void gen_img(unsigned char* data,int w, int h){
    for(int j=0;j<h;j++){
        for(int i=0;i<w;i++){
            data[(i+j*w)*4+0]=i*255/w;
            data[(i+j*w)*4+1]=i*255/w;
            data[(i+j*w)*4+2]=i*255/w;
            data[(i+j*w)*4+3]=255;
        }
    }
}

int main(void)
{
    const int screenWidth = 800;
    const int screenHeight = 450;
    InitWindow(screenWidth, screenHeight, "raylib [shaders] example - multi sample2d");

    Image img = (Image){0};
    img.width=screenWidth-20;
    img.height=screenHeight-20;
    img.mipmaps = 1;
    img.format=PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;
    img.data=malloc(img.width*img.height*4*sizeof(unsigned char));
    if(img.data==NULL){
        printf("cannot allocate buf\n");
        return 1;
    }
    gen_img(img.data,img.width,img.height);
    Texture2D tex = LoadTextureFromImage(img);
    free(img.data);


    SetTargetFPS(60);
    while (!WindowShouldClose())
    {
        BeginDrawing();
            ClearBackground(YELLOW);
            DrawTexture(tex,10,10,WHITE);
        EndDrawing();
    }
    UnloadTexture(tex);
    CloseWindow();

    return 0;
}
