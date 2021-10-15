#version 330

in vec3 worldVertexPosition;
in vec3 worldVertexNormal;
in vec3 vertexColorToFS;

uniform vec3 cameraPosition;
uniform vec3 ambientLight;
uniform vec3 materialA;
uniform vec3 materialD;
uniform vec3 materialS;

const int N = 1;

struct Light {
   vec3  lightColor;
   float subcutoff;
   vec3  lightPosition;
   float exponent;
   vec3  lightDirection;
   float cutoff;
};

uniform LightBlock {
   Light lights[N];
};

out vec4 pixelColor;

void main() {
   vec3 n = normalize(worldVertexNormal);
   vec3 v = normalize(cameraPosition - worldVertexPosition);
   int i;
   vec3 l, r, d;
   float factorD, factorS, factorDeg;
   vec3 tempPixelColor = ambientLight * materialA * vertexColorToFS;
   for(i = 0; i < N; i ++) {
      l = normalize(lights[i].lightPosition - worldVertexPosition);
      d = normalize(lights[i].lightDirection);
      
      // Implementación de luz direccional: sólo pasa lo que está dentro del espectro cónico
      float cosAlpha = clamp(dot(d, - l ), 0, 1);
      if(cosAlpha < lights[i].cutoff) continue;
      
      // Degradación de la mancha de luz
      if(cosAlpha >= lights[i].subcutoff) factorDeg = 1.0;
      else factorDeg = (cosAlpha - lights[i].cutoff) / (lights[i].subcutoff - lights[i].cutoff);
      
      // Atenuación de la luz
      float distance  = length(worldVertexPosition - lights[i].lightPosition);
      float factorAtt = 0.4;
      
      factorD = clamp(dot(n, l), 0, 1);
      r = normalize((2 * n) * dot(n, l) - l);
      
      //factorS = clamp(pow(dot(r, v), lights[i].exponent), 0, 1);      
      factorS = pow(clamp(dot(r, v), 0, 1), lights[i].exponent);      
      
      tempPixelColor += factorDeg * factorAtt * lights[i].lightColor * (materialD * factorD + materialS * factorS);
   }
   pixelColor = vec4(clamp(tempPixelColor, 0, 1), 1);
   //if(lights[0].lightColor[0] == 0 && lights[0].lightColor[1] == 0 && lights[0].lightColor[2] == 0) { pixelColor = vec4(0, 0, 0, 1); } 
   //else{  } 
   
}

