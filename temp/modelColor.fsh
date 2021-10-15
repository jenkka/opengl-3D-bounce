#version 400

uniform vec3 modelColor; 
out vec4 pixelColor; 

void main() { 
  pixelColor = vec4(modelColor, 1.0); 
} 
