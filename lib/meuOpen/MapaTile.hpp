#pragma once

#include "configTile.hpp"
#include "configTela.hpp"
#include "Shader.h"
#include <gl/glew.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>

// ESTRUTURA: ObjetoJogo
// Representa itens coletáveis ou obstáculos no mapa (moedas, lava, saída)
struct ObjetoJogo {
    std::string tipo;      // Tipo do objeto: "MOEDA", "LAVA", "SAIDA"
    int linha, coluna;     // Posição no grid do mapa
    bool ativo = true;     // Se false, o objeto foi coletado ou destruído (não será renderizado)
};

// Gerencia o mapa isométrico, renderização, lógica do jogo e interações
class MapaTile {
public:
    // Construtor: carrega o arquivo de mapa (arquivo .txt) e inicializa o jogador
    MapaTile(const std::string& caminhoArquivo); 
    ~MapaTile();
    
    // Funções de renderização
    void carregarMapa();        // Renderiza o mapa, objetos e personagem a cada frame
    void carregarShader();      // Carrega e compila os shaders GLSL
    void carregarConfig();      // Configura texturas, VAO/VBO e projeção ortogonal
    void desenharTile(GLfloat posX, GLfloat posY, int cor); // Desenha um único tile na tela

    // Variáveis do jogador (públicas para fácil acesso)
    int playerI = 1;            // Linha do jogador na grid do mapa
    int playerC = 1;            // Coluna do jogador na grid do mapa
    int direcaoPlayer = 0;      // Direção para qual o personagem está virado (0-7, representa os 8 frames de animação)
    
    // Funções de controle do jogador
    void moverPlayer(int deltaI, int deltaC);  // Move o jogador, verifica colisões e interações
    void alterarTileAtual(int novoTipo);       // Altera o tipo do tile onde o jogador está (modo editor)

private:
    //  VARIÁVEIS OPENGL E RENDERIZAÇÃO 
    GLuint linhas, colunas;         // Dimensões do mapa em grid
    GLuint projLoc, u_CorDoBloco, modelLoc;  // Locações de uniforms do shader
    GLuint VAO, VBO;                // Vertex Array Object e Vertex Buffer Object (geometria do tile)
    glm::mat4 mapa2D;               // Matriz de projeção ortogonal 2D
    Shader shaderPrograme;          // Programa de shaders para renderização
    std::vector<std::vector<int>> terreno;  // Matriz que armazena os IDs de cada tile do mapa
    std::string arquivo;

    //  VARIÁVEIS DE TEXTURA 
    GLuint TID_tileset;             // ID da textura do tileset (chão e objetos)
    GLuint TID_personagem;          // ID da textura da sprite sheet do personagem
    GLuint loc_TexOffset;           // Localização do uniform para recorte de textura (offset)
    GLuint loc_TexScale;            // Localização do uniform para escala de textura
    GLuint loc_UsarTextura;         // Localização do uniform que ativa/desativa o uso de texturas

    //  VARIÁVEIS DE LÓGICA DO JOGO 
    std::vector<int> tilesCaminhaveis;  // Lista de IDs de tiles nos quais o jogador pode pisar
    std::vector<ObjetoJogo> objetos;    // Lista de todos os objetos do mapa (moedas, lava, saída)
    int moedasColetadas = 0;            // Contador de moedas coletadas
    int moedasTotal = 0;                // Total de moedas no mapa
    bool gameOver = false;              // Indica se o jogador perdeu (caiu na lava)
    bool venceu = false;                // Indica se o jogador venceu (coletou todas as moedas e chegou na saída)

    //  FUNÇÕES AUXILIARES PRIVADAS 
    void criarPontosTile();                         // Cria o VAO/VBO com a geometria de um quadrado
    void escolherCorTerreno(int cor);               // Define a cor de um tile (usado sem textura)
    GLuint carregarTextura(const char* caminho);    // Carrega uma imagem PNG como textura OpenGL
    bool carregarDeArquivo(const std::string& caminho); // Parse do arquivo .txt com configuração do mapa
    bool podeCaminhar(int id);                      // Verifica se um tile pode ser pisado
    void verificarInteracao();                      // Verifica colisões com objetos (moedas, lava, saída)
    void resetarMapa();
};