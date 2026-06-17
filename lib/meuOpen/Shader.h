#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <iostream>
#include <cstring>

// Classe auxiliar para carregar, compilar e linkar shaders.
class Shader
{
public:
    Shader();

    // Retorna o identificador do programa de shader.
    GLint get_program();

    // Carrega os arquivos de shader do disco, compila e linka o programa.
    bool carregar_arquivo(const char* vs, const char* fs);

    // Compila o vertex shader.
    bool carregar_vshader();

    // Compila o fragment shader.
    bool carregar_fshader();

    // Linka os shaders em um único programa executável.
    bool carregar_program();

private:
    char vertex_shader[1024 * 256];    // Buffer para o código do vertex shader.
    char fragment_shader[1024 * 256];  // Buffer para o código do fragment shader.
    GLuint vs, fs, shader_programme, GLtransform;

    // Lê o conteúdo do arquivo de shader para o buffer.
    bool parse_file_into_str(const char* file_name, char* shader_str, int max_len);
};