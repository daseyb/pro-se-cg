uniform sampler2D uSamplerSSAO;

uniform int uBlurSize = 4; // use size of noise texture

in vec2 vTexCoord; // input from vertex shader

out vec4 oResult;

void main() {
   vec2 texelSize = 1.0 / vec2(textureSize(uSamplerSSAO, 0));
   float result = 0.0;
   vec2 hlim = vec2(float(-uBlurSize) * 0.5 + 0.5);
   for (int i = 0; i < uBlurSize; ++i) {
      for (int j = 0; j < uBlurSize; ++j) {
         vec2 offset = (hlim + vec2(float(i), float(j))) * texelSize;
         result += texture(uSamplerSSAO, vTexCoord + offset).r;
      }
   }
 
   oResult.r = result / float(uBlurSize * uBlurSize);
}