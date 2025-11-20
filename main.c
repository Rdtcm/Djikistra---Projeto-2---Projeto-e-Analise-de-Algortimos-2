/*
Referencias para implementacao: https://coderivers.org/blog/c-dijkstra/
Livro: Algoritmos - Teoria e Prática (Cormen)

Projeto - 2 de Projeto e analise de Algortimos 2

#Nome:Alef de souza Iima RA: 10431891
#Nome:Derick Sant'Ana Nascimento RA: 10443727
#Nome:Renan Horochk de Andrade RA: 10438120
#Nome:Ryan Vinicius Ledo RA: 10352727
*/

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

// --- ESTRUTURAS ---

// Estrutura para representar uma aresta (esquina vizinha + tempo)
typedef struct adj {
    int destino;
    int tempo;
    struct adj* prox;
} Adj;

// Estrutura do grafo (Lista de Adjacência)
typedef struct {
    int V;          // Número de vértices (esquinas)
    Adj** lista;    // Vetor de ponteiros para listas de adjacência
} Grafo;

// Estrutura para retornar os vetores T (tempos) e R (rotas)
typedef struct {
    int* T;
    int* R;
} Resultado;

// --- FUNÇÕES DE GRAFO ---

Grafo* criarGrafo(int V) {
    Grafo* g = (Grafo*)malloc(sizeof(Grafo));
    g->V = V;
    // Aloca V + 1 para usar índices de 1 a V diretamente
    g->lista = (Adj**)calloc(V + 1, sizeof(Adj*));
    return g;
}

void adicionarAresta(Grafo* g, int origem, int destino, int tempo) {
    // Inserção no início da lista encadeada (mais rápido O(1))
    Adj* nova = (Adj*)malloc(sizeof(Adj));
    nova->destino = destino;
    nova->tempo = tempo;
    nova->prox = g->lista[origem];
    g->lista[origem] = nova;
}

void liberarGrafo(Grafo* g) {
    if (g == NULL) return;
    for (int i = 1; i <= g->V; i++) {
        Adj* atual = g->lista[i];
        while (atual != NULL) {
            Adj* temp = atual;
            atual = atual->prox;
            free(temp);
        }
    }
    free(g->lista);
    free(g);
}

// --- FUNÇÕES DE SAÍDA (TELA E ARQUIVO) ---

// Função auxiliar recursiva para imprimir a rota
void imprimirCaminhoRecursivo(FILE* saida, int destino, int* R) {
    if (R[destino] == -1) {
        fprintf(saida, "%d", destino);
        return;
    }
    imprimirCaminhoRecursivo(saida, R[destino], R);
    fprintf(saida, " %d", destino);
}

// Função genérica que escreve o relatório tanto na tela quanto no arquivo
void gerarRelatorio(FILE* saida, int destino, Resultado* res) {
    fprintf(saida, "rota até a esquina #%d: ", destino);

    if (res->T[destino] == INT_MAX) {
        fprintf(saida, "Rota inacessível.\n");
        fprintf(saida, "tempo calculado para rota = infinito.\n");
    } else {
        // Reconstrói o caminho usando o vetor R
        imprimirCaminhoRecursivo(saida, destino, res->R);
        fprintf(saida, "\n");
        fprintf(saida, "tempo calculado para rota = %d min.\n", res->T[destino]);
    }
}

// --- ALGORITMO PRINCIPAL ---

