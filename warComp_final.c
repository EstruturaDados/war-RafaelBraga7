#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Estrutura que guarda as informacoes de um territorio
typedef struct {
    char nome[30];
    char cor[10];   // cor do exercito (dono do territorio)
    int tropas;     // quantidade de tropas no territorio
} Territorio;

// Estrutura simples para jogadores
typedef struct {
    char nome[30];
    char cor[10];   // cor principal do jogador (ex: "vermelho", "azul")
    char *missao;   // missao armazenada dinamicamente
} Jogador;

// Tamanho maximo de uma descricao de missao
#define TAM_MISSAO 120

// Vetor de missoes pre-definidas
// Obs: a logica de verificacao foi feita para estas descricoes EXATAS
char *listaMissoes[] = {
    "Destruir totalmente as tropas do jogador de cor Azul",
    "Destruir totalmente as tropas do jogador de cor Vermelha",
    "Destruir totalmente as tropas do jogador de cor Verde",
    "Destruir totalmente as tropas do jogador de cor Amarela",
    "Destruir totalmente as tropas do jogador de cor Preta",
    "Conquistar a Asia e a America do Sul",
    "Conquistar a Europa e a Oceania",
    "Conquistar a Asia e a Africa",
    "Conquistar a Europa e a America do Sul",
    "Conquistar a America do Norte e a Africa",
    "Conquistar 24 territorios ocupando cada um com pelo menos 2 exercitos",
    "Conquistar 18 territorios ocupando cada um com pelo menos 2 exercitos"
};

int totalMissoes = sizeof(listaMissoes) / sizeof(listaMissoes[0]);

/* === Protótipos === */
void cadastrarTerritorios(Territorio *mapa, int qtd);
void exibirMapa(Territorio *mapa, int qtd);
void atacar(Territorio *atacante, Territorio *defensor);

void atribuirMissao(char *destino, char *missoes[], int totalMissoes);
void exibirMissao(const Jogador *jogador);
int verificarMissao(char *missao, Territorio *mapa, int tamanho);

void liberarMemoria(Territorio *mapa, Jogador *jogadores, int qtdJogadores);

