#version 330 core
layout(location = 0) in vec2 position;
layout(location = 1) in vec2 texCoord; // NOVO: Coordenada da textura

uniform mat4 u_Model;
uniform mat4 u_Projecao;

uniform vec2 u_TexOffset; 
uniform vec2 u_TexScale;  

out vec2 v_TexCoord;

void main() {
    gl_Position = u_Projecao * u_Model * vec4(position, 0.0, 1.0);
    v_TexCoord = texCoord * u_TexScale + u_TexOffset; 
}