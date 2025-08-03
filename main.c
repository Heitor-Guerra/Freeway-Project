#include <stdio.h>
#include <stdlib.h>


/*------------------------------------------------Constantes------------------------------------------------*/


#define MAX_CARROS 10
#define MAX_PISTAS 12
#define MAX_LARGURA 102      //largura + 2 para laterais      //Ao longo do codigo: y = pista*3 + 1, e largura + 2 
#define MAX_ALTURA 37        //altura + 2 para borda superior e inferior
#define MAX_CAMINHO 1000


/*-----------------------------------------Tipos de Dados Definidos-----------------------------------------*/


typedef struct {
    int x;
    int y;
    int vidas;
} tGalinha;

typedef struct {
    int velocidade;
    int numCarros;
    char direcao; 
    int carrosX[MAX_CARROS];        //o id do carro e a posicao no vetor
} tPista;

typedef struct {
    int idPista;              //Importante para gerar o resumo e ranking
    int idCarro;
    int iteracao;
    int x;
    int y;
} tAtropelamentos;

typedef struct {
    int animacoes;
    int quantidadePistas;
    int largura;
    int iteracao;
    char grade[MAX_ALTURA][MAX_LARGURA]; 
    char personagensSprites[5][2][4];    //0-3 carro e animacoes,  4 galinha, cada um com 2 linhas e 4 colunas (3 chars e \0)
    tGalinha galinha;
    tPista pistas[MAX_PISTAS];    // o id da pista e a posicao no vetor
} tJogo;


/*-------------------------------------------Funcoes Inicializacao------------------------------------------*/


tGalinha InicializaGalinha(int x, int vidas, int quantidadePistas) {   //Funcao Galinha
    tGalinha galinha;
    galinha.x = x;
    galinha.y = quantidadePistas * 3 - 2;         //Nascer na ultima pista
    galinha.vidas = vidas;
    return galinha;
}

tPista InicializaPista(char direcao, int velocidade, int numCarros, int CarrosX[]) {  //Funcao Pista
    int i;
    tPista pista;
    pista.direcao = direcao;
    pista.velocidade = velocidade;
    pista.numCarros = numCarros;
    for(i = 0; i < numCarros; i++) {
        pista.carrosX[i] = CarrosX[i];
    }
    return pista;
}

tJogo InicializaMapaJogo(tJogo jogo) {  //Funcao Jogo
    int i, j;
    for(i = 0; i < jogo.quantidadePistas*3 + 1; i++) {
        for(j = 0; j < jogo.largura + 2; j++) {
            if(j == 0 || j == jogo.largura + 1) {      //Desenha as laterais
                jogo.grade[i][j] = '|';
            }
            else if((i%3 == 0 && j%3) || i == 0 || i == jogo.quantidadePistas*3) {     //Linha e multiplo de 3, e a coluna nao e   //Desenha as divisorias de pista
                jogo.grade[i][j] = '-';
            }
            else {                                 //Desenha os espacoes na pista
                jogo.grade[i][j] = ' ';
            }
        }
    }
    return jogo;
}

