#shader vertex
#version 400 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoord;
layout(location = 3) in vec2 maptexCoord;

uniform mat4 u_view;
uniform mat4 u_projection;
uniform mat4 u_lightSpace;
uniform vec4 u_clipPlane;

out vec2 v_TexCoord;
out vec2 v_MapTexCoord;
out vec3 v_normal;
out vec3 v_fPos;
out vec4 v_lightPos;
out vec4 v_normalPos;
out float v_visibility;

const float density = 0.015;
const float gradient = 3.0;

void main()
{
    vec4 worldPos = vec4(position, 1.0);
    vec4 positionFromCamera = u_view * worldPos;
    gl_Position = u_projection * positionFromCamera;
    v_normalPos = gl_Position;
    v_lightPos = u_lightSpace * worldPos;

    v_TexCoord = texCoord;
    v_MapTexCoord = maptexCoord;
    v_normal = normal;
    v_fPos = position;

    float distance = length(positionFromCamera.xyz);
    v_visibility = exp(-pow(distance*density,gradient));
    v_visibility = clamp(v_visibility, 0.0, 1.0);
    gl_ClipDistance[0] = dot(worldPos, u_clipPlane);
}


//-----------------------------------------------
#shader fragment
#version 400 core

struct DirectionalLight {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct Material{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

uniform sampler2D u_Textures[2];
uniform int u_texSlot;
uniform int u_textureMap;
uniform vec3 u_color;
uniform vec3 u_viewPos;

uniform sampler2D backSlot;
uniform sampler2D rSlot;
uniform sampler2D gSlot;
uniform sampler2D bSlot;
uniform sampler2D mapSlot;
uniform sampler2D depthSlot;

uniform Material material;
uniform DirectionalLight dlight;
uniform vec3 u_skyColor;

in vec2 v_TexCoord;
in vec2 v_MapTexCoord;
in vec3 v_normal;
in vec3 v_fPos;
in float v_visibility;
in vec4 v_lightPos;
in vec4 v_normalPos;

out vec4 color;
const float bias = 0.005;

void main()
{
    vec4 blendMapColor = texture(mapSlot, v_MapTexCoord);
    float backTextureAmount = max(1 - (blendMapColor.r+ blendMapColor.g+ blendMapColor.b), 0.0);

    //Using texture mapping of different slots
    vec4 backTexture = texture(backSlot, v_TexCoord) * backTextureAmount;
    vec4 rTexture = texture(rSlot, v_TexCoord) * blendMapColor.r;
    vec4 gTexture = texture(gSlot, v_TexCoord) * blendMapColor.g;
    vec4 bTexture = texture(bSlot, v_TexCoord) * blendMapColor.b;
    vec4 texColor = backTexture+ rTexture+ gTexture+ bTexture;

    vec3 ambient = dlight.ambient * material.ambient;

    vec3 norm = normalize(v_normal);
    vec3 lightDirection = normalize(-dlight.direction);
    float diff = max(dot(norm, lightDirection), 0.0);
    vec3 diffuse = dlight.diffuse * diff * material.diffuse;

    vec3 viewDir = normalize(u_viewPos - v_fPos);
    vec3 reflectDir = reflect(-lightDirection, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = dlight.specular * (spec * material.specular);

    vec2 normalizedLightSpace = (v_lightPos.xy/v_lightPos.w)/2.0 + 0.5;
    float visibility = 1;
    vec2 poissonDisk[4] = vec2[](
                        vec2( -0.94201624, -0.39906216 ),
                        vec2( 0.94558609, -0.76890725 ),
                        vec2( -0.094184101, -0.92938870 ),
                        vec2( 0.34495938, 0.29387760 ));
    if(normalizedLightSpace.x<=1 && normalizedLightSpace.x>=0 && normalizedLightSpace.y<=1 && normalizedLightSpace.y>=0)
    {
        for(int i=0;i<4;i++){
          vec4 depthValue = texture(depthSlot, normalizedLightSpace+poissonDisk[i]/700.0);
          float depth = depthValue.r*2-1;
          if (depth < (v_lightPos.z/v_lightPos.w - bias))
            visibility-=0.2;
        }
    }
    color = vec4((ambient + diffuse + specular)* vec3(texColor), 1.0);
    color = mix(vec4(u_skyColor, 1.0), color, v_visibility);
    color = mix(vec4(vec3(0.0), 1.0), color, visibility);
    //color = depthValue;
}
