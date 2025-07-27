#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/*-------------------Constantes-------------------*/


#define MAX_CARROS 10
#define MAX_PISTAS 12
#define MAX_LARGURA 100
#define MAX_ALTURA 35
#define MAX_VIDAS 3


/*------------Tipos de Dados Definidos------------*/


typedef struct {
    int x;
    int y;
    int vidas;
    char desenho[2][4]; //2 linhas e 3 caracteres + \0
} tGalinha;

typedef struct {
    int id;
    int x;
    char desenho[4][2][4]; //4, animacoes, 2 linhas e 3 caracteres + \0
} tCarro;

typedef struct {
    int id;
    int velocidade;
    int numCarros;
    char direcao;
    tCarro carros[MAX_CARROS];
} tPista;

typedef struct {
    int idPista;
    int idCarro;
    int iteracao;
} tAtropelamentos;  //Importante para gerar o resumo e ranking

typedef struct {
    tGalinha galinha;
    tPista pistas[MAX_PISTAS];
    tAtropelamentos atropelamentos[MAX_VIDAS];
    int quantidadePistas;
    int largura;
    char grade[2][MAX_ALTURA][MAX_LARGURA]; //ACHO que e necessario o 2 para o heatmap
} tJogo;


/*------------Funcoes XXXXXX------------*/



int main() {
    int a;
    return 0;
}
