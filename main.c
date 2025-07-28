#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_CARROS 10
#define MAX_PISTAS 12
#define MAX_LARGURA 102      //largura + 2 para laterais
#define MAX_ALTURA 36        //altura + 1 para borda superior
#define MAX_VIDAS 3

/*------------Tipos de Dados Definidos------------*/

typedef struct {
    int x;
    int y;
    int vidas;
    char desenhoGalinha[2][4];    //2 linhas e 3 caracteres + \0
} tGalinha;

typedef struct {
    int id;
    int velocidade;
    int numCarros;
    char direcao; 
    int carrosX[MAX_CARROS];        //o id do carro e a posicao no vetor
    char desenhoCarro[4][2][4];     //4, animacoes, 2 linhas e 3 caracteres + \0
} tPista;

typedef struct {
    int idPista;
    int idCarro;
    int iteracao;
} tAtropelamentos;              //Importante para gerar o resumo e ranking

typedef struct {
    int quantidadePistas;
    int largura;
    char grade[MAX_ALTURA+1][MAX_LARGURA+2]; 
    tGalinha galinha;
    tPista pistas[MAX_PISTAS];
    tAtropelamentos atropelamentos[MAX_VIDAS];
} tJogo;

/*------------Funcoes Galinha------------*/

tGalinha MovimentoGalinha(tGalinha galinha, char mov, int qtdPistas) {
    if(mov == 'w') {
        galinha.y -= 3;                                     // 3 porque a largura da pista e de 3 linhas
    }
    else if(mov == 's' && galinha.y < qtdPistas*3 - 2) {    // verifica se o movimento e "s", e se tem espaco abaixo 
        galinha.y += 3;
    }
    return galinha;
}

tGalinha ResetaGalinha(tGalinha galinha, int qtdPistas) {      //Reseta o y e tira uma vida (quando e atropelada)
    galinha.vidas--;
    galinha.y = qtdPistas*3 - 2;
    return galinha;
}

int ObtemXGalinha(tGalinha galinha) {
    return galinha.x;
}

int ObtemYGalinha(tGalinha galinha) {
    return galinha.y;
}

int ObtemVidasGalinha(tGalinha galinha) {
    return galinha.vidas;
}

void DesenhaGalinha(tGalinha galinha, char mapa[MAX_ALTURA+1][MAX_LARGURA+2]) { //
    int i;
    for(i = -1; i <= 1; i++) { //comeca em -1 porque a posicao central e a posicao da galinha
        mapa[galinha.y][galinha.x+i] = galinha.desenhoGalinha[0][i+1]; 
        mapa[galinha.y+1][galinha.x+i] = galinha.desenhoGalinha[1][i+1];
    }
}

void ApagaGalinha(tGalinha galinha, char mapa[MAX_ALTURA+1][MAX_LARGURA+2]) { //
    int i;
    for(i = -1; i <= 1; i++) { //comeca em -1 porque a posicao central e a posicao da galinha
        mapa[galinha.y][galinha.x+i] = ' '; 
        mapa[galinha.y+1][galinha.x+i] = ' ';
    }
}

/*-------------------------MAIN-------------------------*/

int main() {
    int i, j;
    char ch = 'w';

    tJogo jogo;
    jogo.largura = 12;
    jogo.quantidadePistas = 4;
    jogo.galinha.vidas = 3;
    jogo.galinha.x = 6;
    jogo.galinha.y = jogo.quantidadePistas*3 - 2;
    strcpy(jogo.galinha.desenhoGalinha[0], "^_^");
    strcpy(jogo.galinha.desenhoGalinha[1], "(o)");


    return 0;
}
