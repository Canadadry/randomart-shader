#include "../vendor/raylib/raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "dynamicarray.h"
#define _BUFFER_IMPL
#define _SLICE_IMPL
#include "buffer.h"


typedef enum{
    NK_X,
    NK_Y,
    NK_T,
    NK_NUMBER,
    NK_ADD,
    NK_MUL,
    NK_VEC3
}NodeKind;

const char* print_nk(NodeKind nk){
    switch(nk){
        case NK_X:return "NK_X";
        case NK_Y:return "NK_Y";
        case NK_T:return "NK_T";
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

void print_node(Buffer* out,Program p,NodeIndex idx){
    #define MAX_TMP_LEN 32
    char tmp[MAX_TMP_LEN]={0};
    Node n= p.nodes.data[idx];
    int tmp_len=0;
    switch (n.kind) {
        case NK_X: write_string(out,"x");break;
        case NK_Y: write_string(out,"y");break;
        case NK_T: write_string(out,"t");break;
        case NK_NUMBER:
        tmp_len=snprintf(tmp,MAX_TMP_LEN,"%f",n.value.number);
        write_string_len(out,tmp,tmp_len);break;
        case NK_ADD:
            write_string(out,"(");
            print_node(out,p,n.value.infix.left);
            write_string(out,"+");
            print_node(out,p,n.value.infix.right);
            write_string(out,")");
            break;
        case NK_MUL:
            write_string(out,"(");
            print_node(out,p,n.value.infix.left);
            write_string(out,"*");
            print_node(out,p,n.value.infix.right);
            write_string(out,")");
            break;
        case NK_VEC3:
            write_string(out,"vec3(");
            print_node(out,p,n.value.vec3.x);
            write_string(out,",");
            print_node(out,p,n.value.vec3.y);
            write_string(out,",");
            print_node(out,p,n.value.vec3.z);
            write_string(out,")");
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



float eval_float(Program p,NodeIndex head,float x, float y,float t){
    switch(p.nodes.data[head].kind){
    case NK_VEC3: return 0;
    case NK_X:return x;
    case NK_Y:return y;
    case NK_T:return t;
    case NK_NUMBER:return p.nodes.data[head].value.number;
    case NK_ADD:return eval_float(p,p.nodes.data[head].value.infix.left,x,y,t)+eval_float(p,p.nodes.data[head].value.infix.right,x,y,t);
    case NK_MUL:return eval_float(p,p.nodes.data[head].value.infix.left,x,y,t)*eval_float(p,p.nodes.data[head].value.infix.right,x,y,t);
    }
    return 0;
}
pixel eval(Program p,NodeIndex head,float x, float y,float t){
    if (p.nodes.data[head].kind !=  NK_VEC3){
        return (pixel){0};
    }
    return (pixel){
        .x = eval_float(p,p.nodes.data[head].value.vec3.x,x,y,t),
        .y = eval_float(p,p.nodes.data[head].value.vec3.y,x,y,t),
        .z = eval_float(p,p.nodes.data[head].value.vec3.z,x,y,t),
    };
}

void gen_img(unsigned char* data,int w, int h,Program p){
    for(int j=0;j<h;j++){
        for(int i=0;i<w;i++){
            pixel value = eval(p,p.head,((float)i/(float)w-0.5)*2,((float)j/(float)h-0.5)*2,0);
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
    if (r < 87) return NK_Y;
    if (r < 95) return NK_T;
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
#define MAX_AST_LEN 10000

const char *fragmentTemplate =
"#version 330\n"
"in vec2 fragTexCoord;\n"
"out vec4 finalColor;\n"
"uniform vec2 resolution;\n"
"uniform float t;\n"
"void main() {\n"
"    float x = (gl_FragCoord.x / resolution.x) * 2.0 - 1.0;\n"
"    float y = (gl_FragCoord.y / resolution.y) * 2.0 - 1.0;\n"
"\n"
"    vec3 col = %.*s;\n"
"\n"
"    col =(0.5 + 0.5 * (col));\n"
"    finalColor = vec4(col, 1.0);\n"
"}\n";

int main(void)
{
    Node nodes[MAX_NODE]={0};
    Program p =(Program){0};
    p.nodes.data=nodes;
    p.nodes.capacity=MAX_NODE;

    srand(time(NULL));
    p.head =random_program(&p,10);
    Buffer out ={0};
    STATIC_ZERO_INIT(char,out,out_grow,MAX_AST_LEN);
    print_node(&out,p,p.head);
    printf("%.*s\n",out.len,out.data);
    const int screenWidth = 800;
    const int screenHeight = 450;
    SetTraceLogLevel(LOG_WARNING);
    InitWindow(screenWidth, screenHeight, "randomart");

    #ifdef _GPU
    char shaderCode[16384];
    snprintf(shaderCode, sizeof(shaderCode),fragmentTemplate, out.len,out.data);
    Shader shader = LoadShaderFromMemory(0, shaderCode);

    int resLoc = GetShaderLocation(shader, "resolution");
    float res[2] = { (float)screenWidth, (float)screenHeight };
    SetShaderValue(shader, resLoc, res, SHADER_UNIFORM_VEC2);

    SetTargetFPS(60);
    while (!WindowShouldClose())
    {
        float t = fmod(GetTime(),10);
        int resLoc = GetShaderLocation(shader, "t");
        SetShaderValue(shader, resLoc, &t, SHADER_UNIFORM_FLOAT);

        BeginDrawing();
            ClearBackground(YELLOW);
            BeginShaderMode(shader);
            DrawRectangle(0, 0, screenWidth, screenHeight, WHITE);
            EndShaderMode();
        EndDrawing();
    }
    UnloadShader(shader);
    #else
    Image img = (Image){0};
    img.width=screenWidth;
    img.height=screenHeight;
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
            DrawTexture(tex,0, 0, WHITE);
            EndShaderMode();
        EndDrawing();
    }
    UnloadTexture(tex);
    #endif
    CloseWindow();

    return 0;
}
