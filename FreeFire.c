/*
 * torre_fuga.c
 *
 * Módulo de priorização e montagem da torre de fuga
 *
 * Funcionalidades:
 *  - Cadastro de até 20 componentes (nome, tipo, prioridade)
 *  - Bubble sort por nome (alfabético crescente) com contagem de comparações e tempo
 *  - Insertion sort por tipo (alfabético crescente) com contagem de comparações e tempo
 *  - Selection sort por prioridade (decrescente: maior prioridade primeiro) com contagem de comparações e tempo
 *  - Busca binária por nome (após ordenação por nome) com contagem de comparações
 *  - Menu interativo e exibição de métricas
 *
 * Observações:
 *  - Usa fgets para captura segura de strings
 *  - Utiliza clock() para medir tempo de execução
 *  - Implementa comparação de strings case-insensitive local (stricmp)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#define MAX_COMPONENTES 20
#define MAX_NOME 30
#define MAX_TIPO 20

typedef struct {
    char nome[MAX_NOME];
    char tipo[MAX_TIPO];
    int prioridade; /* 1 (menor) .. 10 (maior) */
} Componente;

/* ---------------- utilitários ---------------- */

/* remove newline no final da string (se presente) */
void trim_newline(char *s) {
    if (!s) return;
    size_t len = strlen(s);
    if (len == 0) return;
    if (s[len-1] == '\n') s[len-1] = '\0';
}

/* compara strings case-insensitive; retorna valor similar a strcmp */
int stricmp_local(const char *a, const char *b) {
    int ia = 0, ib = 0;
    while (a[ia] != '\0' && b[ib] != '\0') {
        char ca = tolower((unsigned char)a[ia]);
        char cb = tolower((unsigned char)b[ib]);
        if (ca != cb) return (ca < cb) ? -1 : 1;
        ia++; ib++;
    }
    if (a[ia] == '\0' && b[ib] == '\0') return 0;
    return (a[ia] == '\0') ? -1 : 1;
}

/* exibe vetor de componentes */
void mostrarComponentes(const Componente arr[], int n) {
    printf("\n--- Componentes (total: %d) ---\n", n);
    if (n == 0) {
        printf("[vazio]\n");
        return;
    }
    printf("%-3s | %-28s | %-15s | %s\n", "ID", "NOME", "TIPO", "PRIORIDADE");
    printf("----+------------------------------+-----------------+----------\n");
    for (int i = 0; i < n; ++i) {
        printf("%-3d | %-28s | %-15s | %-8d\n", i+1, arr[i].nome, arr[i].tipo, arr[i].prioridade);
    }
}

/* copia array (útil para testar/medir sem alterar original se necessário) */
void copiarComponentes(const Componente src[], Componente dst[], int n) {
    for (int i = 0; i < n; ++i) dst[i] = src[i];
}

/* ---------------- algoritmos de ordenação com métricas ---------------- */

/*
 * Bubble Sort por nome (alfabético crescente)
 * Retorna o número de comparações em *comparacoes e tempo em segundos em *tempoSeg.
 */
void bubbleSortNome(Componente arr[], int n, unsigned long long *comparacoes, double *tempoSeg) {
    *comparacoes = 0;
    clock_t t0 = clock();

    int trocou;
    for (int pass = 0; pass < n-1; ++pass) {
        trocou = 0;
        for (int i = 0; i < n-1-pass; ++i) {
            (*comparacoes)++;
            if (stricmp_local(arr[i].nome, arr[i+1].nome) > 0) {
                /* troca */
                Componente tmp = arr[i];
                arr[i] = arr[i+1];
                arr[i+1] = tmp;
                trocou = 1;
            }
        }
        if (!trocou) break; /* otimização: se já ordenado */
    }

    clock_t t1 = clock();
    *tempoSeg = (double)(t1 - t0) / CLOCKS_PER_SEC;
}

/*
 * Insertion Sort por tipo (alfabético crescente)
 */
void insertionSortTipo(Componente arr[], int n, unsigned long long *comparacoes, double *tempoSeg) {
    *comparacoes = 0;
    clock_t t0 = clock();

    for (int i = 1; i < n; ++i) {
        Componente key = arr[i];
        int j = i - 1;
        /* comparar tipos */
        while (j >= 0) {
            (*comparacoes)++;
            if (stricmp_local(arr[j].tipo, key.tipo) > 0) {
                arr[j+1] = arr[j];
                j--;
            } else {
                break;
            }
        }
        arr[j+1] = key;
    }

    clock_t t1 = clock();
    *tempoSeg = (double)(t1 - t0) / CLOCKS_PER_SEC;
}

