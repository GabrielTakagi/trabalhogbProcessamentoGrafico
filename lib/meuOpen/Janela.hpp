#pragma once

#include "configTela.hpp"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <iostream>

// Classe responsável por criar e gerenciar a janela do OpenGL.
class Janela
{
public:
    // Inicializa o GLFW, cria a janela e inicia o GLEW.
    bool janelaInit();

    // Retorna o ponteiro para a janela GLFW.
    GLFWwindow* getJanela();

    // Limpa o buffer de cor antes de desenhar cada frame.
    void limpar();

    // Finaliza o GLFW e fecha a janela.
    void fechar();

    // Troca o buffer de desenho para mostrar o conteúdo na tela.
    void trocarBuffer();

private:
    GLFWwindow *janela; // Ponteiro para a janela criada pelo GLFW.
};