Resultado* calculaRotaMaisRapida(Grafo* g, int V) {
    Resultado* res = (Resultado*)malloc(sizeof(Resultado));
    
    // Alocação dos vetores de resultado (índices 1 a V)
    res->T = (int*)malloc((V + 1) * sizeof(int));
    res->R = (int*)malloc((V + 1) * sizeof(int));
    
    // Vetor auxiliar para controlar o conjunto E (1 = está em E, 0 = removido)
    int* emE = (int*)calloc(V + 1, sizeof(int));

    // Inicialização conforme pseudo-código [cite: 12-22]
    // A esquina 1 é sempre o quartel (origem) [cite: 11]
    int origem = 1;

    for(int e = 1; e <= V; e++) {
        res->T[e] = INT_MAX; // Infinito
        res->R[e] = -1;      // Sem antecessor
        emE[e] = 1;          // Todas as esquinas começam em E
    }
    
    res->T[origem] = 0; // Tempo da origem para origem é 0

    int esquinas_restantes = V;

    // Loop principal: enquanto E não estiver vazio [cite: 23]
    while(esquinas_restantes > 0) {
        
        // Encontrar u em E com o menor T[u]
        int u = -1;
        int min_tempo = INT_MAX;

        for(int e = 1; e <= V; e++) {
            if (emE[e] == 1 && res->T[e] < min_tempo) {
                min_tempo = res->T[e];
                u = e;
            }
        }

        // Se não encontrar vértice acessível ou E estiver vazio "logicamente"
        if (u == -1 || min_tempo == INT_MAX) {
            break;
        }

        // Remove u de E [cite: 25]
        emE[u] = 0; 
        esquinas_restantes--;

        // Relaxamento dos vizinhos [cite: 26-31]
        Adj* vizinho = g->lista[u];
        while(vizinho != NULL) {
            int v = vizinho->destino;
            int peso = vizinho->tempo;

            // Verifica se vizinho v ainda está em E
            if(emE[v] == 1) {
                if(res->T[u] != INT_MAX) {
                    int novo_tempo = res->T[u] + peso;
                    
                    if (res->T[v] > novo_tempo) {
                        res->T[v] = novo_tempo;
                        res->R[v] = u; // Atualiza predecessor
                    }
                }
            }
            vizinho = vizinho->prox;
        }
    }

    free(emE);
    return res;
}

// --- MAIN ---

int main() {
    int origem, destino, tempo;
    int num_esquinas = 0, esquina_incendio = 0;
    char nome_arquivo[] = "teste_1.txt"; // Altere aqui se necessário ou use argv

    FILE* arquivo = fopen(nome_arquivo, "r");
    if (arquivo == NULL) {
        printf("Erro: Nao foi possivel abrir o arquivo '%s'.\nVerifique se ele existe na pasta do projeto.\n", nome_arquivo);
        return 1; // Retorna erro se não abrir
    }

    // 1. Leitura da esquina do incêndio
    if (fscanf(arquivo, "%d", &esquina_incendio) != 1) {
        printf("Erro ao ler a esquina do incendio.\n");
        fclose(arquivo);
        return 1;
    }

    // 2. Leitura do número de esquinas (V)
    if (fscanf(arquivo, "%d", &num_esquinas) != 1) {
        printf("Erro ao ler o numero de esquinas.\n");
        fclose(arquivo);
        return 1;
    }

    // Criação do grafo
    Grafo* mapa = criarGrafo(num_esquinas);

    // 3. Leitura das ruas (tratamento robusto para o '0' final) [cite: 56-60]
    while (1) {
        int r = fscanf(arquivo, "%d", &origem);
        
        // Se não leu nada ou leu o 0 final, para o loop
        if (r != 1 || origem == 0) {
            break;
        }

        // Lê o resto da linha (destino e tempo)
        fscanf(arquivo, "%d %d", &destino, &tempo);
        adicionarAresta(mapa, origem, destino, tempo);
    }
    fclose(arquivo);

    // 4. Execução do Algoritmo
    Resultado* resultado = calculaRotaMaisRapida(mapa, num_esquinas);

    // 5. Saída na TELA [cite: 62]
    gerarRelatorio(stdout, esquina_incendio, resultado);

    // 6. Saída no ARQUIVO "saida.txt" 
    FILE* arq_saida = fopen("saida.txt", "w");
    if (arq_saida == NULL) {
        printf("Erro ao criar o arquivo saida.txt.\n");
    } else {
        gerarRelatorio(arq_saida, esquina_incendio, resultado);
        fclose(arq_saida);
        printf("\n(Resultado salvo com sucesso em 'saida.txt')\n");
    }

    // 7. Liberação de Memória
    free(resultado->T);
    free(resultado->R);
    free(resultado);
    liberarGrafo(mapa);

    return 0;
}
