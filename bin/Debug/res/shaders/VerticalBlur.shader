#shader vertex
#version 400 core

layout(location = 0) in vec2 position;

out vec2 v_TexCoord;

void main()
{
    gl_Position = vec4(position, -1.0, 1.0);
    v_TexCoord = position.xy/2.0+0.5;
}

//-----------------------------------------------

#shader fragment
#version 400 core

in vec2 v_TexCoord;
uniform sampler2D u_slot;
uniform int u_width;
uniform int u_height;

vec2 getNewUv(int dy_pixel, vec2 uv)
{
    uv.y += float(dy_pixel)/float(u_height);
    if(uv.y>1){
        uv.y = 1;
    }
    else if(uv.y<0){
        uv.y = 0;
    }
    return uv.xy;
}
out vec4 color;
float weights[5] =  float[5](0.06136, 0.24477, 0.38774, 0.24477, 0.06136);
void main()
{
    //Handle color here
    color = vec4(0.0);
    for(int i=-2; i<=2; i++)
    {
        vec2 uv = getNewUv(i, v_TexCoord);
        color += weights[i+2]*texture(u_slot, uv);
    }
}
