#include "MapaTile.hpp"
#include <iostream>

// Carrega a implementação da biblioteca stb_image (para ler arquivos PNG)
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

//  constructor e inicialização do mapa, leitura do arquivo, configuração de OpenGL e shaders, renderização e lógica do jogo
// Inicializa as variáveis do jogador e lê o arquivo de mapa
MapaTile::MapaTile(const std::string& caminhoArquivo) {
    playerI = 1;             // Posição inicial: linha 1
    playerC = 1;             // Posição inicial: coluna 1
    direcaoPlayer = 0;       // Jogador começa olhando para baixo (Sul)
    arquivo = caminhoArquivo;
    carregarDeArquivo(caminhoArquivo);  // Lê o arquivo mapa.txt e popula as estruturas de dados
}

// Destrutor vazio (OpenGL libera memória de texturas automaticamente)
MapaTile::~MapaTile() {}

// configurações iniciais de OpenGL, como projeção ortogonal, carregamento de texturas e configuração dos shaders
void MapaTile::carregarConfig() {
    // Define a projeção ortogonal 2D (sem perspectiva, ideal para pixel art)
    mapa2D = glm::ortho(0.0f, LARGURA_JANELA, 0.0f, ALTURA_JANELA);
    // Cria o VAO/VBO com os vértices de um quadrado (será escalado para cada tile)
    criarPontosTile(); 

    // Carrega as imagens PNG para a memória de vídeo (GPU)
    // ATENÇÃO: As imagens devem estar no mesmo diretório do executável 🐵 com cmake tem que estar na build uma pasta img com as img
    TID_tileset = carregarTextura("img/Tileset.png");        // Imagem com chão e objetos
    TID_personagem = carregarTextura("img/spriteSheet8.png"); // Sprite do personagem com 8 frames
}

// Carrega e compila os shaders GLSL
void MapaTile::carregarShader() {
    // Carrega os arquivos de shader e compila o programa
    shaderPrograme.carregar_arquivo("static/vMapShader.glsl", "static/fMapShader.glsl");
    
    // Conecta as variáveis do C++ com as variáveis uniform do shader GLSL
    // Isso permite enviar dados do CPU para a GPU
    modelLoc = glGetUniformLocation(shaderPrograme.get_program(), "u_Model");        // Matriz de modelo (posição e escala)
    projLoc = glGetUniformLocation(shaderPrograme.get_program(), "u_Projecao");      // Matriz de projeção
    u_CorDoBloco = glGetUniformLocation(shaderPrograme.get_program(), "u_CorDoBloco"); // Cor do tile
    
    // Variáveis para controlar o recorte de texturas (atlas texture)
    loc_TexOffset = glGetUniformLocation(shaderPrograme.get_program(), "u_TexOffset");   // Offset dentro da textura
    loc_TexScale = glGetUniformLocation(shaderPrograme.get_program(), "u_TexScale");     // Escala de recorte da textura
    loc_UsarTextura = glGetUniformLocation(shaderPrograme.get_program(), "u_UsarTextura"); // Flag: usar textura ou cor sólida
}

