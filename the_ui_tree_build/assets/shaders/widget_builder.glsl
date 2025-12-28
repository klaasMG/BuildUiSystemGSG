struct Widget{
    ivec3 pos_one;
    ivec3 pos_two;
    int shader_pass;
    ivec4 colour;
    int shape;
    int parent;
};

float pos_to_ndc(int size_any,int postion_any){
    float vertex_position = (float(postion_any) / float(size_any)) * 2.0 - 1.0;
    return vertex_position;
}

float pos_to_ndc(int size_any,float postion_any){
    float vertex_position = (float(postion_any) / size_any) * 2.0 - 1.0;
    return vertex_position;
}

float col_to_ndc(int col_any){
    float colour_ndc = float(col_any) / 255.0;
    return colour_ndc;
}

float center_pos(int coord_one, int coord_two){
    int double_coord = coord_one + coord_two;
    float average_coord = double_coord / 2.0f;
    return average_coord;
}