#include "Shader.h"

Shader::Shader()
{
    // Construtor simples, nenhum inicializador necessário.
}

bool Shader::carregar_vshader()
{
    vs = glCreateShader(GL_VERTEX_SHADER);
    const GLchar *p = (const GLchar *)vertex_shader;
    glShaderSource(vs, 1, &p, NULL);
    glCompileShader(vs);

    // Verifica se o vertex shader compilou corretamente.
    int params = -1;
    glGetShaderiv(vs, GL_COMPILE_STATUS, &params);
    if (GL_TRUE != params)
    {
        fprintf(stderr, "ERROR: GL shader index %i did not compile\n", vs);
        return true;
    }

    return false;
}

bool Shader::carregar_fshader()
{
    fs = glCreateShader(GL_FRAGMENT_SHADER);
    const GLchar *p = (const GLchar *)fragment_shader;
    glShaderSource(fs, 1, &p, NULL);
    glCompileShader(fs);

    // Verifica se o fragment shader compilou corretamente.
    int params = -1;
    glGetShaderiv(fs, GL_COMPILE_STATUS, &params);
    if (GL_TRUE != params)
    {
        fprintf(stderr, "ERROR: GL shader index %i did not compile\n", fs);
        return true;
    }

    return false;
}

bool Shader::carregar_program()
{
    // Cria o programa e anexa os shaders compilados.
    shader_programme = glCreateProgram();
    glAttachShader(shader_programme, vs);
    glAttachShader(shader_programme, fs);
    glLinkProgram(shader_programme);

    // Verifica se o link do programa foi bem-sucedido.
    int params = -1;
    glGetProgramiv(shader_programme, GL_LINK_STATUS, &params);
    if (GL_TRUE != params)
    {
        int max_length = 2048;
        int actual_length = 0;
        char log[2048];
        glGetProgramInfoLog(shader_programme, max_length, &actual_length, log);
        fprintf(stderr, "ERRO NO LINK DO PROGRAMA (Index %i):\n%s\n", shader_programme, log);
        return true;
    }

    return false;
}

GLint Shader::get_program(){
    // Retorna o identificador do programa de shader para uso em glUseProgram.
    return shader_programme;
}

bool Shader::carregar_arquivo(const char* vs, const char* fs)
{
    // Lê os arquivos de shader para os buffers internos.
    parse_file_into_str(vs, vertex_shader, 1024 * 256);
    parse_file_into_str(fs, fragment_shader, 1024 * 256);

    // Compila e linka o shader.
    carregar_vshader();
    carregar_fshader();
    carregar_program();

    return true;
}

bool Shader::parse_file_into_str(
    const char* file_name, char* shader_str, int max_len
) {
    shader_str[0] = '\0'; // Inicializa o buffer vazio.
    FILE* file = fopen(file_name, "r");
    if (!file) {
        std::cerr << "ERRO: não foi possível abrir o arquivo: " << file_name << std::endl;
        return false;
    }

    int current_len = 0;
    char line[2048];
    strcpy(line, "");

    // Lê o arquivo linha por linha e concatena no buffer.
    while (!feof(file)) {
        if (NULL != fgets(line, 2048, file)) {
            current_len += strlen(line);
            if (current_len >= max_len) {
                // Aqui poderíamos tratar overflow, mas assumimos que o arquivo cabe no buffer.
            }
            strcat(shader_str, line);
        }
    }

    if (EOF == fclose(file)) {
        std::cout << "Erro";
        return false;
    }
    return true;
}
