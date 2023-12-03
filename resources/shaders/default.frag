#version 330 core

const int POINT = 0;
const int DIRECTIONAL = 1;
const int SPOT = 2;
const int MAX_NUM_LIGHTS = 8;

in vec3 outPosition;
in vec3 outNormal;

// lights
uniform float ka;
uniform float kd;
uniform float ks;
struct Light
{
    vec4 color;
    vec4 pos; // Position with CTM applied (Not applicable to directional lights)
    vec4 dir; // Direction with CTM applied (Not applicable to point lights)
    vec3 function;
    int type;
    float penumbra; // Only applicable to spot lights, in RADIANS
    float angle;    // Only applicable to spot lights, in RADIANS
};
uniform int num_lights;

uniform Light lights_array[MAX_NUM_LIGHTS];

// material
uniform vec4 cAmbient;
uniform vec4 cDiffuse;
uniform vec4 cSpecular;
uniform float shininess;

// camera
uniform vec4 campos;

out vec4 color;

void main() {
    // Remember that you need to renormalize vectors here if you want them to be normalized

    color = vec4(0,0,0,1);

    vec3 ambientColor = ka*vec3(cAmbient);
    color.rgb += ambientColor;

    vec3 n_outNormal = normalize(outNormal);
    for (int i=0; i<num_lights; i++){

        switch (lights_array[i].type) {

        case (POINT):{
            // diffuse
            vec3 n_lightdir = normalize(vec3(lights_array[i].pos-vec4(outPosition,1)));
            float diffuseIntensity = clamp(dot(n_outNormal, n_lightdir), 0.0, 1.0);
            vec3 diffuseColor = kd * diffuseIntensity * vec3(cDiffuse);

            // specular
            vec3 specularColor = vec3(0,0,0);
            vec3 viewDir = normalize(campos.xyz-outPosition);
            vec3 reflectDir = normalize(reflect(-n_lightdir, n_outNormal));
            float dotProduct = clamp(dot(viewDir, reflectDir), 0.0,1.0);
            if (dotProduct > 0 && shininess > 0){
                float specularIntensity = pow(dotProduct, shininess);
                specularColor = ks * specularIntensity * vec3(cSpecular);
            }

            float dist = length(vec3(lights_array[i].pos)-outPosition);
            float c1 = lights_array[i].function.x;
            float c2 = lights_array[i].function.y;
            float c3 = lights_array[i].function.z;
            float atten = min(1.0f,1/(c1+dist*c2+dist*dist*c3));

            color.rgb += atten*vec3(lights_array[i].color)*(diffuseColor+specularColor);

            break;
        }

        case (DIRECTIONAL):{
            // diffuse
            vec3 n_lightdir = -normalize(vec3(lights_array[i].dir));
            float diffuseIntensity = clamp(dot(n_outNormal, n_lightdir), 0.0, 1.0);
            vec3 diffuseColor = kd * diffuseIntensity * vec3(cDiffuse);

            // specular
            vec3 specularColor = vec3(0,0,0);
            vec3 viewDir = normalize(campos.xyz-outPosition);
            vec3 reflectDir = normalize(reflect(-n_lightdir, n_outNormal));
            float dotProduct = clamp(dot(viewDir, reflectDir), 0.0,1.0);
            if (dotProduct > 0 && shininess > 0){
                float specularIntensity = pow(dotProduct, shininess);
                specularColor = ks * specularIntensity * vec3(cSpecular);
                //vec3 specularColor = vec3(1) * vec3(1) * vec3(1);
            }

            color.rgb += vec3(lights_array[i].color) * (diffuseColor+specularColor);
            break;
        }

        case (SPOT):{
            float dist = length(vec3(lights_array[i].pos)-outPosition);
            float c1 = lights_array[i].function.x;
            float c2 = lights_array[i].function.y;
            float c3 = lights_array[i].function.z;
            float atten = min(1.0f,1/(c1+dist*c2+dist*dist*c3));

            vec3 rayDir = normalize(vec3(lights_array[i].pos)-outPosition);
            vec3 lightDir = normalize(vec3(lights_array[i].dir));

            float angle = abs(acos(dot(lightDir,-rayDir)));
            if (angle < lights_array[i].angle){
                // diffuse
                float diffuseIntensity = clamp(dot(n_outNormal, rayDir), 0.0, 1.0);
                vec3 diffuseColor = kd * diffuseIntensity * vec3(cDiffuse);

                // specular
                vec3 specularColor = vec3(0,0,0);
                vec3 viewDir = normalize(campos.xyz-outPosition);
                vec3 reflectDir = normalize(reflect(-rayDir, n_outNormal));
                float dotProduct = clamp(dot(viewDir, reflectDir), 0.0,1.0);
                if (dotProduct > 0 && shininess > 0){
                    float specularIntensity = pow(dotProduct, shininess);
                    specularColor = ks * specularIntensity * vec3(cSpecular);
                }

                float falloff = 0;
                float inner = lights_array[i].angle - lights_array[i].penumbra;
                if (angle > inner){
                    falloff = -2 * pow((angle-inner)/lights_array[i].penumbra,3) + 3*pow((angle-inner)/lights_array[i].penumbra,2);
                }
                color.rgb += atten*(1-falloff) * vec3(lights_array[i].color) * (diffuseColor+specularColor);
            }
            break;
        }
        }
    }

    color.a = 1.0;
}