/*
 * Selection Sort por prioridade (decrescente: maior prioridade primeiro)
 */
void selectionSortPrioridade(Componente arr[], int n, unsigned long long *comparacoes, double *tempoSeg) {
    *comparacoes = 0;
    clock_t t0 = clock();

    for (int i = 0; i < n-1; ++i) {
        int idxMax = i;
        for (int j = i+1; j < n; ++j) {
            (*comparacoes)++;
            if (arr[j].prioridade > arr[idxMax].prioridade) {
                idxMax = j;
            }
        }
        if (idxMax != i) {
            Componente tmp = arr[i];
            arr[i] = arr[idxMax];
            arr[idxMax] = tmp;
        }
    }

    clock_t t1 = clock();
    *tempoSeg = (double)(t1 - t0) / CLOCKS_PER_SEC;
}

/* ---------------- busca binária por nome (após ordenação por nome) ---------------- */
/*
 * Retorna índice do componente encontrado ou -1 se não achar.
 * Também preenche comparacoesBusca com o número de comparações feitas.
 * Utiliza comparação case-insensitive (stricmp_local).
 */
int buscaBinariaPorNome(const Componente arr[], int n, const char chave[], unsigned long long *comparacoesBusca) {
    int left = 0, right = n - 1;
    *comparacoesBusca = 0;
    while (left <= right) {
        int mid = left + (right - left) / 2;
        (*comparacoesBusca)++;
        int cmp = stricmp_local(arr[mid].nome, chave);
        if (cmp == 0) return mid;
        if (cmp < 0) left = mid + 1;
        else right = mid - 1;
    }
    return -1;
}

/* ---------------- entrada de dados ---------------- */

void cadastrarComponentes(Componente arr[], int *n) {
    char buffer[128];
    int quantidade;
    *n = 0;

    printf("\nQuantos componentes deseja cadastrar? (1-%d): ", MAX_COMPONENTES);
    if (fgets(buffer, sizeof(buffer), stdin) == NULL) return;
    if (sscanf(buffer, "%d", &quantidade) != 1 || quantidade < 1) {
        printf("Entrada inválida. Abortando cadastro.\n");
        return;
    }
    if (quantidade > MAX_COMPONENTES) quantidade = MAX_COMPONENTES;

    for (int i = 0; i < quantidade; ++i) {
        printf("\n--- Componente %d ---\n", i+1);

        printf("Nome: ");
        if (fgets(arr[i].nome, MAX_NOME, stdin) == NULL) arr[i].nome[0] = '\0';
        trim_newline(arr[i].nome);
        if (strlen(arr[i].nome) == 0) {
            strncpy(arr[i].nome, "SEM_NOME", MAX_NOME-1);
            arr[i].nome[MAX_NOME-1] = '\0';
        }

        printf("Tipo (ex: controle, suporte, propulsao): ");
        if (fgets(arr[i].tipo, MAX_TIPO, stdin) == NULL) arr[i].tipo[0] = '\0';
        trim_newline(arr[i].tipo);
        if (strlen(arr[i].tipo) == 0) {
            strncpy(arr[i].tipo, "GENERIC", MAX_TIPO-1);
            arr[i].tipo[MAX_TIPO-1] = '\0';
        }

        int prio = -1;
        do {
            printf("Prioridade (1-10): ");
            if (fgets(buffer, sizeof(buffer), stdin) == NULL) break;
            if (sscanf(buffer, "%d", &prio) != 1 || prio < 1 || prio > 10) {
                printf("Valor inválido. Tente novamente.\n");
                prio = -1;
            }
        } while (prio == -1);
        arr[i].prioridade = prio;
    }
    *n = quantidade;
    printf("\nCadastro concluído: %d componentes.\n", *n);
}

/* ---------------- menu e fluxo ---------------- */

