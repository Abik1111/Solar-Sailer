#shader vertex
#version 400 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCord;

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;
uniform vec3 u_viewPos;

out vec4 v_clipSpace;
out vec2 v_texCord;
out vec3 v_viewDir;

void main()
{
    vec4 worldPos = u_model * vec4(position, 1.0);
    gl_Position = u_projection * u_view * worldPos;
    v_clipSpace = gl_Position;
    v_texCord = texCord;
    v_viewDir = u_viewPos - worldPos.xyz;
    v_viewDir = normalize(v_viewDir);
}


//-----------------------------------------------
#shader fragment
#version 400 core

uniform sampler2D u_reflectionSampler;
uniform sampler2D u_refractionSampler;
uniform sampler2D u_dudvSampler;
uniform sampler2D u_normalSampler;
uniform sampler2D u_depthSampler;
uniform vec3 u_lightColor;
uniform vec3 u_lightDirection;
uniform float u_moveFactor;
uniform float near;
uniform float far;


in vec4 v_clipSpace;
in vec2 v_texCord;
in vec3 v_viewDir;

out vec4 color;
const float waveStrength = 0.01;
const float shineDamper = 20.0;
const float reflectivity = 5.0;

void main()
{
    vec2 normalizedDeviceSpace = (v_clipSpace.xy/v_clipSpace.w)/2.0 + 0.5;
    vec2 refractionCoords = vec2(normalizedDeviceSpace.x, normalizedDeviceSpace.y);
    vec2 reflectionCoords = vec2(normalizedDeviceSpace.x, -normalizedDeviceSpace.y);

    float depth = texture(u_depthSampler, refractionCoords).r;
    float floorDistance = 2*near*far/(near+far-(2*depth-1.0)*(far-near));
    depth = gl_FragCoord.z;
    float waterDistance = 2*near*far/(near+far-(2*depth-1.0)*(far-near));
    float waterDepth = floorDistance-waterDistance;
    float alpha = clamp(waterDepth/2.0, 0.0, 1.0);

    float x_cord = v_texCord.x+u_moveFactor;
    float y_cord = v_texCord.y+u_moveFactor;
    if(x_cord > 1)
        x_cord = 2-x_cord;
    if(y_cord > 1)
        y_cord = 2-y_cord;
    vec2 distortionTexCord = texture(u_dudvSampler, vec2(x_cord, y_cord)).rg;
    distortionTexCord += v_texCord;
    vec2 distortion = (texture(u_dudvSampler, distortionTexCord).rg*2.0-1.0)*waveStrength * alpha;

    refractionCoords += distortion;
    reflectionCoords += distortion;

    refractionCoords = clamp(refractionCoords, 0.01, 0.99);
    reflectionCoords.x = clamp(reflectionCoords.x , 0.01, 0.99);
    reflectionCoords.y = clamp(reflectionCoords.y , -0.99, -0.01);

    vec4 reflectionColor = texture(u_reflectionSampler, reflectionCoords);
    vec4 refractionColor = texture(u_refractionSampler, refractionCoords);

    vec4 normalColor = texture(u_normalSampler, distortionTexCord);
    vec3 normal = vec3(normalColor.r*2-1, normalColor.b*2, normalColor.g*2-1);
    normal = normalize(normal);

    float dotProduct = dot(v_viewDir, normal);
    dotProduct = pow(dotProduct, 0.5);

    vec3 reflectDir = reflect(u_lightDirection, normal);
    float specular = max(dot(reflectDir, v_viewDir), 0.0);
    specular = pow(specular, shineDamper);
    vec3 specularHighlight = u_lightColor * specular * reflectivity;

    color = mix(reflectionColor, refractionColor, dotProduct);
    color = mix(color, vec4(0.0, 0.3, 0.5, 1.0), 0.1) + vec4(specularHighlight, 0.0);
    color.a = alpha;
}