//  renderiza o mapa a cada frame, desenhando o chão, objetos e personagem na ordem correta para criar a ilusão de profundidade em um mapa isométrico 
//  renderiza todo o mapa a cada frame
void MapaTile::carregarMapa() {
    // Ativa o programa de shader para renderização
    glUseProgram(shaderPrograme.get_program());
    // Envia a matriz de projeção ortogonal para o shader
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(mapa2D));
    // Ativa o Vertex Array Object que contém os vértices dos tiles
    glBindVertexArray(VAO);

    GLfloat offsetX = POSICAO_INICIALX;  // Deslocamento inicial em X na tela
    GLfloat offsetY = POSICAO_INICIALY;  // Deslocamento inicial em Y na tela

    // LOOP PRINCIPAL: Renderiza cada tile do mapa 
    // Itera por cada linha e coluna da matriz de terreno
    for (int i = 0; i < linhas; i++) {
        for (int c = 0; c < colunas; c++) {
            
            // Converte coordenadas de grid (i, c) para coordenadas de tela no formato Diamond (isométrico)
            // Está transposto porque o mapa isométrico tem eixos diagonais
            GLfloat posX = (c - i) * (LARGURA_TILE / 2.0f) + offsetX;
            GLfloat posY = (c + i) * (ALTURA_TILE / 2.0f) + offsetY;

            //CAMADA 1: CHÃO (Usando Tileset.png) 
            glUniform1i(loc_UsarTextura, GL_TRUE);      // Avisa o shader: use textura (não cor sólida)
            glBindTexture(GL_TEXTURE_2D, TID_tileset);  // Seleciona a imagem do chão
            
            // O Tileset.png contém 7 tipos diferentes de chão lado a lado
            // Então cada um ocupa 1/7 da largura
            float escalaTileX = 1.0f / 7.0f;
            int idTile = terreno[i][c];  // Lê qual é o tipo de chão nesta posição (lido do mapa.txt)
            
            // Configura qual fração da imagem será exibida neste tile
            glUniform2f(loc_TexScale, escalaTileX, 1.0f);
            glUniform2f(loc_TexOffset, idTile * escalaTileX, 0.0f); // Recorta a seção correta
            desenharTile(posX, posY, idTile);

            // CAMADA 2: OBJETOS E ITENS 
            glUniform1i(loc_UsarTextura, GL_TRUE);
            glBindTexture(GL_TEXTURE_2D, TID_tileset); // Usa a mesma imagem do tileset
            
            // Verifica se há objetos nesta posição (moedas, lava, saída)
            for (auto& obj : objetos) {
                // Só renderiza se o objeto está ativo e está nesta célula
                if (obj.ativo && obj.linha == i && obj.coluna == c) {
                    
                    int idItem = 0;          // ID do tile no tileset para renderizar
                    float alturaItem = posY; // Altura base do item
                    
                    // Mapeia o tipo de objeto para o ID correto no tileset
                    if (obj.tipo == "MOEDA") {
                        idItem = 0;          // ID 0 no tileset é a moeda
                        // Levanta um pouco para parecer flutuar sobre o chão
                        alturaItem = posY + 1.0f;
                    }
                    else if (obj.tipo == "LAVA") {
                        idItem = 3;          // ID 3 no tileset é a lava
                        // Fica rente ao chão (+1.0f apenas para evitar artefatos de renderização)
                        alturaItem = posY + 1.0f;
                    }
                    else if (obj.tipo == "SAIDA") {
                        idItem = 6;          // ID 6 no tileset é a saída
                        alturaItem = posY + 1.0f;
                    }

                    // Configura o recorte de textura (já calculamos escalaTileX acima)
                    glUniform2f(loc_TexScale, escalaTileX, 1.0f);
                    glUniform2f(loc_TexOffset, idItem * escalaTileX, 0.0f);
                    
                    // Desenha o item (o parâmetro cor é ignorado quando textura está ativa)
                    desenharTile(posX, alturaItem, 0);
                }
            }

            //  CAMADA 3: PERSONAGEM ANIMADO 
            // Verifica se o personagem está nesta célula
            if (i == playerI && c == playerC) {
                glUniform1i(loc_UsarTextura, GL_TRUE);
                glBindTexture(GL_TEXTURE_2D, TID_personagem); // Muda para a sprite sheet do personagem
                
                // A imagem do personagem tem 8 frames de animação lado a lado (1 por direção)
                float escalaSpriteX = 1.0f / 8.0f;
                
                // Configura qual frame exibir baseado na direção (direcaoPlayer vai de 0 a 7)
                glUniform2f(loc_TexScale, escalaSpriteX, 1.0f);
                glUniform2f(loc_TexOffset, direcaoPlayer * escalaSpriteX, 0.0f);

                // Desenha o personagem 8 pixels mais alto para encaixar no meio do losango do chão
                desenharTile(posX, posY + 8.0f, 99);
            }
        }
    }
    // Desativa o VAO após renderização
    glBindVertexArray(0);
}

