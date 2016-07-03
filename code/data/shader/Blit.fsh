#version 330

uniform sampler2D uSamplerColor;

in vec2 vTexCoord;

out vec4 oColor;

float rand(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

float A = 0.15;
float B = 0.50;
float C = 0.10;
float D = 0.20;
float E = 0.02;
float F = 0.30;
float W = 11.2;

vec3 Uncharted2Tonemap(vec3 x)
{
   return ((x*(A*x+C*B)+D*E)/(x*(A*x+B)+D*F))-E/F;
}

//RADIUS of our vignette, where 0.5 results in a circle fitting the screen
const float RADIUS = 0.75;

//softness of our vignette, between 0.0 and 1.0
const float SOFTNESS = 0.35;


vec3 Vignette(vec3 color) {
    //determine center position
    vec2 position = vTexCoord - vec2(0.5);

    //determine the vector length of the center position
    float len = length(position);

    //use smoothstep to create a smooth vignette
    float vignette = smoothstep(RADIUS, RADIUS-SOFTNESS, len);

    //apply the vignette with 50% opacity
    return mix(color, color * vignette, 0.3);
}


void main()
{
  vec3 color = texture(uSamplerColor, vTexCoord).rgb;

  color *= 1;  // Hardcoded Exposure Adjustment

  float ExposureBias = 2.0f;
  vec3 curr = Uncharted2Tonemap(ExposureBias*color);

  vec3 whiteScale = vec3(1.0f)/Uncharted2Tonemap(vec3(W));
  color = curr*whiteScale;

  color = Vignette(color);

  //color = pow(color, vec3(2.2));
  color += vec3(rand(gl_FragCoord.xy) * 1.0/255 - 0.5/255);
        
  oColor = vec4(color, 1);
}
