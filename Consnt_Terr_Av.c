#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Estrutura que guarda as informações de um território
struct Territorio {
    char nome[30];
    char cor[10];   // cor do exército (dono do território)
    int tropas;     // quantidade de tropas no território
};

typedef struct Territorio Territorio;

// Protótipos das funções
void cadastrarTerritorios(Territorio *mapa, int qtd);
void exibirTerritorios(Territorio *mapa, int qtd);
void atacar(Territorio *atacante, Territorio *defensor);
void liberarMemoria(Territorio *mapa);

int main() {
    Territorio *mapa = NULL;
    int qtdTerritorios;
    int opcao;
    int idxAtacante, idxDefensor;

    // Semente para gerar números aleatórios diferentes a cada execução
    srand(time(NULL));

    printf("=== Sistema de Territorios - Nivel Aventureiro ===\n\n");

    // Pergunta a quantidade de territórios e faz alocação dinâmica
    printf("Informe a quantidade de territorios que deseja cadastrar: ");
    if (scanf("%d", &qtdTerritorios) != 1 || qtdTerritorios <= 0) {
        printf("Quantidade invalida. Encerrando o programa.\n");
        return 1;
    }

    // Consome o \n deixado pelo scanf
    getchar();

    // Alocação dinâmica usando calloc (inicializa com zero)
    mapa = (Territorio *)calloc(qtdTerritorios, sizeof(Territorio));
    if (mapa == NULL) {
        printf("Erro ao alocar memoria.\n");
        return 1;
    }

    // Cadastro dos territórios
    cadastrarTerritorios(mapa, qtdTerritorios);

    do {
        printf("\n=== Menu de Acoes ===\n");
        printf("1 - Exibir territorios\n");
        printf("2 - Realizar ataque\n");
        printf("0 - Sair\n");
        printf("Escolha uma opcao: ");
        if (scanf("%d", &opcao) != 1) {
            printf("Opcao invalida.\n");
            // limpando buffer e forçando saída
            break;
        }

        getchar(); // limpa o \n

        switch (opcao) {
            case 1:
                exibirTerritorios(mapa, qtdTerritorios);
                break;

            case 2:
                exibirTerritorios(mapa, qtdTerritorios);
                printf("\nEscolha o numero do territorio ATACANTE: ");
                scanf("%d", &idxAtacante);
                printf("Escolha o numero do territorio DEFENSOR: ");
                scanf("%d", &idxDefensor);
                getchar(); // limpa o \n

                // Ajusta índices para base 0
                idxAtacante--;
                idxDefensor--;

                // Valida índices
                if (idxAtacante < 0 || idxAtacante >= qtdTerritorios ||
                    idxDefensor < 0 || idxDefensor >= qtdTerritorios) {
                    printf("Indice de territorio invalido! Tente novamente.\n");
                    break;
                }

                // Não pode atacar a si mesmo
                if (idxAtacante == idxDefensor) {
                    printf("Um territorio nao pode atacar a si mesmo!\n");
                    break;
                }

                // Não pode atacar território da mesma cor
                if (strcmp(mapa[idxAtacante].cor, mapa[idxDefensor].cor) == 0) {
                    printf("Nao e permitido atacar um territorio da mesma cor (mesmo dono).\n");
                    break;
                }

                // Verifica se o atacante tem tropas suficientes
                if (mapa[idxAtacante].tropas <= 0) {
                    printf("O territorio atacante nao possui tropas suficientes para atacar.\n");
                    break;
                }

                // Chama a função de ataque passando ponteiros
                atacar(&mapa[idxAtacante], &mapa[idxDefensor]);

                // Exibe o resultado após o ataque
                exibirTerritorios(mapa, qtdTerritorios);
                break;

            case 0:
                printf("Encerrando o programa...\n");
                break;

            default:
                printf("Opcao invalida. Tente novamente.\n");
        }

    } while (opcao != 0);

    // Liberação da memória alocada
    liberarMemoria(mapa);

    return 0;
}

// Função responsável por cadastrar os territórios
void cadastrarTerritorios(Territorio *mapa, int qtd) {
    int i;
    printf("\n=== Cadastro de Territorios ===\n");

    for (i = 0; i < qtd; i++) {
        printf("\nTerritorio %d:\n", i + 1);

        printf("Digite o nome: ");
        fgets(mapa[i].nome, sizeof(mapa[i].nome), stdin);
        mapa[i].nome[strcspn(mapa[i].nome, "\n")] = '\0'; // remove o \n

        printf("Digite a cor do exercito: ");
        fgets(mapa[i].cor, sizeof(mapa[i].cor), stdin);
        mapa[i].cor[strcspn(mapa[i].cor, "\n")] = '\0';

        printf("Digite a quantidade de tropas: ");
        scanf("%d", &mapa[i].tropas);
        getchar(); // limpa o \n que sobrou do scanf
    }
}

// Função que exibe todos os territórios cadastrados
void exibirTerritorios(Territorio *mapa, int qtd) {
    int i;
    printf("\n=== Territorios Cadastrados ===\n");
    for (i = 0; i < qtd; i++) {
        printf("\nTerritorio %d\n", i + 1);
        printf("Nome  : %s\n", mapa[i].nome);
        printf("Cor   : %s\n", mapa[i].cor);
        printf("Tropas: %d\n", mapa[i].tropas);
    }
}

// Função que simula um ataque entre dois territórios
void atacar(Territorio *atacante, Territorio *defensor) {
    int dadoAtacante = (rand() % 6) + 1; // valor entre 1 e 6
    int dadoDefensor = (rand() % 6) + 1; // valor entre 1 e 6

    printf("\n=== Ataque em andamento ===\n");
    printf("Atacante: %s (%s) - Tropas: %d\n",
           atacante->nome, atacante->cor, atacante->tropas);
    printf("Defensor: %s (%s) - Tropas: %d\n",
           defensor->nome, defensor->cor, defensor->tropas);

    printf("\nRolando os dados...\n");
    printf("Dado do atacante: %d\n", dadoAtacante);
    printf("Dado do defensor: %d\n", dadoDefensor);

    if (dadoAtacante > dadoDefensor) {
        // Atacante vence
        printf("\nResultado: O atacante venceu a batalha!\n");

        // Transfere a cor (dono) do atacante para o defensor
        strcpy(defensor->cor, atacante->cor);

        // Transfere metade das tropas do atacante para o território conquistado
        int tropasTransferidas = atacante->tropas / 2;
        if (tropasTransferidas < 1 && atacante->tropas > 0) {
            tropasTransferidas = 1; // garante pelo menos 1 tropa transferida
        }

        atacante->tropas -= tropasTransferidas;
        defensor->tropas = tropasTransferidas;

        printf("O territorio %s foi conquistado!\n", defensor->nome);
        printf("Tropas transferidas: %d\n", tropasTransferidas);
    } else {
        // Defensor vence (ou empate considerado a favor do defensor)
        printf("\nResultado: O defensor resistiu ao ataque!\n");

        if (atacante->tropas > 0) {
            atacante->tropas -= 1; // atacante perde 1 tropa
        }

        printf("O atacante perdeu 1 tropa.\n");
    }
}

// Função para liberar a memória do vetor de territórios
void liberarMemoria(Territorio *mapa) {
    if (mapa != NULL) {
        free(mapa);
    }
}
