#include <iostream>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <cmath>

using namespace std;

// Função para gerar um membro da população
int* individuo(int numDeItens) {
    int* ind = new int[numDeItens];
    for (int i = 0; i < numDeItens; ++i) {
        ind[i] = rand() % 2;
    }
    return ind;
}

// Função para gerar a população
int** populacaoInicial(int numDeIndividuos, int numDeItens) {
    int** pop = new int*[numDeIndividuos];
    for (int i = 0; i < numDeIndividuos; ++i) {
        pop[i] = individuo(numDeItens);
    }
    return pop;
}

// Função de avaliação do indivíduo
int fitness(int* individuo, int pesoMaximo, int* pesos, int* valores, int numDeItens) {
    int peso_total = 0, valor_total = 0;
    for (int i = 0; i < numDeItens; ++i) {
        peso_total += individuo[i] * pesos[i];
        valor_total += individuo[i] * valores[i];
    }

    if (pesoMaximo - peso_total < 0) {
        return -1; // Retorna -1 no caso de peso excedido
    }
    return valor_total; // Se for um indivíduo válido, retorna seu valor, sendo maior melhor
}

// Função para encontrar a avaliação média da população
double mediaFitness(int** populacao, int pesoMaximo, int* pesos, int* valores, int numDeIndividuos, int numDeItens) {
    double summed = 0;
    int count = 0;
    for (int i = 0; i < numDeIndividuos; ++i) {
        int fit = fitness(populacao[i], pesoMaximo, pesos, valores, numDeItens);
        if (fit >= 0) {
            summed += fit;
            ++count;
        }
    }
    return count > 0 ? summed / count : 0.0;
}

void selecao(int** pais, int* fitness, int numDePais, int*& pai, int*& mae) {
    auto sortear = [&](double fitness_total, int indiceAIgnorar) -> int {
        double acumulado = 0, valor_sorteado = (rand() % 1000) / 1000.0;

        if (indiceAIgnorar != -1) {
            fitness_total -= fitness[indiceAIgnorar];
        }

        for (int indice = 0; indice < numDePais; ++indice) {
            if (indiceAIgnorar == indice) {
                continue;
            }
            acumulado += fitness[indice];
            if (acumulado / fitness_total >= valor_sorteado) {
                return indice;
            }
        }
        return -1;
    };

    double fitness_total = 0;
    for (int i = 0; i < numDePais; ++i) {
        fitness_total += fitness[i];
    }

    int indice_pai = sortear(fitness_total, -1);
    int indice_mae = sortear(fitness_total, indice_pai);

    pai = pais[indice_pai];
    mae = pais[indice_mae];
}

int** evoluir(int** populacao, int pesoMaximo, int* pesos, int* valores, int numDeIndividuos, int numDeItens, double mutate = 0.05) {
    // Tabula cada indivíduo e o seu fitness
    int** novos_pais = new int*[numDeIndividuos];
    int* novos_fitness = new int[numDeIndividuos];
    int count = 0;
    for (int i = 0; i < numDeIndividuos; ++i) {
        int fit = fitness(populacao[i], pesoMaximo, pesos, valores, numDeItens);
        if (fit >= 0) {
            novos_pais[count] = populacao[i];
            novos_fitness[count] = fit;
            ++count;
        }
    }

    // Ordena o vetor usando o comparador personalizado
    for(int i = 0; i < count; i++) {
        for(int j = i+1; j < count; j++) {
            if(novos_fitness[j] > novos_fitness[i]) {
                swap(novos_fitness[i], novos_fitness[j]);
                swap(novos_pais[i], novos_pais[j]);
            }
        }
    }

    // Reprodução
    int** filhos = new int*[numDeIndividuos];
    for (int i = 0; i < numDeIndividuos; ++i) {
        int* pai;
        int* mae;
        selecao(novos_pais, novos_fitness, count, pai, mae);

        // Crossover
        int* filho = new int[numDeItens];
        int ponto_corte = rand() % numDeItens;
        for (int j = 0; j < ponto_corte; ++j) {
            filho[j] = pai[j];
        }
        for (int j = ponto_corte; j < numDeItens; ++j) {
            filho[j] = mae[j];
        }

        // Mutação
        for (int j = 0; j < numDeItens; ++j) {
            if ((rand() % 1000) / 1000.0 < mutate) {
                filho[j] = 1 - filho[j];
            }
        }

        filhos[i] = filho;
    }

    delete[] novos_pais;
    delete[] novos_fitness;

    return filhos;
}

int main() {

    // Exemplo de uso
    int numDeItens = 10; // Atualizado para refletir o número correto de itens
    int numCromossomos = 150;
    int geracoes = 80;
    int pesoMaximo = 1000;

    // Inicialização dos pesos e valores
    int pesos[] = {40, 80, 80, 250, 20, 100, 60, 120, 1000, 80};
    int valores[] = {300, 100, 300, 750, 100, 1000, 3000, 500, 4000, 3000};

    // Inicialização da população
    int** populacao = populacaoInicial(numCromossomos, numDeItens);

    // Execução dos procedimentos
    double historico_de_fitness[geracoes + 1];
    historico_de_fitness[0] = mediaFitness(populacao, pesoMaximo, pesos, valores, numCromossomos, numDeItens);

    for (int i = 0; i < geracoes; ++i) {
        populacao = evoluir(populacao, pesoMaximo, pesos, valores, numCromossomos, numDeItens);
        historico_de_fitness[i + 1] = mediaFitness(populacao, pesoMaximo, pesos, valores, numCromossomos, numDeItens);
    }

  
    // Impressões no terminal
    cout << "Geração" << " | Valor na mochila: " << endl;
    cout << "---------------------------------------" << endl;
    for (int i = 0; i < geracoes + 1; ++i) {
        cout << i << "\t|" << historico_de_fitness[i] << endl;
    }
    cout << "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-" << endl;
    cout << "\nPeso máximo permitido:" << pesoMaximo << "Kg\n\nItens disponíveis:" << endl;
    for (int i = 0; i < numDeItens; ++i) {
        cout << "Item " << i + 1 << ": " << pesos[i] << " Kg | R$ " << valores[i] << endl;
    }
    cout << "\n-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-" << endl;

    cout << "\nPossiveis escolhas: " << endl;
    for (int i = 0; i < 5; ++i) {
        cout << i+1 << "[";
        for (int j = 0; j < numDeItens; ++j) {
            cout << populacao[i][j] << " ";
        }
        cout << "]";
        cout << endl;
    }
    cout << "\n-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-" << endl;

    return 0;
}
