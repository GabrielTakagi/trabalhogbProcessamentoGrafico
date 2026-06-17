#version 330 core
in vec2 v_TexCoord;
out vec4 color;

uniform sampler2D u_Texture;  
uniform vec3 u_CorDoBloco;    
uniform bool u_UsarTextura;   

void main() {
    if (u_UsarTextura) {
        vec4 texColor = texture(u_Texture, v_TexCoord);
        if(texColor.a < 0.1) discard; 
        color = texColor;
    } else {
        color = vec4(u_CorDoBloco, 1.0);
    }
}