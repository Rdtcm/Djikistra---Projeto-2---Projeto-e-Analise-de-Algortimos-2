/*
Referencias para implementacao: https://coderivers.org/blog/c-dijkstra/

Projeto - 2 de Projeto e analise de Algortimos 2

Nome: Ryan Vinicius Ledo dos Santos
RA: 10352727

*/
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>


#define TAM_VET 100

// estrutura usada para representar uma aresta (esquina vizinha + tempo)
typedef struct adj{
    int destino;
    int tempo;
    struct adj* prox;
}Adj;

// estrutura do grafo
typedef struct {
    int V;
    Adj** lista;
}Grafo;

// estrutura para os resultados do algortimo dado
typedef struct {
    int* T;
    int* R;
}Resultado;


// funcao para criar o grafo
Grafo* criarGrafo(int V) {
    Grafo* g = malloc(sizeof(Grafo));
    g->V = V;
    g->lista = calloc(V + 1, sizeof(Adj*));
    return g;
};

// funcao para adicionar aresta
void adicionarAresta(Grafo* g, int origem, int destino, int tempo) {
    Adj* nova = malloc(sizeof(Adj));
    nova->destino = destino;
    nova->tempo = tempo;
    nova->prox = g->lista[origem];
    g->lista[origem] = nova;

}

// funcao para reconstruir o caminho
void imprimeRota(int destino, int* R) {
    if (R[destino] == -1) {
        printf("%d", destino);
        return;
    }
    imprimeRota(R[destino], R);
    printf(" %d", destino);
}

// alortimo dado pelo professor
Resultado* calculaRotaMaisRapida(Grafo* g, int V) {
    Resultado* res = (Resultado*)malloc(sizeof(Resultado));

    // alocando memoria para os tempos e para as rotas
    res->T = (int*)malloc((V + 1) * sizeof(int));
    res->R = (int*)malloc((V + 1) * sizeof(int));

    int* emE = (int*)calloc(V + 1, sizeof(int));

    int origem = 1;

    for(int e = 1; e <= V; e++) {
        res->T[e] = INT_MAX; // t[e] = infinito
        res->R[e] = -1; // inicializando o antecessor como -1 (nenhum)
    }
    // t[1] = 0;
    res->T[origem] = 0;

    int esquinas_restantes = V;

    while(esquinas_restantes > 0) {
        int v = -1;
        int min_tempo = INT_MAX;

        for(int e = 1; e <= V; e++) {
            if (emE[e] == 0 && res->T[e] < min_tempo) {
                min_tempo = res->T[e];
                v = e;
            }
        }

        if (v == -1 || min_tempo == INT_MAX) {
            break;
        }

        emE[v] = 1;
        esquinas_restantes--;

        Adj* adj = g->lista[v];
        while(adj != NULL) {
            int e = adj->destino;
            int tempo_v_e = adj->tempo;

            // Verificação principal de relaxamento
            if(emE[e] == 0) {
                if(res->T[v] != INT_MAX) {
                   
                   int novo_tempo = res->T[v] + tempo_v_e;
                    
                    if (res->T[e] > novo_tempo) {
                        res->T[e] = novo_tempo;
                        res->R[e] = v;
                    }
                }
            }
            
            // CORREÇÃO: Mover o avanço do ponteiro para fora do if(emE[e] == 0)
            adj = adj->prox; 
        }
    }
    free(emE);
    return res;
}


int main() {
    int origem = 0, destino = 0, tempo = 0;
    int num_esquinas = 0, esquina_incendio = 0;

    FILE* arquivo = fopen("teste_1.txt", "r");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo 'teste_1.txt'\n");
        return 1;
    }
    
    // --- 1. LEITURA OBRIGATÓRIA: Esquina do Incêndio ---
    if (fscanf(arquivo, "%d", &esquina_incendio) != 1) {
        printf("Erro ao ler a esquina do incendio\n");
        fclose(arquivo);
        return 1;
    }

    // --- 2. LEITURA OBRIGATÓRIA: Quantidade de Esquinas (V) ---
    if (fscanf(arquivo, "%d", &num_esquinas) != 1) {
        printf("Erro ao ler o numero de esquinas\n");
        fclose(arquivo);
        return 1;
    }

    // --- 3. CRIAÇÃO DO GRAFO (AGORA COM O TAMANHO CORRETO) ---
    // Passamos V para criarGrafo(V) que alocará V + 1 posições.
    Grafo* mapa_M = criarGrafo(num_esquinas); 

    // --- 4. LEITURA DAS TRIPLAS (Ruas) ---
    // Continua a leitura a partir da terceira linha.
    while(fscanf(arquivo, "%d %d %d", &origem, &destino, &tempo) == 3) {
        if (origem == 0) break;

        adicionarAresta(mapa_M, origem, destino, tempo);
    }

    fclose(arquivo);

    // --- 5. EXECUÇÃO DO ALGORITMO ---
    Resultado* resultado = calculaRotaMaisRapida(mapa_M, num_esquinas);

    // --- 6. SAÍDA DE DADOS E RELATÓRIO DE TEMPO ---
    printf("rota até a esquina #%d: ", esquina_incendio);

    // Se o tempo for INT_MAX, o caminho é inacessível, e imprimeRota deve ser chamada apenas
    // se um caminho foi encontrado (T[destino] != INT_MAX).

    if (resultado->T[esquina_incendio] == INT_MAX) {
        printf("Rota inacessível ou inexistente.\n");
        printf("tempo calculado para rota = Rota inacessível.\n");
    } else {
        imprimeRota(esquina_incendio, resultado->R);
        printf("\n");
        printf("tempo calculado para rota = %d min.\n", resultado->T[esquina_incendio]);
    }
    
    // --- 7. LIBERAÇÃO DE MEMÓRIA ---
    free(resultado->T);
    free(resultado->R);
    free(resultado);
    // Para liberar o grafo mapa_M, você precisaria de uma função freeGrafo, mas vou omitir 
    // para focar na funcionalidade principal.

    return 0;
}