tJogo InicializaJogo(char * argv) {  // Funcao Jogo
    int i, j;
    int pistaVelocidade, pistaNumCarros;
    char pistaDirecao;
    int xGalinha, galinhaVidas;
    char diretorio[MAX_CAMINHO];
    tJogo jogo;
    FILE * arquivoConfig; FILE * arquivoPersonagens;
    sprintf(diretorio, "%s/config_inicial.txt", argv); //Junta o caminho e o nome

    arquivoConfig = fopen(diretorio, "r");
    if(!arquivoConfig) {
        printf("ERRO (%s): Nao foi possivel abrir config_inicial.txt", diretorio);
        exit(1);
    }
    
    fscanf(arquivoConfig, "%d %d %d", &jogo.animacoes, &jogo.largura, &jogo.quantidadePistas);
    fscanf(arquivoConfig, "%*c");      //Apagar UM \n antes de pegar as pistas
    for(i = 0; i < jogo.quantidadePistas; i++) {
        fscanf(arquivoConfig, "%c", &pistaDirecao);
        if(pistaDirecao == 'D' || pistaDirecao == 'E') {
            fscanf(arquivoConfig, "%d", &pistaVelocidade);
            fscanf(arquivoConfig, "%d", &pistaNumCarros);
            int CarrosX[pistaNumCarros];
            for(j = 0; j < pistaNumCarros; j++) {
                fscanf(arquivoConfig, "%d", &CarrosX[j]);
            }
            jogo.pistas[i] = InicializaPista(pistaDirecao, pistaVelocidade, pistaNumCarros, CarrosX);
        }
        else if(pistaDirecao == 'G') {
            fscanf(arquivoConfig, "%d", &xGalinha);
            fscanf(arquivoConfig, "%d", &galinhaVidas);
            jogo.galinha = InicializaGalinha(xGalinha, galinhaVidas, jogo.quantidadePistas);
            jogo.pistas[i] = InicializaPista(0, 0, 0, 0);
        }
        else {
            jogo.pistas[i] = InicializaPista(0, 0, 0, 0);
            continue;                    //O continue e para nao consumir mais de um \n
        }
        fscanf(arquivoConfig, "%*[^\n]");
        fscanf(arquivoConfig, "%*c");    //Apagar UM \n no final da pista
    }
    fclose(arquivoConfig);

    sprintf(diretorio, "%s/personagens.txt", argv); //Junta o caminho e o nome
    arquivoPersonagens = fopen(diretorio, "r");
    if(!arquivoPersonagens) {
        printf("ERRO (%s): Nao foi possivel abrir personagens.txt", diretorio);
        fclose(arquivoConfig);
        exit(1);
    }

    fscanf(arquivoPersonagens, "%3[^\n]\n", jogo.personagensSprites[4][0]);   //Carregar a galinha
    fscanf(arquivoPersonagens, "%3[^\n]\n", jogo.personagensSprites[4][1]);   //OBS: Necessario ter %3[^\n]\n para quando tiver espacos no sprite
    for(i = 0; i < 4; i++) {                    //Caso adicione mais sprites de personagem, alterar aqui e nos desenhos
        for(j = 0; j < 2; j++) {
            fscanf(arquivoPersonagens, "%3[^\n]\n", jogo.personagensSprites[i][j]);  //Carregar os carros
        }
    }
    fclose(arquivoPersonagens);

    jogo.iteracao = 0;
    jogo = InicializaMapaJogo(jogo);
    return jogo;
}


/*---------------------------------------------Funcoes Galinha----------------------------------------------*/