// Função auxiliar: Desenha um único tile na posição especificada
void MapaTile::desenharTile(GLfloat posX, GLfloat posY, int cor) {
    // Cria uma matriz de transformação (identidade)
    glm::mat4 model = glm::mat4(1.0f);
    // Translada para a posição do tile
    model = glm::translate(model, glm::vec3(posX, posY, 0.0f));
    // Escala para o tamanho padrão de um tile
    model = glm::scale(model, glm::vec3(LARGURA_TILE, ALTURA_TILE, 1.0f));
    // Envia a matriz de modelo para o shader
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    // Define a cor do tile (só usado se loc_UsarTextura estiver FALSE)
    escolherCorTerreno(cor);
    // Desenha o quadrado com 6 vértices (2 triângulos)
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

// logica do jogo: controle do jogador, detecção de colisoes e interações com objetos
void MapaTile::moverPlayer(int deltaI, int deltaC) {
    // Se o jogo terminou (venceu ou gameOver(nisso python e melhor, prefiro or and e not)), congela os controles
    if (gameOver || venceu) return;

    // Mapeia a direção de movimento para qual frame da animação exibir
    // A sprite sheet tem 8 frames (0-7) representando 8 direções diferentes
    if (deltaI == -1 && deltaC == -1) direcaoPlayer = 0; // Sul (pra baixo-esquerda)
    if (deltaI == -1 && deltaC == 0)  direcaoPlayer = 1; // Sudeste (pra baixo)
    if (deltaI == -1 && deltaC == 1)  direcaoPlayer = 2; // Leste (pra baixo-direita)
    if (deltaI == 0  && deltaC == 1)  direcaoPlayer = 3; // Nordeste (pra direita)
    if (deltaI == 1  && deltaC == 1)  direcaoPlayer = 4; // Norte (pra cima-direita)
    if (deltaI == 1  && deltaC == 0)  direcaoPlayer = 5; // Noroeste (pra cima)
    if (deltaI == 1  && deltaC == -1) direcaoPlayer = 6; // Oeste (pra cima-esquerda)
    if (deltaI == 0  && deltaC == -1) direcaoPlayer = 7; // Sudoeste (pra esquerda)

    // Calcula a nova posição
    int novoI = playerI + deltaI;
    int novoC = playerC + deltaC;

    // Verifica se a nova posição está dentro dos limites do mapa
    if (novoI >= 0 && novoI < linhas && novoC >= 0 && novoC < colunas) {
        
        // Verifica se o tile de destino pode ser pisado (lido do mapa.txt)
        if (podeCaminhar(terreno[novoI][novoC])) {
            // Move o jogador
            playerI = novoI;
            playerC = novoC;
            
            // Verifica se caiu na lava (ID 3 no mapa)
            if (terreno[playerI][playerC] == 3) {
                //gameOver = true;
                printf("GAME OVER! Voce caiu na lava e derreteu.\n");//aqui
                resetarMapa();
            }
            if (terreno[playerI][playerC] == 6) {
                //gameOver = true;
                printf("SAIDA ERRADA!\n");
                alterarTileAtual(1);
                //resetarMapa();

            }

            // Verifica interações com objetos (moedas, saída)
            verificarInteracao();
        }
    }
}

// Verifica se um tile pode ser pisado
bool MapaTile::podeCaminhar(int idTile) {
    // Procura se o ID do tile está na lista de IDs caminháveis (lida do mapa.txt)
    for (int id : tilesCaminhaveis) {
        if (idTile == id) return true;
    }
    // Se não estiver na lista, é uma parede ou obstáculo
    return false;
}

// Verifica se o jogador colide ou interage com objetos
void MapaTile::verificarInteracao() {
    // Itera sobre todos os objetos do mapa
    for (auto& obj : objetos) {
        // Verifica se há um objeto ativo na posição do jogador
        if (obj.ativo && obj.linha == playerI && obj.coluna == playerC) {
            
            if (obj.tipo == "MOEDA") {
                // Desativa a moeda (desaparece do mapa)
                obj.ativo = false;
                moedasColetadas++;
                //printf("test");
                printf("Pegou Moeda! Ouro: %d/%d\n", moedasColetadas, moedasTotal);
            } 
            else if (obj.tipo == "LAVA") {
                // Lava causa game over
                //gameOver = true;
                //printf("test");
                printf("GAME OVER! Pisou na lava.\n");

                resetarMapa();
                
            } 
            else if (obj.tipo == "SAIDA") {
                // Só permite vencer se coletou todas as moedas
                if (moedasColetadas >= moedasTotal) {
                    //printf("testwin");
                    printf("VITORIA!\n");
                    resetarMapa();
                } else {
                    //printf("testdefeat");
                    printf("Faltam moedas!\n");
                }

            }
        }
    }
}

//Realiza o Reset do jogo
void MapaTile::resetarMapa()
{

    moedasColetadas = 0;
    moedasTotal = 0;
    carregarDeArquivo(arquivo);
    playerC = 1;
    playerI = 1;
    


}

// Altera o tipo de tile na posição do jogador (modo editor)
void MapaTile::alterarTileAtual(int novoTipo) {
    terreno[playerI][playerC] = novoTipo;
}

// Lê o arquivo mapa.txt e carrega os dados do mapa em memória
bool MapaTile::carregarDeArquivo(const std::string& caminho) {
    std::ifstream arquivo(caminho);
    if (!arquivo.is_open()) {
        printf("ERRO: Nao achou %s\n", caminho.c_str());
        return false;
    }

    std::string linhaStr;
    // Lê a primeira linha 
    std::getline(arquivo, linhaStr);

    // Lê a linha com os tiles caminháveis (tiles que o jogador pode pisar)
    std::getline(arquivo, linhaStr);
    std::stringstream ssCam(linhaStr);
    std::string tag;
    ssCam >> tag; // Descarta a palavra "CAMINHAVEL"
    int idCam;
    // Carrega todos os IDs de tiles caminháveis
    while (ssCam >> idCam) tilesCaminhaveis.push_back(idCam);

    // Lê as dimensões do mapa (linhas e colunas)
    arquivo >> linhas >> colunas;
    // Inicializa a matriz de terreno com zeros
    terreno.assign(linhas, std::vector<int>(colunas, 0));
    // Lê os IDs de cada tile do mapa
    for (int i = 0; i < linhas; i++) {
        for (int c = 0; c < colunas; c++) {
            arquivo >> terreno[i][c];
        }
    }

    // Le a seçao de objetos
    std::getline(arquivo, linhaStr); 
    std::getline(arquivo, linhaStr); // Descarta a linha com "OBJETOS"
    
    // Lê cada objeto (tipo, linha, coluna)
    std::string tipoObj;
    int ol, oc;
    while (arquivo >> tipoObj >> ol >> oc) {
        objetos.push_back({tipoObj, ol, oc, true});
        // Conta quantas moedas existem para a condiçao de vitoria
        if (tipoObj == "MOEDA") moedasTotal++;
    }
    arquivo.close();
    return true;
}

// Carrega uma imagem PNG e a envia para a placa de vídeo como textura
GLuint MapaTile::carregarTextura(const char* caminho) {
    GLuint textureID;
    // Gera um ID único para a textura
    glGenTextures(1, &textureID);
    // Ativa a textura como alvo das operações seguintes
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Configura filtros de renderização
    // GL_NEAREST mantém os pixels nítidos (ideal para pixel art)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    int width, height, nrChannels;
    // stbi_set_flip_vertically_on_load: OpenGL lê texturas de baixo pra cima,
    // enquanto imagens PNG são salvas de cima pra baixo. Isso inverte a imagem.
    stbi_set_flip_vertically_on_load(true); 
    // Carrega a imagem usando stb_image
    unsigned char *data = stbi_load(caminho, &width, &height, &nrChannels, 0);
    
    if (data) {
        // Detecta se a imagem tem canal alfa 
        GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
        // Envia os dados da imagem para a GPU
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        // Gera mipmaps (versões reduzidas da textura para renderização de distância)
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cout << "ERRO DE TEXTURA: " << caminho << std::endl;
    }
    // Libera a memória da imagem (já está na GPU)
    stbi_image_free(data);
    return textureID;
}

// Cria o VAO e VBO com um quadrado de dois triângulos
void MapaTile::criarPontosTile() {
    // Define um quadrado unitário (1x1) com coordenadas de textura
    // Cada vértice tem 4 floats: (posX, posY, texU, texV)
    GLfloat vertices[] = {
        // Vértice 1      // Coordenada de textura
        0.0f, 1.0f,         0.0f, 1.0f, 
        // Vértice 2
        1.0f, 1.0f,         1.0f, 1.0f, 
        // Vértice 3
        0.0f, 0.0f,         0.0f, 0.0f, 

        // Segundo triângulo (compartilha 2 vértices com o primeiro)
        1.0f, 1.0f,         1.0f, 1.0f, 
        1.0f, 0.0f,         1.0f, 0.0f, 
        0.0f, 0.0f,         0.0f, 0.0f  
    };

    // Cria um Vertex Array Object (gerencia os atributos dos vértices)
    glGenVertexArrays(1, &VAO);
    // Cria um Vertex Buffer Object (armazena os dados dos vértices na GPU)
    glGenBuffers(1, &VBO);
    // Ativa o VAO
    glBindVertexArray(VAO);
    // Ativa o VBO
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // Envia os dados dos vértices para o VBO
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Configure o atributo de posição (localização 0 no shader)
    // 2 floats por vértice, stride de 4 floats
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void *)0);
    glEnableVertexAttribArray(0);
    
    // Configure o atributo de coordenada de textura (localização 1 no shader)
    // 2 floats por vértice, começa no offset de 2 floats (após a posição)
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void *)(2 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    // Desativa o VAO
    glBindVertexArray(0);
}

// Define a cor de um tile (usado apenas quando a textura está desativada)
void MapaTile::escolherCorTerreno(int idDoBloco) {
    glm::vec3 corTile(1.0f, 1.0f, 1.0f); // Cor padrão: branco
    
    // Cores especiais para IDs especiais
    if (idDoBloco == 100) corTile = glm::vec3(1.0f, 0.8f, 0.0f); // Amarelo para moeda
    else if (idDoBloco == 101) corTile = glm::vec3(0.9f, 0.1f, 0.1f); // Vermelho para lava
    else if (idDoBloco == 102) corTile = glm::vec3(1.0f, 1.0f, 1.0f); // Branco para saída
    
    // Envia a cor para o shader
    glUniform3f(u_CorDoBloco, corTile.r, corTile.g, corTile.b);
}