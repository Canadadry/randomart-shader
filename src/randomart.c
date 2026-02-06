#include "../vendor/raylib/raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include "dynamicarray.h"

typedef enum{
    NK_X,
    NK_Y,
    NK_NUMBER,
    NK_ADD,
    NK_MUL
}NodeKind;

typedef int NodeIndex;

typedef struct{
    NodeIndex left;
    NodeIndex right;
}InfixNode;

typedef struct {
    NodeKind kind;
    union {
        float number;
        InfixNode infix;
    } value;
} Node;

CREATE_ARRAY_TYPE(Node);
typedef struct{
    ARRAY(Node) nodes;
    NodeIndex head;
}Program;

// static NodeIndex program_add_node(Program *p, Node n) {
//     array_append_Node(&p->nodes, n);
//     return (NodeIndex)(p->nodes.len - 1);
// }

float eval(Program p,NodeIndex head,float x, float y){
    switch(p.nodes.data[head].kind){
    case NK_X:return x;
    case NK_Y:return y;
    case NK_NUMBER:return p.nodes.data[head].value.number;
    case NK_ADD:return eval(p,p.nodes.data[head].value.infix.left,x,y)+eval(p,p.nodes.data[head].value.infix.right,x,y);
    case NK_MUL:return eval(p,p.nodes.data[head].value.infix.left,x,y)*eval(p,p.nodes.data[head].value.infix.right,x,y);
    }
    return 0;
}

void gen_img(unsigned char* data,int w, int h,Program p){
    for(int j=0;j<h;j++){
        for(int i=0;i<w;i++){
            float value = eval(p,p.head,((float)i/(float)w-0.5)*2,((float)j/(float)h-0.5)*2);
            data[(i+j*w)*4+0]=(value/2+0.5)*255;
            data[(i+j*w)*4+1]=(value/2+0.5)*255;
            data[(i+j*w)*4+2]=(value/2+0.5)*255;
            data[(i+j*w)*4+3]=255;
        }
    }
}

#define MAX_NODE 10
int main(void)
{
    Node nodes[MAX_NODE]={0};
    Program p =(Program){0};
    p.nodes.data=nodes;
    p.nodes.capacity=MAX_NODE;

    p.head=2;
    nodes[0].kind=NK_X;
    nodes[1].kind=NK_Y;
    nodes[2].kind=NK_MUL;
    nodes[2].value.infix.left=0;
    nodes[2].value.infix.right=1;

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
    gen_img(img.data,img.width,img.height,p);
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