int main() {
    Territorio *mapa = NULL;
    Jogador *jogadores = NULL;
    int qtdTerritorios;
    int qtdJogadores;
    int opcao;
    int idxAtacante, idxDefensor;
    int i;
    int vencedor = -1; // -1 indica que ainda nao ha vencedor

    // Semente para gerar numeros aleatorios diferentes a cada execucao
    srand((unsigned int)time(NULL));

    printf("=== Sistema de Territorios com Missoes Estrategicas ===\n\n");

    /* -------------------------
       Cadastro de jogadores
       ------------------------- */
    printf("Informe a quantidade de jogadores: ");
    if (scanf("%d", &qtdJogadores) != 1 || qtdJogadores <= 0) {
        printf("Quantidade de jogadores invalida. Encerrando o programa.\n");
        return 1;
    }
    getchar(); // consome o \n

    jogadores = (Jogador *)malloc(qtdJogadores * sizeof(Jogador));
    if (jogadores == NULL) {
        printf("Erro ao alocar memoria para jogadores.\n");
        return 1;
    }

    for (i = 0; i < qtdJogadores; i++) {
        printf("\n=== Cadastro do Jogador %d ===\n", i + 1);

        printf("Nome do jogador: ");
        fgets(jogadores[i].nome, sizeof(jogadores[i].nome), stdin);
        jogadores[i].nome[strcspn(jogadores[i].nome, "\n")] = '\0';

        printf("Cor do exercito do jogador (ex: vermelho, azul): ");
        fgets(jogadores[i].cor, sizeof(jogadores[i].cor), stdin);
        jogadores[i].cor[strcspn(jogadores[i].cor, "\n")] = '\0';

        // Aloca memoria para a missao deste jogador
        jogadores[i].missao = (char *)malloc(TAM_MISSAO * sizeof(char));
        if (jogadores[i].missao == NULL) {
            printf("Erro ao alocar memoria para missao do jogador.\n");
            // Libera o que ja foi alocado e encerra
            liberarMemoria(NULL, jogadores, i);
            return 1;
        }

        // Atribui uma missao aleatoria
        atribuirMissao(jogadores[i].missao, listaMissoes, totalMissoes);

        // Exibe a missao do jogador APENAS UMA VEZ (no inicio)
        exibirMissao(&jogadores[i]);
    }

    /* -------------------------
       Cadastro de territorios
       ------------------------- */
    printf("\nInforme a quantidade de territorios que deseja cadastrar: ");
    if (scanf("%d", &qtdTerritorios) != 1 || qtdTerritorios <= 0) {
        printf("Quantidade invalida de territorios. Encerrando o programa.\n");
        liberarMemoria(NULL, jogadores, qtdJogadores);
        return 1;
    }

    getchar(); // consome o \n

    // Alocacao dinamica usando calloc (inicializa com zero)
    mapa = (Territorio *)calloc(qtdTerritorios, sizeof(Territorio));
    if (mapa == NULL) {
        printf("Erro ao alocar memoria para territorios.\n");
        liberarMemoria(NULL, jogadores, qtdJogadores);
        return 1;
    }

    cadastrarTerritorios(mapa, qtdTerritorios);

    /* -------------------------
       Loop principal do jogo
       ------------------------- */
    do {
        printf("\n=== Menu de Acoes ===\n");
        printf("1 - Exibir mapa de territorios\n");
        printf("2 - Realizar ataque\n");
        printf("0 - Sair\n");
        printf("Escolha uma opcao: ");

        if (scanf("%d", &opcao) != 1) {
            printf("Opcao invalida.\n");
            break;
        }

        getchar(); // limpa o \n

        switch (opcao) {
            case 1:
                exibirMapa(mapa, qtdTerritorios);
                break;

            case 2:
                exibirMapa(mapa, qtdTerritorios);

                printf("\nEscolha o numero do territorio ATACANTE: ");
                scanf("%d", &idxAtacante);
                printf("Escolha o numero do territorio DEFENSOR: ");
                scanf("%d", &idxDefensor);
                getchar(); // limpa o \n

                // Ajusta indices para base 0
                idxAtacante--;
                idxDefensor--;

                // Valida indices
                if (idxAtacante < 0 || idxAtacante >= qtdTerritorios ||
                    idxDefensor < 0 || idxDefensor >= qtdTerritorios) {
                    printf("Indice de territorio invalido! Tente novamente.\n");
                    break;
                }

                // Nao pode atacar a si mesmo
                if (idxAtacante == idxDefensor) {
                    printf("Um territorio nao pode atacar a si mesmo!\n");
                    break;
                }

                // Nao pode atacar territorio da mesma cor
                if (strcmp(mapa[idxAtacante].cor, mapa[idxDefensor].cor) == 0) {
                    printf("Nao e permitido atacar um territorio da mesma cor (mesmo dono).\n");
                    break;
                }

                // Verifica se o atacante tem tropas suficientes
                if (mapa[idxAtacante].tropas <= 0) {
                    printf("O territorio atacante nao possui tropas suficientes para atacar.\n");
                    break;
                }

                // Realiza o ataque
                atacar(&mapa[idxAtacante], &mapa[idxDefensor]);

                // Exibe o resultado apos o ataque
                exibirMapa(mapa, qtdTerritorios);

                // Ao final do turno, verifica silenciosamente se algum jogador venceu
                for (i = 0; i < qtdJogadores; i++) {
                    if (verificarMissao(jogadores[i].missao, mapa, qtdTerritorios)) {
                        vencedor = i;
                        printf("\n==================================\n");
                        printf("      FIM DE JOGO - VENCEDOR!     \n");
                        printf("O jogador %s cumpriu sua missao:\n", jogadores[i].nome);
                        printf("\"%s\"\n", jogadores[i].missao);
                        printf("==================================\n");
                        break;
                    }
                }

                break;

            case 0:
                printf("Encerrando o programa...\n");
                break;

            default:
                printf("Opcao invalida. Tente novamente.\n");
        }

        // Se alguem venceu, sai do loop principal
        if (vencedor != -1) {
            break;
        }

    } while (opcao != 0);

    // Liberacao da memoria
    liberarMemoria(mapa, jogadores, qtdJogadores);

    return 0;
}

/* ============================================================
   Implementacao das funcoes
   ============================================================ */

