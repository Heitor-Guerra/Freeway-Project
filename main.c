#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/*------------------------------------------------Constantes------------------------------------------------*/


#define MAX_CARROS 10
#define MAX_PISTAS 12
#define MAX_LARGURA 102      //largura + 2 para laterais
#define MAX_ALTURA 37        //altura + 2 para borda superior e inferior


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
    int x;
    int y;
} tAtropelamentos;              //Importante para gerar o resumo e ranking

typedef struct {
    int animacoes;
    int quantidadePistas;
    int largura;
    int iteracao;
    char grade[MAX_ALTURA][MAX_LARGURA]; 
    tGalinha galinha;
    tPista pistas[MAX_PISTAS];    // o id da pista e a posicao no vetor
} tJogo;


/*---------------------------------------------Funcoes Galinha----------------------------------------------*/


tGalinha InicializaGalinha() {

}

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


tPista InicializaPista(){

}

tPista MovimentaCarrosNaPista(tPista pista, int largura) {
    int i;
    switch (pista.direcao) {
        case 'D':
            for(i = 0; i < pista.numCarros; i++) {
                pista.carrosX[i] += pista.velocidade; //Move para direita
                if(pista.carrosX[i] > largura) {
                    pista.carrosX[i] -= largura;      //Aparece do outro lado ao ultrapassar a borda
                }
            }
            break;
        case 'E':
            for(i = 0; i < pista.numCarros; i++) {
                pista.carrosX[i] -= pista.velocidade;  //Move para esquerda
                if(pista.carrosX[i] < 1) {
                    pista.carrosX[i] += largura;       //Aparece do outro lado ao ultrapassar a borda
                }
            }
            break;
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


/*-------------------------------------------Funcoes Atropelamento------------------------------------------*/


tAtropelamentos AtribuiInfo(int idPista, int idCarro, int iteracao, int x, int y) {
    tAtropelamentos atropelamento;
    atropelamento.idPista = idPista;
    atropelamento.idCarro = idCarro;
    atropelamento.iteracao = iteracao;
    atropelamento.x = x;
    atropelamento.y = y;
    return atropelamento;
}

void TrocaPosicaoVetor(tAtropelamentos vet[], int pos1, int pos2) {
    tAtropelamentos aux;
    aux = vet[pos1];
    vet[pos1] = vet[pos2];
    vet[pos2] = aux;
}
//Nao testado
void OrdenaAtropelamentos(tAtropelamentos atropelamentos[], int numAtropelamentos) { //Ordenacao BubbleSort
    int i, pronto = 0;        
    while(!pronto) {
        for(i = 1; i < numAtropelamentos; i++) {
            if(atropelamentos[i-1].idPista > atropelamentos[i].idPista) {
                TrocaPosicaoVetor(atropelamentos, i-1, i);
            }
            else if(atropelamentos[i-1].idPista == atropelamentos[i].idPista) {
                if(atropelamentos[i-1].idCarro > atropelamentos[i].idCarro) {
                    TrocaPosicaoVetor(atropelamentos, i-1, i);
                }
                else if((atropelamentos[i-1].idCarro == atropelamentos[i].idCarro) && atropelamentos[i-1].iteracao > atropelamentos[i].iteracao) {
                    TrocaPosicaoVetor(atropelamentos, i-1, i);
                }
            }
        }
        pronto = 1;
        for(i = 1; i < numAtropelamentos; i++) {
            if(atropelamentos[i-1].idPista > atropelamentos[i].idPista) {
                pronto = 0;
            }
            else if(atropelamentos[i-1].idPista == atropelamentos[i].idPista) {
                if(atropelamentos[i-1].idCarro > atropelamentos[i].idCarro) {
                    pronto = 0;
                }
                else if((atropelamentos[i-1].idCarro == atropelamentos[i].idCarro) && atropelamentos[i-1].iteracao > atropelamentos[i].iteracao) {
                    pronto = 0;
                }
            }
        }
    }
}


/*-----------------------------------------------Funcoes Jogo-----------------------------------------------*/


tJogo InicializaMapaJogo(tJogo jogo) {
    int i, j;
    for(i = 0; i < jogo.quantidadePistas*3 + 1; i++) {
        for(j = 0; j < jogo.largura + 2; j++) {
            if(j == 0 || j == jogo.largura + 1) {      //Desenha as laterais
                jogo.grade[i][j] = '|';
            }
            else if(i%3 == 0) {                    //Desenha as divisorias de pista
                if(j%3) {
                    jogo.grade[i][j] = '-';
                }
                else {
                    jogo.grade[i][j] = ' ';
                }
            }
            else {                                 //Desenha os espacoes na pista
                jogo.grade[i][j] = ' ';
            }
        }
    }
    return jogo;
}

tJogo InicializaJogo() {


}

int AcabouJogo(tJogo jogo) {
    if(ObtemVidasGalinha(jogo.galinha) == 0 || ObtemYGalinha(jogo.galinha) == 1) {
        return 1;
    }
    return 0;
}

void DesenhaMapaJogo(tJogo jogo, int pontos) {  //Desenha e printa o mapa  //Como a funcao e void nao causa alteracao permanente no grid
    int i, j;

    DesenhaGalinha(jogo.galinha, jogo.grade);
    for(i = 0; i < jogo.quantidadePistas; i++) {
        DesenhaCarrosNaPista(jogo.pistas[i], jogo.grade, i, jogo.largura, jogo.animacoes, jogo.iteracao);
    }

    printf("Pontos: %d | Vidas: %d | Iteracoes: %d\n", pontos, ObtemVidasGalinha(jogo.galinha), jogo.iteracao);
    for(i = 0; i < jogo.quantidadePistas*3 + 1; i++) {       //printa o grid
        for(j = 0; j < jogo.largura + 2; j++) {              //largura + 2 para correcao da borda
            printf("%c", jogo.grade[i][j]);
        }
        printf("\n");
    }
}

void JogaJogo(tJogo jogo) {
    int i, pistaGalinha, carroAtr;
    int pontos = 0;
    char movimento = 0;

    int numMov = 0, numMovS = 0, yMaxGalinha = 0, yMaxAtr = 0, yMinAtr = 0;
    int numAtr = 0;                                                             //Variaveis para estatistica
    tAtropelamentos atropelamentos[ObtemVidasGalinha(jogo.galinha)];

    DesenhaMapaJogo(jogo, pontos);
    while(!AcabouJogo(jogo)) {

        /*------------------------Movimento da Galinha e Carros------------------------*/
        scanf("%c", &movimento);
        if(movimento != 'w' && movimento != 's' && movimento != ' ') {
            continue;
        }
        switch(movimento) {
            case 'w':
                numMov++;
                break;
            case 's':
                numMov++;
                numMovS++;
                break;
        }
        jogo.galinha = MovimentaGalinha(jogo.galinha, movimento, jogo.quantidadePistas);
        for(i = 0; i < jogo.quantidadePistas; i++) {
            jogo.pistas[i] = MovimentaCarrosNaPista(jogo.pistas[i], jogo.largura);
        }
        pistaGalinha = (ObtemYGalinha(jogo.galinha) - 1)/3;

        /*-------------------------------Atropelamentos-------------------------------*/
        carroAtr = VerificaColisaoNaPista(jogo.pistas[pistaGalinha], jogo.galinha, jogo.largura);
        if(carroAtr) {
            atropelamentos[numAtr] = AtribuiInfo(pistaGalinha+1, carroAtr, jogo.iteracao, ObtemXGalinha(jogo.galinha), ObtemYGalinha(jogo.galinha));
            jogo.galinha = ResetaGalinha(jogo.galinha, jogo.quantidadePistas);
            pontos = 0;
            numAtr++;
            if(jogo.animacoes) {
                jogo.pistas[pistaGalinha] = ReduzVelocidadePista(jogo.pistas[pistaGalinha]);
            }
        }
        else if (!VerificaColisaoNaPista(jogo.pistas[pistaGalinha], jogo.galinha, jogo.largura) && movimento == 'w') {
            pontos++;
        }


        /*---------------------------Print do Mapa e outros---------------------------*/
        if(ObtemYGalinha(jogo.galinha) == 1) {
            pontos += 10;
        }
        jogo.iteracao++;
        DesenhaMapaJogo(jogo, pontos);
    }

    if(ObtemYGalinha(jogo.galinha) == 1) {
        printf("Parabens! Voce atravessou todas as pistas e venceu!\n");
    }
    else {
        printf("Voce perdeu todas as vidas! Fim de jogo.\n");
    }

}


/*---------------------------------------------------MAIN---------------------------------------------------*/


tJogo jogo;        //Struct muito grande, declarada global para nao ocorrer falha de segmentacao
int main() {       //Nao atrapalha outras funcoes, pois foi declarada no fim do codigo
    int i, j;


    jogo = InicializaMapaJogo(jogo);
    JogaJogo(jogo);

    return 0;
}
