#shader vertex
#version 400 core

layout(location = 0) in vec3 position;
layout(location = 1)in vec3 normal;
layout(location = 2) in vec2 texCoord;

uniform mat4 u_MVP;
uniform mat4 u_model;

out vec2 v_TexCoord;
out vec3 v_normal;
out vec3 v_fPos;

void main()
{
    gl_Position = u_MVP * vec4(position, 1.0);

    v_fPos = vec3(u_model * vec4(position, 1.0));
    v_TexCoord = texCoord;

    //Recalculating the normal but without translation effect
    mat4 model = u_model;
    model[3][0] = 0;
    model[3][1]= 0;
    model[3][2] = 0;
    model[3][3] = 1;
    v_normal = normalize(vec3(model * vec4(normal, 1.0)));
}


//-----------------------------------------------
#shader fragment
#version 400 core

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct Light {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct DirectionalLight {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

struct SpotLight {
    vec3 position;
    vec3  direction;
    float cutOff;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

uniform vec4 u_Color;
uniform vec3 u_viewPos;

uniform sampler2D u_Textures[2];
uniform int u_texSlot;
uniform int u_textureMap;
uniform vec3 u_color;

uniform Material material;
uniform Light light;
uniform DirectionalLight dlight;
uniform PointLight plight;
uniform SpotLight slight;
uniform int u_useLighting;
uniform int u_lightType;

uniform float u_velocity;
uniform float u_wavelength;

in vec2 v_TexCoord;
in float v_texSlot;
in vec3 v_normal;
in vec3 v_fPos;

out vec4 color;

float powerOfExp(float lambda, float a, float b){
    return (lambda-(b+a)/2)/((b-a)/2);
}

float getColor(float lambda, float a, float b){
    return exp(-powerOfExp(lambda, a, b)*powerOfExp(lambda, a, b));
}

float shiftedLambda(float lambda){
    return lambda*pow((1-u_velocity)/(1+u_velocity), 0.5);
}

float wavelength_shift(float lambda){
    return shiftedLambda(lambda)-lambda;
}

float max(float r, float g, float b)
{
	float max_val = r;
	if(max_val < g){
		max_val = g;
	}
	if (max_val < b){
		max_val = b;
	}
	return max_val;
}

float min(float r, float g, float b)
{
	float min_val = r;
	if(min_val > g){
		min_val = g;
	}
	if (min_val > b){
		min_val = b;
	}
	return min_val;
}

float RGB_to_lambda(float r, float g, float b)
{
	float max_val, min_val, h, s, v, wavelength;
    max_val = max(r, g, b);
    min_val = min(r, g, b);
    if(max_val == 0){
        s = 0;
        h = 0;
    }

    else if((max_val - min_val) == 0.0 ){
        s = 0;
        h = 0;
    }
    else{
        s = (max_val - min_val) / max_val;
        if (max_val == r){
            h = 60 * ((g - b) / (max_val - min_val)) + 0;
        }
        else if (max_val == g ){
            h = 60 * ((b - r) / (max_val - min_val)) + 120;
        }
        else{
            h = 60 * ((r - g) / (max_val - min_val)) + 240;
        }
    }

    if(h < 0){
        h = h + 360.0;
    }
    s = s * 255.0;
    v = v * 255.0;
    wavelength = 620.0 - (250.0 / 270.0 )* h;
    return wavelength;
}


void main()
{
    //Use Color instead of texture if invalid slot is specified
    if(u_texSlot == -1)
    {
        //Simply use color if no lighting is to be used
        if(u_useLighting == 0)
        {
            color = vec4(u_color, 1.0);
        }
        else
        {
            vec3 ambient = light.ambient * material.ambient;

            vec3 norm = normalize(v_normal);
            vec3 lightDirection = normalize(light.position - v_fPos);
            float diff = max(dot(norm, lightDirection), 0.0);
            vec3 diffuse = light.diffuse * diff * material.diffuse;

            vec3 viewDir = normalize(u_viewPos - v_fPos);
            vec3 reflectDir = reflect(-lightDirection, norm);
            float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
            vec3 specular = light.specular * (spec * material.specular);

            color = vec4((ambient + diffuse + specular)* u_color, 1.0);
        }
    }

    //Use texture of specified slot
    else
    {
        int index = u_texSlot;
        vec4 texColor = texture(u_Textures[index], v_TexCoord);

        if(texColor.a< 0.5)
            discard;

        //Simply use texture color if no lighting is to be used
        if(u_useLighting == 0)
        {
            color = texColor;
        }

        //If lighting is to be used
        else
        {
            //If texture mapping is not used
            if(u_textureMap == 0)
            {
                //Type 0 is point light without attenuation
                if(u_lightType == 0)
                {
                    vec3 ambient = light.ambient * material.ambient;

                    vec3 norm = normalize(v_normal);
                    vec3 lightDirection = normalize(light.position - v_fPos);
                    float diff = max(dot(norm, lightDirection), 0.0);
                    vec3 diffuse = light.diffuse * diff * material.diffuse;

                    vec3 viewDir = normalize(u_viewPos - v_fPos);
                    vec3 reflectDir = reflect(-lightDirection, norm);
                    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
                    vec3 specular = light.specular * (spec * material.specular);

                    color = vec4((ambient + diffuse + specular)* vec3(texColor), 1.0);
                }
                //Type 1 is directional light
                else if(u_lightType == 1)
                {
                    vec3 ambient = dlight.ambient * material.ambient;

                    vec3 norm = normalize(v_normal);
                    vec3 lightDirection = normalize(-dlight.direction);
                    float diff = max(dot(norm, lightDirection), 0.0);
                    vec3 diffuse = dlight.diffuse * diff * material.diffuse;

                    vec3 viewDir = normalize(u_viewPos - v_fPos);
                    vec3 reflectDir = reflect(-lightDirection, norm);
                    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
                    vec3 specular = dlight.specular * (spec * material.specular);

                    color = vec4((ambient + diffuse + specular)* vec3(texColor), 1.0);
                }
                //Type 2 is point light
                else if(u_lightType == 2)
                {
                    float distance    = length(plight.position - v_fPos);
                    float attenuation = 1.0 / (plight.constant + plight.linear * distance +
                                    plight.quadratic * (distance * distance));
                    vec3 ambient = plight.ambient * material.ambient;

                    vec3 norm = normalize(v_normal);
                    vec3 lightDirection = normalize(plight.position - v_fPos);
                    float diff = max(dot(norm, lightDirection), 0.0);
                    vec3 diffuse = plight.diffuse * diff * material.diffuse;

                    vec3 viewDir = normalize(u_viewPos - v_fPos);
                    vec3 reflectDir = reflect(-lightDirection, norm);
                    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
                    vec3 specular = plight.specular * (spec * material.specular);

                    color = vec4((attenuation*(ambient + diffuse + specular))* vec3(texColor), 1.0);
                }

            }

            //If texture is to be mapped
            else
            {
                vec3 ambient = light.ambient * vec3(texColor);

                vec3 norm = normalize(v_normal);
                vec3 lightDirection = normalize(light.position - v_fPos);
                float diff = max(dot(norm, lightDirection), 0.0);
                vec3 diffuse = light.diffuse * diff * vec3(texColor);

                vec3 viewDir = normalize(u_viewPos - v_fPos);
                vec3 reflectDir = reflect(-lightDirection, norm);
                float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
                vec3 specular = light.specular * (spec * vec3(texColor));

                color = vec4(ambient + diffuse + specular, 1.0);
            }
        }
    }
    //Color value is calculated for all
    //Doppler shift here
    if(u_velocity>=0.0){
        float wavelength = shiftedLambda(u_wavelength);
        vec4 wavelengthColor = vec4(getColor(wavelength, 520.0, 630.0), getColor(wavelength, 500.0, 590.0), getColor(wavelength, 410.0, 480.0), 1.0);

        wavelength = RGB_to_lambda(color.r, color.g, color.b);
        wavelength = shiftedLambda(wavelength);
        vec4 shiftedColor = vec4(getColor(wavelength, 520.0, 630.0), getColor(wavelength, 500.0, 590.0), getColor(wavelength, 410.0, 480.0), 1.0);

        float gamma = 1.2-7.2/(6+30.0*u_velocity);
        shiftedColor = (wavelengthColor+shiftedColor)/2.0;
        color = (1-gamma)*color+gamma*shiftedColor;
    }
    else{
        float wavelength = shiftedLambda(u_wavelength);
        vec4 wavelengthColor = vec4(getColor(wavelength, 520.0, 630.0), getColor(wavelength, 500.0, 590.0), getColor(wavelength, 410.0, 480.0), 1.0);

        wavelength = RGB_to_lambda(color.r, color.g, color.b);
        wavelength = shiftedLambda(wavelength);
        vec4 shiftedColor = vec4(getColor(wavelength, 520.0, 630.0), getColor(wavelength, 500.0, 590.0), getColor(wavelength, 410.0, 480.0), 1.0);

        float gamma = 1.2-7.2/(6-30.0*u_velocity);
        shiftedColor = (wavelengthColor+shiftedColor)/2.0;
        color = (1-gamma)*color+gamma*shiftedColor;
    }
}