tGalinha MovimentaGalinha(tGalinha galinha, char mov, int qtdPistas) {
    if(mov == 'w') {
        galinha.y -= 3;                                     // 3 porque a largura da pista e de 3 linhas
    }
    else if(mov == 's' && galinha.y < qtdPistas*3 - 2) {    // Verifica se o movimento e "s", e se tem espaco abaixo 
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

void DesenhaGalinha(tGalinha galinha, char mapa[MAX_ALTURA][MAX_LARGURA], char desenhos[5][2][4]) {
    int i;
    for(i = -1; i <= 1; i++) {                //Comeca em -1 porque a posicao central e a posicao da galinha
        mapa[galinha.y][galinha.x+i] = desenhos[4][0][i+1]; 
        mapa[galinha.y+1][galinha.x+i] = desenhos[4][1][i+1];
    }
}


/*----------------------------------------------Funcoes Pista-----------------------------------------------*/


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
        pista.velocidade--;  //Quando tem animacao
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

void DesenhaCarrosNaPista(tPista pista, char mapa[MAX_ALTURA][MAX_LARGURA], int numPista, int largura, int animacao, int iteracao, char desenhos[5][2][4]) { //
    int i, j;
    int ani = iteracao%4;         //Pega o resto da div para decidir qual animacao usar
    int y = 3*numPista + 1;

    if(!animacao) {
        ani = 0;
    }
    for(j = 0; j < pista.numCarros; j++) {
        if(pista.carrosX[j] == 1) {     //Verifica se o x do carro esta na borda esquerda e desenha a ponta do outro lado
            for(i = 0; i <= 1; i++) {                
                mapa[y][pista.carrosX[j]+i] = desenhos[ani][0][i+1];
                mapa[y+1][pista.carrosX[j]+i] = desenhos[ani][1][i+1];
            }
            mapa[y][largura] = desenhos[ani][0][0];
            mapa[y+1][largura] = desenhos[ani][1][0];
        }
        else if(pista.carrosX[j] == largura) { //Verifica se o x do carro esta na borda direita e desenha a ponta do outro lado
            for(i = -1; i <= 0; i++) {                
                mapa[y][pista.carrosX[j]+i] = desenhos[ani][0][i+1];
                mapa[y+1][pista.carrosX[j]+i] = desenhos[ani][1][i+1];
            }
            mapa[y][1] = desenhos[ani][0][2];
            mapa[y+1][1] = desenhos[ani][1][2];
        }
        else if(pista.carrosX[j] > 1 && pista.carrosX[j] < largura) {
            for(i = -1; i <= 1; i++) {                
                mapa[y][pista.carrosX[j]+i] = desenhos[ani][0][i+1];
                mapa[y+1][pista.carrosX[j]+i] = desenhos[ani][1][i+1];
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
                else if((atropelamentos[i-1].idCarro == atropelamentos[i].idCarro) && atropelamentos[i-1].iteracao < atropelamentos[i].iteracao) {
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
                else if((atropelamentos[i-1].idCarro == atropelamentos[i].idCarro) && atropelamentos[i-1].iteracao <= atropelamentos[i].iteracao) {
                    pronto = 0;
                }
            }
        }
    }
}


/*---------------------------------------------Funcoes Arquivos--------------------------------------------*/


void GeraInicializacaoTXT(tJogo jogo, char * argv) {
    int i, j;
    char diretorio[MAX_CAMINHO];
    FILE * arquivoInicializacao;
    sprintf(diretorio, "%s/saida/inicializacao.txt", argv);
    arquivoInicializacao = fopen(diretorio, "w");

    DesenhaGalinha(jogo.galinha, jogo.grade, jogo.personagensSprites);
    for(i = 0; i < jogo.quantidadePistas; i++) {
        DesenhaCarrosNaPista(jogo.pistas[i], jogo.grade, i, jogo.largura, jogo.animacoes, jogo.iteracao, jogo.personagensSprites);
    }

    for(i = 0; i < jogo.quantidadePistas*3 + 1; i++) {
        for(j = 0; j < jogo.largura + 2; j++) {
            fprintf(arquivoInicializacao, "%c", jogo.grade[i][j]);          //Mesmo codigo pra printar normal, mas trocando por fprintf
        }
        fprintf(arquivoInicializacao, "\n");
    }
    fprintf(arquivoInicializacao, "A posicao central da galinha iniciara em (%d %d).", ObtemXGalinha(jogo.galinha), ObtemYGalinha(jogo.galinha));
    fclose(arquivoInicializacao);
}

void GeraResumoTXT(tAtropelamentos atropelamentos[], int numAtropelamentos, int iteracaoFinal, char * argv) {
    int i;
    char diretorio[MAX_CAMINHO];
    FILE * arquivoResumo;
    sprintf(diretorio, "%s/saida/resumo.txt", argv);
    arquivoResumo = fopen(diretorio, "w");

    for(i = 0; i < numAtropelamentos; i++) {
        fprintf(arquivoResumo, "[%d] Na pista %d o carro %d atropelou a galinha na posicao (%d,%d).\n",atropelamentos[i].iteracao, atropelamentos[i].idPista, atropelamentos[i].idCarro, atropelamentos[i].x, atropelamentos[i].y);
    }
    fprintf(arquivoResumo, "[%d] Fim de jogo", iteracaoFinal);
    fclose(arquivoResumo);
}

void GeraRankingTXT(tAtropelamentos atropelamentos[], int numAtropelamentos, char * argv) {
    int i;
    char diretorio[MAX_CAMINHO];
    FILE * arquivoRanking;
    sprintf(diretorio, "%s/saida/ranking.txt", argv);
    arquivoRanking = fopen(diretorio, "w");

    OrdenaAtropelamentos(atropelamentos, numAtropelamentos);
    fprintf(arquivoRanking, "id_pista,id_carro,iteracao\n");
    for(i = 0; i < numAtropelamentos; i++) {
        fprintf(arquivoRanking, "%d,%d,%d\n", atropelamentos[i].idPista, atropelamentos[i].idCarro, atropelamentos[i].iteracao);
    }
    fclose(arquivoRanking);
}

void GeraEstatisticaTXT(int numMov, int numMovS, int yMax, int yMaxAtr, int yMinAtr, char * argv) {
    char diretorio[MAX_CAMINHO];
    FILE * arquivoEstatistica;
    sprintf(diretorio, "%s/saida/estatistica.txt", argv);
    arquivoEstatistica = fopen(diretorio, "w");

    fprintf(arquivoEstatistica, "Numero total de movimentos: %d\n", numMov);
    fprintf(arquivoEstatistica, "Altura maxima que a galinha chegou: %d\n", yMax);
    fprintf(arquivoEstatistica, "Altura maxima que a galinha foi atropelada: %d\n", yMaxAtr);
    fprintf(arquivoEstatistica, "Altura minima que a galinha foi atropelada: %d\n", yMinAtr);
    fprintf(arquivoEstatistica, "Numero de movimentos na direcao oposta: %d\n", numMovS);

    fclose(arquivoEstatistica);
}

void GeraHeatmapTXT(int linhas, int colunas, int heatmap[linhas][colunas], char * argv) {
    int i, j;
    char diretorio[MAX_CAMINHO];
    FILE * arquivoHeatmap;
    sprintf(diretorio, "%s/saida/heatmap.txt", argv);
    arquivoHeatmap = fopen(diretorio, "w");

    for(i = 0; i < linhas; i++) {
        for(j = 0; j < colunas; j++) {
            if(heatmap[i][j] == -1) {
                fprintf(arquivoHeatmap, " * ");
            }
            else {
                fprintf(arquivoHeatmap, "%2d ", heatmap[i][j]);
            }
        }
        fprintf(arquivoHeatmap, "\n");
    }
    fclose(arquivoHeatmap);
}


/*-----------------------------------------------Funcoes Jogo-----------------------------------------------*/


int AcabouJogo(tJogo jogo) {
    return(ObtemVidasGalinha(jogo.galinha) == 0 || ObtemYGalinha(jogo.galinha) == 1);
}

void DesenhaMapaJogo(tJogo jogo, int pontos) {  //Desenha e printa o mapa  //Como a funcao e void nao causa alteracao permanente no grid
    int i, j;

    DesenhaGalinha(jogo.galinha, jogo.grade, jogo.personagensSprites);
    for(i = 0; i < jogo.quantidadePistas; i++) {
        DesenhaCarrosNaPista(jogo.pistas[i], jogo.grade, i, jogo.largura, jogo.animacoes, jogo.iteracao, jogo.personagensSprites);
    }

    printf("Pontos: %d | Vidas: %d | Iteracoes: %d\n", pontos, ObtemVidasGalinha(jogo.galinha), jogo.iteracao);
    for(i = 0; i < jogo.quantidadePistas*3 + 1; i++) {       //Printa o grid
        for(j = 0; j < jogo.largura + 2; j++) {
            printf("%c", jogo.grade[i][j]);
        }
        printf("\n");
    }
}

void AtualizaHeatmap(tJogo jogo, int linhas, int colunas, int heatmap[linhas][colunas], int colisao) {
    int i, j;
    int x = ObtemXGalinha(jogo.galinha) - 1, y = ObtemYGalinha(jogo.galinha) - 1; // -1 porque comeca no 0, enquanto no jogo comeca em 1 
    if(colisao) {
        for(i = 0; i < colunas; i++) {
            heatmap[y][i] = -1;     //
            heatmap[y+1][i] = -1;
        }
    }
    else {
        for(i = 0; i <= 1; i++) {
            for(j = -1; j <= 1; j++) {
                if(heatmap[y+i][x+j] >= 0 && heatmap[y+i][x+j] < 99) {
                    heatmap[y+i][x+j]++;      //Incrementa o valor no retangulo 2x3 da galinha
                }
            }
        }
    }
}

void JogaJogo(tJogo jogo, char * argv) {
    int i, j, pistaGalinha, carroAtr;
    int pontos = 0;
    char movimento = 0;

    int linhasHeatmap = jogo.quantidadePistas*3 - 1, colunasHeatmap = jogo.largura;   //Desconsiderar as bordas
    int heatmap[linhasHeatmap][colunasHeatmap];
    for(i = 0; i < linhasHeatmap; i++) {
        for(j = 0; j < colunasHeatmap; j++) {
            heatmap[i][j] = 0;                      //Zera o heatmap
        }
    }
    AtualizaHeatmap(jogo, linhasHeatmap, colunasHeatmap, heatmap, 0); //Contar a posicao Inicial


    int numMov = 0, numMovS = 0, yMaxGalinha = 0, yMaxAtr = 0, yMinAtr = 0; //Numeros arbitrarios 
    int numAtr = 0;                                                             //Variaveis para estatistica
    tAtropelamentos atropelamentos[ObtemVidasGalinha(jogo.galinha)];

    DesenhaMapaJogo(jogo, pontos);
    while(!AcabouJogo(jogo)) {

        /*------------------------Movimento da Galinha e Carros------------------------*/
        scanf("%c", &movimento);
        if(movimento != 'w' && movimento != 's' && movimento != ' ') {
            continue;
        }
        switch(movimento) {           //Estatisticas
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
        if(carroAtr) {      //Se nao tiver colisao, retorna 0, do contrario retorna o numero do carro
            atropelamentos[numAtr] = AtribuiInfo(pistaGalinha+1, carroAtr, jogo.iteracao + 1, ObtemXGalinha(jogo.galinha), ObtemYGalinha(jogo.galinha)); //Pista + 1, pq pistaGalinha comeca em 0; Iteracao + 1, pq a iteracao adiciona no final
            numAtr++;
            AtualizaHeatmap(jogo, linhasHeatmap, colunasHeatmap, heatmap, 1);

            if((jogo.quantidadePistas*3 - ObtemYGalinha(jogo.galinha)) > yMaxAtr) { //Diferenca da altura do mapa(ponto 1 ate borda inferior) e da galinha
                yMaxAtr = jogo.quantidadePistas*3 - ObtemYGalinha(jogo.galinha);
            }
            if((jogo.quantidadePistas*3 - ObtemYGalinha(jogo.galinha)) < yMinAtr || yMinAtr == 0) { //Diferenca da altura do mapa(ponto 1 ate borda inferior) e da galinha
                yMinAtr = jogo.quantidadePistas*3 - ObtemYGalinha(jogo.galinha);
            }

            jogo.galinha = ResetaGalinha(jogo.galinha, jogo.quantidadePistas);
            pontos = 0;
            if(jogo.animacoes) {
                jogo.pistas[pistaGalinha] = ReduzVelocidadePista(jogo.pistas[pistaGalinha]);
            }
        }
        else if (!carroAtr && movimento == 'w') {
            pontos++;
        }

        /*---------------------------Estatisticas e Heatmap---------------------------*/
        if((jogo.quantidadePistas*3 - ObtemYGalinha(jogo.galinha)) > yMaxGalinha) { //Diferenca da altura do mapa(ponto 1 ate borda inferior) e da galinha
            yMaxGalinha = jogo.quantidadePistas*3 - ObtemYGalinha(jogo.galinha);
        }
        AtualizaHeatmap(jogo, linhasHeatmap, colunasHeatmap, heatmap, 0);

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

    GeraResumoTXT(atropelamentos, numAtr, jogo.iteracao, argv);
    GeraRankingTXT(atropelamentos, numAtr, argv);
    GeraEstatisticaTXT(numMov, numMovS, yMaxGalinha, yMaxAtr, yMinAtr, argv);
    GeraHeatmapTXT(linhasHeatmap, colunasHeatmap, heatmap, argv);
}


/*---------------------------------------------------MAIN---------------------------------------------------*/


int main(int argc, char * argv[]) {
    if(argc <= 1) {
        printf("ERRO: Informe o diretorio com os arquivos de configuracao.");
        return 1;
    }
    
    tJogo jogo;
    jogo = InicializaJogo(argv[1]);
    GeraInicializacaoTXT(jogo, argv[1]);
    JogaJogo(jogo, argv[1]);

    return 0;
}