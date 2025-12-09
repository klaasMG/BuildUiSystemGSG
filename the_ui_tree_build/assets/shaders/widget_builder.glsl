struct Widget{
    ivec3 pos_one;
    ivec3 pos_two;
    int shader_pass;
    ivec4 colour;
    int shape;
    int parent;
};

float pos_to_ndc(int size_any,int postion_any){
    float vertex_position = (postion_any / size_any) * 2.0 - 1.0;
    return vertex_position;
}