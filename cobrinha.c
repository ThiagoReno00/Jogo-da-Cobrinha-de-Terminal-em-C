#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <windows.h>
#include <time.h>
#include <locale.h>
#include <direct.h>

#define LARGURA 80
#define ALTURA 20

typedef struct {
    char nome[50];
    int pontos;
    double tempo;
} Recorde;

typedef struct {
    int x, y;
} Obstaculo;

void posicao(int x, int y) {
    COORD coord = {x, y};
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void desenhaMapa() {
    for (int i = 0; i <= ALTURA; i++) {
        for (int j = 0; j <= LARGURA; j++) {
            if (i == 0 || i == ALTURA || j == 0 || j == LARGURA) printf("#");
            else printf(" ");
        }
        printf("\n");
    }
}

void desenhaCobrinha(int *x, int *y, int tamanho) {
    for (int i = 0; i < tamanho; i++) {
        posicao(x[i], y[i]);
        printf(i == 0 ? "O" : "o");
    }
}

void desenhaObstaculos(Obstaculo *obstaculos, int qtd) {
    for (int i = 0; i < qtd; i++) {
        posicao(obstaculos[i].x, obstaculos[i].y);
        printf("X");
    }
}

int verificaColisaoObstaculos(int x, int y, Obstaculo *obstaculos, int qtd) {
    for (int i = 0; i < qtd; i++) {
        if (x == obstaculos[i].x && y == obstaculos[i].y) return 1;
    }
    return 0;
}

char capturaTecla(char direcao) {
    if (kbhit()) {
        char tecla = getch();
        if ((tecla == 'w' || tecla == 'W') && direcao != 's') return 'w';
        if ((tecla == 's' || tecla == 'S') && direcao != 'w') return 's';
        if ((tecla == 'a' || tecla == 'A') && direcao != 'd') return 'a';
        if ((tecla == 'd' || tecla == 'D') && direcao != 'a') return 'd';
    }
    return direcao;
}

int verificaColisao(int *x, int *y, int tamanho) {
    for (int i = 1; i < tamanho; i++)
        if (x[0] == x[i] && y[0] == y[i]) return 1;
    return 0;
}

void posicionaMaca(int *mx, int *my, Obstaculo *obstaculos, int qtd, int *x, int *y, int tamanho) {
    int posicaoInvalida;
    do {
        *mx = rand() % (LARGURA - 2) + 1;
        *my = rand() % (ALTURA - 2) + 1;
        posicaoInvalida = verificaColisaoObstaculos(*mx, *my, obstaculos, qtd);
        for (int i = 0; i < tamanho && !posicaoInvalida; i++) {
            if (*mx == x[i] && *my == y[i]) {
                posicaoInvalida = 1;
            }
        }
    } while (posicaoInvalida);
    posicao(*mx, *my);
    printf("@");
}

int carregarObstaculosDeArquivo(const char *arquivo, Obstaculo **obstaculos, int *qtd) {
    FILE *file = fopen(arquivo, "r");
    if (!file) {
        perror("Erro ao abrir o arquivo");
        return 0;
    }

    int x, y, count = 0;
    Obstaculo *temp = NULL;

    while (fscanf(file, "%d %d", &x, &y) == 2) {
        count++;
        temp = realloc(temp, count * sizeof(Obstaculo));
        if (!temp) {
            printf("Erro de memoria ao carregar obstáculos.\n");
            fclose(file);
            return 0;
        }
        temp[count - 1].x = x;
        temp[count - 1].y = y;
    }

    fclose(file);

    *obstaculos = temp;
    *qtd = count;

    return 1;
}

void salvarRecordes(Recorde recordes[], int qtd) {
    FILE *arquivo = fopen("recordes.bin", "wb");
    if (arquivo != NULL) {
        fwrite(&qtd, sizeof(int), 1, arquivo);
        fwrite(recordes, sizeof(Recorde), qtd, arquivo);
        fclose(arquivo);
    } else {
        printf("Erro ao salvar os recordes.\n");
    }
}

int carregarRecordes(Recorde **recordes) {
    FILE *arquivo = fopen("recordes.bin", "rb");
    if (arquivo != NULL) {
        int qtd;
        fread(&qtd, sizeof(int), 1, arquivo);

        *recordes = malloc(qtd * sizeof(Recorde));
        fread(*recordes, sizeof(Recorde), qtd, arquivo);
        fclose(arquivo);

        return qtd;
    }
    return 0;
}

void ordenarRecordes(Recorde *recordes, int qtd) {
    for (int i = 0; i < qtd - 1; i++) {
        for (int j = i + 1; j < qtd; j++) {
            if (recordes[i].pontos < recordes[j].pontos ||
               (recordes[i].pontos == recordes[j].pontos && recordes[i].tempo > recordes[j].tempo)) {
                Recorde temp = recordes[i];
                recordes[i] = recordes[j];
                recordes[j] = temp;
            }
        }
    }
}

void exibirRecordes() {
    system("cls");

    Recorde *recordes = NULL;
    int qtd = carregarRecordes(&recordes);

    printf("==== Estatísticas ====\n\n");
    if (qtd == 0) {
        printf("Nenhum recorde encontrado.\n");
    } else {
        ordenarRecordes(recordes, qtd);

        for (int i = 0; i < qtd; i++) {
            printf("%d. %s - Pontos: %d - Tempo: %.2f segundos\n",
                   i + 1, recordes[i].nome, recordes[i].pontos, recordes[i].tempo);
        }
    }
    free(recordes);
    printf("\nPressione X para voltar ao menu principal...");
    while (getch() != 'x' && getch() != 'X');
}

void mensagemGameOver() {
    posicao(25, 10);
    printf("==============================");
    posicao(30, 11);
    printf("G A M E  O V E R");
    posicao(25, 12);
    printf("==============================");
}

void mensagemYouWin() {
    posicao(25, 10);
    printf("==============================");
    posicao(30, 11);
    printf("Y O U    W I N !");
    posicao(25, 12);
    printf("==============================");
}

void exibirComoJogar() {
    system("cls");
    printf("==== Como Jogar ====\n\n");
    printf("- Use as teclas W, A, S e D para mover a cobrinha para cima, esquerda, baixo e direita, respectivamente.\n");
    printf("- A cada 5 maçãs, você avança de fase aumentando sua pontuação e o tamanho da cobrinha.\n");
    printf("- Vença o jogo completando as 5 fases sem colidir com as paredes, o próprio corpo ou obstáculos (X).\n");
    printf("- Depois de jogar veja sua posição no ranking de Estatísticas! Boa sorte!\n\n");
    printf("Pressione X para voltar ao menu principal...");
    while (getch() != 'x' && getch() != 'X');
}
void menuPrincipal() {
    int opcao;
    while (1) {
        system("cls");
        printf("==== Jogo da Cobrinha ====\n\n");
        printf("1. Iniciar Jogo\n");
        printf("2. Como Jogar\n");
        printf("3. Estatísticas\n");
        printf("4. Sair\n\n");
        printf("Escolha uma opção: ");
        scanf("%d", &opcao);

        if (opcao == 1) return;
        else if (opcao == 2) exibirComoJogar();
        else if (opcao == 3) exibirRecordes();
        else if (opcao == 4) exit(0);
        else printf("Opção invalida! Tente novamente.\n");
    }
}

double calcularTempo(clock_t inicio) {
    return ((double)(clock() - inicio)) / CLOCKS_PER_SEC;
}

int main() {
    setlocale(LC_ALL, "Portuguese");
    srand(time(NULL));

    int *x = NULL, *y = NULL;
    int tamanho, pontos, mx, my, fase, qtdObstaculos;
    char direcao, nome[50];
    Obstaculo *obstaculos = NULL;

    while (1) {
        menuPrincipal();

        tamanho = 5;
        pontos = 0;
        fase = 1;
        direcao = 'd';
        qtdObstaculos = 0;

        x = malloc(tamanho * sizeof(int));
        y = malloc(tamanho * sizeof(int));

        for (int i = 0; i < tamanho; i++) {
            x[i] = LARGURA / 2 - i;
            y[i] = ALTURA / 2;
        }

        system("cls");
        printf("Digite seu nome: ");
        scanf("%s", nome);

        clock_t inicio = clock();

        system("cls");
        desenhaMapa();

        char arquivoFase[20];
        snprintf(arquivoFase, sizeof(arquivoFase), "fase%d.txt", fase);
        if (!carregarObstaculosDeArquivo(arquivoFase, &obstaculos, &qtdObstaculos)) {
            printf("Erro ao carregar obstaculos da fase 1.\n");
            break;
        }

        desenhaObstaculos(obstaculos, qtdObstaculos);
        posicionaMaca(&mx, &my, obstaculos, qtdObstaculos, x, y, tamanho);

        while (1) {
            double tempoDecorrido = calcularTempo(inicio);

            posicao(x[tamanho - 1], y[tamanho - 1]);
            printf(" ");
            for (int i = tamanho - 1; i > 0; i--) {
                x[i] = x[i - 1];
                y[i] = y[i - 1];
            }

            if (direcao == 'w') y[0]--;
            if (direcao == 's') y[0]++;
            if (direcao == 'a') x[0]--;
            if (direcao == 'd') x[0]++;

            if (x[0] == 0 || x[0] == LARGURA || y[0] == 0 || y[0] == ALTURA || verificaColisao(x, y, tamanho) || verificaColisaoObstaculos(x[0], y[0], obstaculos, qtdObstaculos)) break;

            if (x[0] == mx && y[0] == my) {
                tamanho++;
                x = realloc(x, tamanho * sizeof(int));
                y = realloc(y, tamanho * sizeof(int));
                x[tamanho - 1] = x[tamanho - 2];
                y[tamanho - 1] = y[tamanho - 2];

                pontos++;
                if (pontos % 5 == 0) {
                    fase++;
                    if (fase > 5) break;

                    snprintf(arquivoFase, sizeof(arquivoFase), "fase%d.txt", fase);
                    if (!carregarObstaculosDeArquivo(arquivoFase, &obstaculos, &qtdObstaculos)) {
                        printf("Erro ao carregar obstaculos da fase %d.\n", fase);
                        break;
                    }

                    system("cls");
                    desenhaMapa();
                    desenhaObstaculos(obstaculos, qtdObstaculos);
                }
                posicionaMaca(&mx, &my, obstaculos, qtdObstaculos, x, y, tamanho);
            }

            desenhaCobrinha(x, y, tamanho);

            posicao((LARGURA / 2) - 20, ALTURA + 2);
            printf("| Fase: %d | Pontos: %d | Tempo: %.2f segundos |", fase, pontos, tempoDecorrido);

            posicao(0, ALTURA + 15);

            direcao = capturaTecla(direcao);
            Sleep(100);
        }

        double tempoFinal = calcularTempo(inicio);

        if (fase > 5) {
            system("cls");
            mensagemYouWin();
            posicao(25, 14);
            printf("Parabéns, %s! Você venceu o jogo!\n", nome);
        } else {
            system("cls");
            mensagemGameOver();
            posicao(25, 14);
            printf("Pontuação final: %d | Tempo: %.2f segundos\n", pontos, tempoFinal);
        }

        Recorde novo = {.pontos = pontos, .tempo = tempoFinal};
        strcpy(novo.nome, nome);

        Recorde *recordes = NULL;
        int qtd = carregarRecordes(&recordes);

        recordes = realloc(recordes, (qtd + 1) * sizeof(Recorde));
        recordes[qtd++] = novo;

        salvarRecordes(recordes, qtd);
        free(recordes);

        free(x);
        free(y);
        free(obstaculos);

        printf("\nPressione X para voltar ao menu principal...");
        while (getch() != 'x' && getch() != 'X');
    }
    return 0;
}