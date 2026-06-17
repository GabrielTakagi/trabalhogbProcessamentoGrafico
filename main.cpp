#include "configTela.hpp"
#include "Janela.hpp"
#include "MapaTile.hpp"
#include <iostream>

// Esta função captura as teclas pressionadas pelo jogador
// e transmite os comandos para o mapa (movimento e alteração de tiles)
void processarEntrada(GLFWwindow* window, MapaTile& mapa) {
    static double ultimoTempo = 0;
    double tempoAtual = glfwGetTime();
    // Implementa um cooldown para evitar múltiplos inputs por frame
    if (tempoAtual - ultimoTempo < 0.18) return; // Cooldown do teclado

    int moveI = 0;  // Variação na linha (Y do mapa em coordenadas de grade)
    int moveC = 0;  // Variação na coluna (X do mapa em coordenadas de grade)
    bool pressionouBotao = false;

    // Processa as setas direcionais do teclado
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) { moveI += 1; moveC += 1; pressionouBotao = true; }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) { moveI -= 1; moveC -= 1; pressionouBotao = true; }
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) { moveI += 1; moveC -= 1; pressionouBotao = true; }
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) { moveI -= 1; moveC += 1; pressionouBotao = true; }

    // Se alguma tecla foi pressionada, valida e aplica o movimento
    if (pressionouBotao) {
        // Limita o movimento para -1, 0 ou 1 (uma célula por vez)
        if (moveI > 1) moveI = 1;  if (moveI < -1) moveI = -1;
        if (moveC > 1) moveC = 1;  if (moveC < -1) moveC = -1;

        // Chama a função de movimento do mapa, que verifica colisões
        mapa.moverPlayer(moveI, moveC);
        ultimoTempo = tempoAtual;
    }

    // MODO EDITOR: Teclas numéricas alteram o tipo do tile onde o jogador está
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) { mapa.alterarTileAtual(1); ultimoTempo = tempoAtual; } 
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) { mapa.alterarTileAtual(2); ultimoTempo = tempoAtual; } 
    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) { mapa.alterarTileAtual(3); ultimoTempo = tempoAtual; } 
}

int main() {
    // Inicializa a janela GLFW com as configurações de vídeo
    Janela janela;
    janela.janelaInit();

    // Carrega o arquivo de mapa (mapa.txt) e prepara os dados do mapa em memória
    MapaTile mapa("mapa.txt"); 
    // Carrega e compila os shaders (programas de renderização GPU)
    mapa.carregarShader();
    // Carrega as texturas, configura a projeção 2D ortho e prepara os vértices dos tiles
    mapa.carregarConfig();

    // Executa enquanto a janela não for fechada
    while (!glfwWindowShouldClose(janela.getJanela())) {
        // Limpa a tela com a cor de fundo definida
        janela.limpar();
        // Processa inputs do teclado (movimento e edição de tiles)
        processarEntrada(janela.getJanela(), mapa);
        // Renderiza todo o mapa e personagens na tela
        mapa.carregarMapa();
        // Troca os buffers (double buffering para evitar flicker)
        janela.trocarBuffer();
        // Processa eventos da janela (redimensionar, fechar, etc)
        glfwPollEvents();
    }

    // Libera recursos e fecha a janela
    janela.fechar();
    return 0;
}