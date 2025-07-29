#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/*------------------------------------------------Constantes------------------------------------------------*/


#define MAX_CARROS 10
#define MAX_PISTAS 12
#define MAX_LARGURA 102      //largura + 2 para laterais
#define MAX_ALTURA 37        //altura + 2 para borda superior e inferior
#define MAX_VIDAS 3


/*-----------------------------------------Tipos de Dados Definidos-----------------------------------------*/


typedef struct {
    int x;
    int y;
    int vidas;
    char desenhoGalinha[2][4];    //2 linhas e 3 caracteres + \0
} tGalinha;

typedef struct {
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
    int animacoes;
    int quantidadePistas;
    int largura;
    int iteracao;
    char grade[MAX_ALTURA][MAX_LARGURA]; 
    tGalinha galinha;
    tPista pistas[MAX_PISTAS];    // o id da pista e a posicao no vetor
    tAtropelamentos atropelamentos[MAX_VIDAS];
} tJogo;


/*---------------------------------------------Funcoes Galinha----------------------------------------------*/


tGalinha MovimentaGalinha(tGalinha galinha, char mov, int qtdPistas) {
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

void DesenhaGalinha(tGalinha galinha, char mapa[MAX_ALTURA][MAX_LARGURA]) {
    int i;
    for(i = -1; i <= 1; i++) {                //comeca em -1 porque a posicao central e a posicao da galinha
        mapa[galinha.y][galinha.x+i] = galinha.desenhoGalinha[0][i+1]; 
        mapa[galinha.y+1][galinha.x+i] = galinha.desenhoGalinha[1][i+1];
    }
}


/*----------------------------------------------Funcoes Pista-----------------------------------------------*/


tPista MovimentaCarrosNaPista(tPista pista, int largura) {
    int i;
    if(pista.direcao == 'D') {
        for(i = 0; i < pista.numCarros; i++) {
            pista.carrosX[i] += pista.velocidade; //Move para direita
            if(pista.carrosX[i] > largura) {
                pista.carrosX[i] -= largura;      //Aparece do outro lado ao ultrapassar a borda
            }
        }
    }
    else if(pista.direcao == 'E') {
        for(i = 0; i < pista.numCarros; i++) {
            pista.carrosX[i] -= pista.velocidade;  //Move para esquerda
            if(pista.carrosX[i] < 1) {
                pista.carrosX[i] += largura;       //Aparece do outro lado ao ultrapassar a borda
            }
        }
    }
    return pista;
}

tPista ReduzVelocidadePista(tPista pista) {
    if(pista.velocidade > 1) {
        pista.velocidade--;
    }
    return pista;
}

int VerificaColisaoNaPista(tPista pista, tGalinha galinha, int largura) {
    int i;
    int xGalinha = ObtemXGalinha(galinha);
    for(i = 0; i < pista.numCarros; i++) {
        if(abs(pista.carrosX[i] - xGalinha) <= 2) {  
            return i+1;      //Verifica se a distancia entre a galinha e algum carro causa colisao e retorna o carro(comecando do 1)
        }
        else if((xGalinha == 2 && pista.carrosX[i] == largura) || (xGalinha == largura-1 && pista.carrosX[i] == 1)) {
            return i+1;      //Verifica se a galinha esta numa extremidade e o carro acabou de atravessar a borda e retorna o carro(comecando do 1)
        }
    }
    return 0;
}

void DesenhaCarrosNaPista(tPista pista, char mapa[MAX_ALTURA][MAX_LARGURA], int numPista, int largura, int animacao, int iteracao) { //
    int i, j;
    int ani = iteracao%4;         //Pega o resto da div para decidir qual animacao usar
    int y = 3*numPista + 1;

    if(!animacao) {
        ani = 0;
    }
    for(j = 0; j < pista.numCarros; j++) {
        if(pista.carrosX[j] == 1) {     //Verifica se o x do carro esta na borda esquerda e desenha a ponta do outro lado
            for(i = 0; i <= 1; i++) {                
                mapa[y][pista.carrosX[j]+i] = pista.desenhoCarro[ani][0][i+1];
                mapa[y+1][pista.carrosX[j]+i] = pista.desenhoCarro[ani][1][i+1];
            }
            mapa[y][largura] = pista.desenhoCarro[ani][0][0];
            mapa[y+1][largura] = pista.desenhoCarro[ani][1][0];
        }
        else if(pista.carrosX[j] == largura) { //Verifica se o x do carro esta na borda direita e desenha a ponta do outro lado
            for(i = -1; i <= 0; i++) {                
                mapa[y][pista.carrosX[j]+i] = pista.desenhoCarro[ani][0][i+1];
                mapa[y+1][pista.carrosX[j]+i] = pista.desenhoCarro[ani][1][i+1];
            }
            mapa[y][1] = pista.desenhoCarro[ani][0][2];
            mapa[y+1][1] = pista.desenhoCarro[ani][1][2];
        }
        else if(pista.carrosX[j] > 1 && pista.carrosX[j] < largura) {
            for(i = -1; i <= 1; i++) {                
                mapa[y][pista.carrosX[j]+i] = pista.desenhoCarro[ani][0][i+1];
                mapa[y+1][pista.carrosX[j]+i] = pista.desenhoCarro[ani][1][i+1];
            }
        }
    }
}


/*-----------------------------------------------Funcoes Jogo-----------------------------------------------*/


int AcabouJogo(tJogo jogo) {
    if(ObtemVidasGalinha(jogo.galinha) == 0 || ObtemYGalinha(jogo.galinha) == 1) {
        return 1;
    }
    return 0;
}

void DesenhaMapaJogo(tJogo jogo) {  //Desenha o mapa, printa, e apaga em sequencia            ////Possivel erro
    int i, j;
    DesenhaGalinha(jogo.galinha, jogo.grade);
    for(i = 0; i < jogo.quantidadePistas; i++) {
        DesenhaCarrosNaPista(jogo.pistas[i], jogo.grade, i, jogo.largura, jogo.animacoes, jogo.iteracao);
    }
    for(i = 0; i < jogo.quantidadePistas*3 + 1; i++) {       //printa o grid
        for(j = 0; j < jogo.largura + 2; j++) {              //largura + 2 para correcao da borda
            printf("%c", jogo.grade[i][j]);
        }
        printf("\n");
    }
    for(i = 1; i < jogo.quantidadePistas*3 + 1; i++) {       //apaga as alteracoes
        for(j = 1; j < jogo.largura + 1; j++) {
            if(i%3) {
                jogo.grade[i][j] = ' ';              /////////////////////Possivel erro
            }
        }
    }
}

void JogaJogo(tJogo jogo) {
    int i;
    int pistaGalinha;
    char movimento = 0;


    while(!AcabouJogo(jogo)) {
        scanf("%c", &movimento);
        if(movimento != 'w' && movimento != 's' && movimento != ' ') {
            continue;
        }
        jogo.galinha = MovimentaGalinha(jogo.galinha, movimento, jogo.quantidadePistas);
        for(i = 0; i < jogo.quantidadePistas; i++) {
            jogo.pistas[i] = MovimentaCarrosNaPista(jogo.pistas[i], jogo.largura);
        }
        pistaGalinha = (ObtemYGalinha(jogo.galinha) - 1)/3;

        if(VerificaColisaoNaPista(jogo.pistas[pistaGalinha], jogo.galinha, jogo.largura)) {
            jogo.galinha = ResetaGalinha(jogo.galinha, jogo.quantidadePistas);
            if(jogo.animacoes) {
                jogo.pistas[pistaGalinha] = ReduzVelocidadePista(jogo.pistas[pistaGalinha]);
            }
        }

        jogo.iteracao++;
        printf("VIDAS:%d, ITERACAO:%d\n\n", jogo.galinha.vidas, jogo.iteracao);
        DesenhaMapaJogo(jogo);
    }
}


/*---------------------------------------------------MAIN---------------------------------------------------*/


int main() {
    int i, j;


    tJogo jogo;
    jogo.largura = 25;
    jogo.animacoes = 1;
    jogo.quantidadePistas = 4;
    jogo.iteracao = -1;
    jogo.pistas[1].velocidade = 2;
    jogo.pistas[1].direcao = 'D';
    jogo.pistas[1].numCarros = 2;
    jogo.pistas[1].carrosX[0] = 2;
    jogo.pistas[1].carrosX[1] = 9;
    jogo.pistas[0].numCarros = 0;
    jogo.pistas[2].numCarros = 0;
    jogo.pistas[3].numCarros = 0;


    strcpy(jogo.galinha.desenhoGalinha[0], "^_^");
    strcpy(jogo.galinha.desenhoGalinha[1], "(o)");
    jogo.galinha.vidas = 2;
    jogo.galinha.x = 12;
    jogo.galinha.y = 10;

    strcpy(jogo.pistas[1].desenhoCarro[0][0], "[-]");
    strcpy(jogo.pistas[1].desenhoCarro[0][1], "---");
    strcpy(jogo.pistas[1].desenhoCarro[1][0], "[-]");
    strcpy(jogo.pistas[1].desenhoCarro[1][1], "/-/");
    strcpy(jogo.pistas[1].desenhoCarro[2][0], "[-]");
    strcpy(jogo.pistas[1].desenhoCarro[2][1], "|-|");
    strcpy(jogo.pistas[1].desenhoCarro[3][0], "[-]");
    strcpy(jogo.pistas[1].desenhoCarro[3][1], "\\-\\");

    for(i  = 0; i < jogo.quantidadePistas*3+1; i++) {
        for(j = 0; j < jogo.largura+2; j++) {
            jogo.grade[i][j] = '0';
        }
    }

    JogaJogo(jogo);

    return 0;
}
