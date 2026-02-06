#include "../vendor/raylib/raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "dynamicarray.h"

typedef enum{
    NK_X,
    NK_Y,
    NK_NUMBER,
    NK_ADD,
    NK_MUL,
    NK_VEC3
}NodeKind;

const char* print_nk(NodeKind nk){
    switch(nk){
        case NK_X:return "NK_X";
        case NK_Y:return "NK_Y";
        case NK_NUMBER:return "NK_NUMBER";
        case NK_ADD:return "NK_ADD";
        case NK_MUL:return "NK_MUL";
        case NK_VEC3:return "NK_VEC3";
    }
}

typedef int NodeIndex;

typedef struct{
    NodeIndex left;
    NodeIndex right;
}InfixNode;

typedef struct{
    NodeIndex x;
    NodeIndex y;
    NodeIndex z;
}Vec3Node;

typedef struct {
    NodeKind kind;
    union {
        float number;
        InfixNode infix;
        Vec3Node vec3;
    } value;
} Node;


CREATE_ARRAY_TYPE(Node);
typedef struct{
    ARRAY(Node) nodes;
    NodeIndex head;
}Program;

void print_node(Program p,NodeIndex idx){
    Node n= p.nodes.data[idx];
    switch (n.kind) {
        case NK_X: printf("x");break;
        case NK_Y: printf("y");break;
        case NK_NUMBER:printf("%f",n.value.number);break;
        case NK_ADD:
            printf("(");
            print_node(p,n.value.infix.left);
            printf("+");
            print_node(p,n.value.infix.right);
            printf(")");
            break;
        case NK_MUL:
            printf("(");
            print_node(p,n.value.infix.left);
            printf("*");
            print_node(p,n.value.infix.right);
            printf(")");
            break;
        case NK_VEC3:
            printf("vec3(");
            print_node(p,n.value.vec3.x);
            printf(",");
            print_node(p,n.value.vec3.y);
            printf(",");
            print_node(p,n.value.vec3.z);
            printf(")");
            break;
    }
}
// static NodeIndex program_add_node(Program *p, Node n) {
//     array_append_Node(&p->nodes, n);
//     return (NodeIndex)(p->nodes.len - 1);
// }
typedef struct{
    float x;
    float y;
    float z;
}pixel;



float eval_float(Program p,NodeIndex head,float x, float y){
    switch(p.nodes.data[head].kind){
    case NK_VEC3: return 0;
    case NK_X:return x;
    case NK_Y:return y;
    case NK_NUMBER:return p.nodes.data[head].value.number;
    case NK_ADD:return eval_float(p,p.nodes.data[head].value.infix.left,x,y)+eval_float(p,p.nodes.data[head].value.infix.right,x,y);
    case NK_MUL:return eval_float(p,p.nodes.data[head].value.infix.left,x,y)*eval_float(p,p.nodes.data[head].value.infix.right,x,y);
    }
    return 0;
}
pixel eval(Program p,NodeIndex head,float x, float y){
    if (p.nodes.data[head].kind !=  NK_VEC3){
        return (pixel){0};
    }
    return (pixel){
        .x = eval_float(p,p.nodes.data[head].value.vec3.x,x,y),
        .y = eval_float(p,p.nodes.data[head].value.vec3.y,x,y),
        .z = eval_float(p,p.nodes.data[head].value.vec3.z,x,y),
    };
}

void gen_img(unsigned char* data,int w, int h,Program p){
    for(int j=0;j<h;j++){
        for(int i=0;i<w;i++){
            pixel value = eval(p,p.head,((float)i/(float)w-0.5)*2,((float)j/(float)h-0.5)*2);
            data[(i+j*w)*4+0]=(value.x/2+0.5)*255;
            data[(i+j*w)*4+1]=(value.y/2+0.5)*255;
            data[(i+j*w)*4+2]=(value.z/2+0.5)*255;
            data[(i+j*w)*4+3]=255;
        }
    }
}

NodeKind random_node(){
    int r = rand() % 100;
    if (r < 40) return NK_ADD;
    if (r < 70) return NK_MUL;
    if (r < 80) return NK_X;
    if (r < 90) return NK_Y;
    return NK_NUMBER;
}

NodeKind random_node_terminated(){
    int index= rand()%3;
    switch(index){
        case 0: return NK_NUMBER;
        case 1: return NK_X;
        case 2: return NK_Y;
    }
    return NK_NUMBER;
}

NodeIndex random_branch(Program *p,int depth){
    int cell =p->nodes.len;
    p->nodes.len+=1;
    NodeKind nk = random_node();
    if (depth==0){
        nk =random_node_terminated();
    }
    p->nodes.data[cell].kind=nk;
    switch(nk){
        case NK_NUMBER:
            p->nodes.data[cell].value.number=(float)rand() / RAND_MAX-0.5;
            break;
        case NK_ADD:
        case NK_MUL:
            p->nodes.data[cell].value.infix.left=random_branch(p,depth-1);
            p->nodes.data[cell].value.infix.right=random_branch(p,depth-1);
            break;
        default:break;
    }
    return cell;
}

NodeIndex random_program(Program *p,int depth){
    int cell =p->nodes.len;
    p->nodes.len+=1;
    p->nodes.data[cell].kind=NK_VEC3;
    p->nodes.data[cell].value.vec3.x=random_branch(p,depth-1);
    p->nodes.data[cell].value.vec3.y=random_branch(p,depth-1);
    p->nodes.data[cell].value.vec3.z=random_branch(p,depth-1);
    return cell;
}

#define MAX_NODE 1000
int main(void)
{
    Node nodes[MAX_NODE]={0};
    Program p =(Program){0};
    p.nodes.data=nodes;
    p.nodes.capacity=MAX_NODE;

    srand(time(NULL));
    p.head =random_program(&p,10);
    print_node(p,p.head);
    printf("\n");
    const int screenWidth = 800;
    const int screenHeight = 450;
    SetTraceLogLevel(LOG_WARNING);
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