void menuPrincipal() {
    Componente componentes[MAX_COMPONENTES];
    int total = 0;
    int ordenadoPorNome = 0; /* flag para saber se está ordenado por nome (para habilitar busca binária) */
    char opcaoBuf[32];

    while (1) {
        printf("\n========== MONTAGEM TORRE DE FUGA ==========\n");
        printf("1 - Cadastrar componentes\n");
        printf("2 - Ordenar por NOME (Bubble Sort) e medir (recomendado para busca)\n");
        printf("3 - Ordenar por TIPO (Insertion Sort) e medir\n");
        printf("4 - Ordenar por PRIORIDADE (Selection Sort) e medir\n");
        printf("5 - Buscar componente-chave por NOME (Busca Binária) [requer ordenação por NOME]\n");
        printf("6 - Mostrar componentes atuais\n");
        printf("0 - Sair\n");
        printf("Escolha: ");
        if (fgets(opcaoBuf, sizeof(opcaoBuf), stdin) == NULL) break;

        int opcao = -1;
        if (sscanf(opcaoBuf, "%d", &opcao) != 1) {
            printf("Entrada inválida.\n");
            continue;
        }

        if (opcao == 0) {
            printf("Encerrando módulo de montagem. Boa sorte na fuga!\n");
            break;
        } else if (opcao == 1) {
            cadastrarComponentes(componentes, &total);
            ordenadoPorNome = 0;
            mostrarComponentes(componentes, total);
        } else if (opcao == 2) {
            if (total == 0) {
                printf("Nenhum componente cadastrado.\n");
                continue;
            }
            unsigned long long comps = 0;
            double tsec = 0.0;
            bubbleSortNome(componentes, total, &comps, &tsec);
            ordenadoPorNome = 1;
            printf("\nBubble Sort por NOME concluído: comparações = %llu, tempo = %.6f s\n", comps, tsec);
            mostrarComponentes(componentes, total);
        } else if (opcao == 3) {
            if (total == 0) {
                printf("Nenhum componente cadastrado.\n");
                continue;
            }
            unsigned long long comps = 0;
            double tsec = 0.0;
            insertionSortTipo(componentes, total, &comps, &tsec);
            ordenadoPorNome = 0;
            printf("\nInsertion Sort por TIPO concluído: comparações = %llu, tempo = %.6f s\n", comps, tsec);
            mostrarComponentes(componentes, total);
        } else if (opcao == 4) {
            if (total == 0) {
                printf("Nenhum componente cadastrado.\n");
                continue;
            }
            unsigned long long comps = 0;
            double tsec = 0.0;
            selectionSortPrioridade(componentes, total, &comps, &tsec);
            ordenadoPorNome = 0;
            printf("\nSelection Sort por PRIORIDADE concluído: comparações = %llu, tempo = %.6f s\n", comps, tsec);
            mostrarComponentes(componentes, total);
        } else if (opcao == 5) {
            if (total == 0) {
                printf("Nenhum componente cadastrado.\n");
                continue;
            }
            if (!ordenadoPorNome) {
                printf("Atenção: busca binária requer que os componentes estejam ordenados por NOME.\n");
                printf("Deseja executar Bubble Sort por NOME agora? (s/n): ");
                char ans[8];
                if (fgets(ans, sizeof(ans), stdin) == NULL) continue;
                if (ans[0] == 's' || ans[0] == 'S') {
                    unsigned long long comps = 0;
                    double tsec = 0.0;
                    bubbleSortNome(componentes, total, &comps, &tsec);
                    ordenadoPorNome = 1;
                    printf("\nBubble Sort por NOME concluído: comparações = %llu, tempo = %.6f s\n", comps, tsec);
                } else {
                    printf("Busca cancelada. Ordene por NOME antes de usar busca binária.\n");
                    continue;
                }
            }

            char chave[MAX_NOME];
            printf("Digite o nome do componente-chave a buscar: ");
            if (fgets(chave, sizeof(chave), stdin) == NULL) continue;
            trim_newline(chave);

            unsigned long long compsBusca = 0;
            clock_t t0 = clock();
            int idx = buscaBinariaPorNome(componentes, total, chave, &compsBusca);
            clock_t t1 = clock();
            double tempoBusca = (double)(t1 - t0) / CLOCKS_PER_SEC;

            if (idx >= 0) {
                printf("\nComponente encontrado na posição %d (ID %d):\n", idx, idx+1);
                printf("Nome: %s | Tipo: %s | Prioridade: %d\n", componentes[idx].nome, componentes[idx].tipo, componentes[idx].prioridade);
            } else {
                printf("\nComponente '%s' não encontrado.\n", chave);
            }
            printf("Busca binária: comparações = %llu, tempo = %.6f s\n", compsBusca, tempoBusca);
        } else if (opcao == 6) {
            mostrarComponentes(componentes, total);
        } else {
            printf("Opção inválida.\n");
        }
    }
}

/* ---------------- ponto de entrada ---------------- */

int main(void) {
    srand((unsigned) time(NULL)); /* semente aleatória (não usada nas ordenações, mas boa prática) */
    menuPrincipal();
    return 0;
}
