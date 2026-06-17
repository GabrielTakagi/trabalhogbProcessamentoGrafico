#include "Janela.hpp"

bool Janela::janelaInit()
{
    // Inicializa o GLFW para criar a janela e o contexto OpenGL.
    if (!glfwInit()) {
        std::cerr << "Falha ao inicializar o GLFW" << std::endl;
        return -1;
    }

    // Cria a janela com largura, altura e título definidos em configTela.hpp.
    janela = glfwCreateWindow(ALTURA_JANELA, LARGURA_JANELA, NOME_JANELA, NULL, NULL);
    if (!janela) {
        glfwTerminate();
        return -1;
    }

    // Define o contexto atual para a janela criada.
    glfwMakeContextCurrent(janela);

    // Inicializa o GLEW após criar o contexto OpenGL.
    glewExperimental = GL_TRUE; // Habilita extensões modernas do OpenGL.
    if (glewInit() != GLEW_OK) {
        std::cerr << "Falha ao inicializar o GLEW" << std::endl;
        return -1;
    }

    // Sincroniza a troca de buffers com a taxa de atualização do monitor.
    glfwSwapInterval(1);

    return 0;
}

GLFWwindow* Janela::getJanela()
{
    // Retorna o ponteiro para a janela usada em todo o aplicativo.
    return janela;
}

void Janela::limpar()
{
    // Limpa apenas o buffer de cor antes de desenhar o próximo frame.
    glClear(GL_COLOR_BUFFER_BIT);
}

void Janela::fechar()
{
    // Finaliza o GLFW e libera recursos.
    glfwTerminate();
}

void Janela::trocarBuffer()
{
    // Envia o conteúdo desenhado para a tela.
    glfwSwapBuffers(janela);
}