// Cadastra os territorios
void cadastrarTerritorios(Territorio *mapa, int qtd) {
    int i;
    printf("\n=== Cadastro de Territorios ===\n");

    for (i = 0; i < qtd; i++) {
        printf("\nTerritorio %d:\n", i + 1);

        printf("Digite o nome do territorio: ");
        fgets(mapa[i].nome, sizeof(mapa[i].nome), stdin);
        mapa[i].nome[strcspn(mapa[i].nome, "\n")] = '\0'; // remove o \n

        printf("Digite a cor do exercito dono do territorio: ");
        fgets(mapa[i].cor, sizeof(mapa[i].cor), stdin);
        mapa[i].cor[strcspn(mapa[i].cor, "\n")] = '\0';

        printf("Digite a quantidade de tropas: ");
        scanf("%d", &mapa[i].tropas);
        getchar(); // limpa o \n
    }
}

// Exibe todos os territorios
void exibirMapa(Territorio *mapa, int qtd) {
    int i;
    printf("\n=== Mapa de Territorios ===\n");
    for (i = 0; i < qtd; i++) {
        printf("\nTerritorio %d\n", i + 1);
        printf("Nome  : %s\n", mapa[i].nome);
        printf("Cor   : %s\n", mapa[i].cor);
        printf("Tropas: %d\n", mapa[i].tropas);
    }
}

// Simula um ataque entre dois territorios
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

        // Transfere metade das tropas do atacante para o territorio conquistado
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

/* ---------- Missoes ---------- */

// Sorteia uma missao e copia para o destino (ja alocado)
void atribuirMissao(char *destino, char *missoes[], int totalMissoes) {
    int indice = rand() % totalMissoes;
    strcpy(destino, missoes[indice]);
}

// Exibe a missao do jogador (apenas no inicio)
void exibirMissao(const Jogador *jogador) {
    printf("\n==================================\n");
    printf("Jogador: %s\n", jogador->nome);
    printf("Cor    : %s\n", jogador->cor);
    printf("Sua missao secreta e:\n");
    printf("\"%s\"\n", jogador->missao);
    printf("==================================\n");
}

/*
    verificarMissao:
    Recebe o texto da missao e o mapa de territorios e verifica,
    com uma logica simples, se a condicao foi atingida.
*/
int verificarMissao(char *missao, Territorio *mapa, int tamanho) {
    int i;
    int count;
    int somaTropas;
    int maxTropas;
    int todosComUmaTropa;

    // Missao 1: Conquistar pelo menos 3 territorios (com tropas > 0)
    if (strcmp(missao, listaMissoes[0]) == 0) {
        count = 0;
        for (i = 0; i < tamanho; i++) {
            if (mapa[i].tropas > 0) {
                count++;
            }
        }
        return (count >= 3);
    }
    // Missao 2: Ter pelo menos 10 tropas somadas
    else if (strcmp(missao, listaMissoes[1]) == 0) {
        somaTropas = 0;
        for (i = 0; i < tamanho; i++) {
            somaTropas += mapa[i].tropas;
        }
        return (somaTropas >= 10);
    }
    // Missao 3: Ter pelo menos 8 tropas em um unico territorio
    else if (strcmp(missao, listaMissoes[2]) == 0) {
        maxTropas = 0;
        for (i = 0; i < tamanho; i++) {
            if (mapa[i].tropas > maxTropas) {
                maxTropas = mapa[i].tropas;
            }
        }
        return (maxTropas >= 8);
    }
    // Missao 4: Todos os territorios com pelo menos 1 tropa
    else if (strcmp(missao, listaMissoes[3]) == 0) {
        todosComUmaTropa = 1;
        for (i = 0; i < tamanho; i++) {
            if (mapa[i].tropas < 1) {
                todosComUmaTropa = 0;
                break;
            }
        }
        return todosComUmaTropa;
    }
    // Missao 5: Ter ao menos 4 territorios cadastrados
    else if (strcmp(missao, listaMissoes[4]) == 0) {
        return (tamanho >= 4);
    }

    // Caso a missao nao seja reconhecida, considera ainda nao cumprida
    return 0;
}

/* ---------- Liberacao de memoria ---------- */

void liberarMemoria(Territorio *mapa, Jogador *jogadores, int qtdJogadores) {
    int i;

    if (mapa != NULL) {
        free(mapa);
    }

    if (jogadores != NULL) {
        for (i = 0; i < qtdJogadores; i++) {
            if (jogadores[i].missao != NULL) {
                free(jogadores[i].missao);
            }
        }
        free(jogadores);
    }
}
