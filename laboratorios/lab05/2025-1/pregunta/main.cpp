#include <iostream>
#include <iomanip>
#include <climits>
#include <cmath>
#include <fstream>
#include <vector>
#include <map>
#include <iterator>
#include <algorithm>
#include <cstring>

using namespace std;

struct Individuo {
    vector<int> cromosoma;
    vector<int> cambio_de_repartidor;
};

vector<Individuo> generar_poblacion_inicial(const vector<vector<int>> & grafo) {
    vector<Individuo> poblacion;

    for (int i = 0; i < grafo.size(); i++) {
        Individuo ind;

    }

    return poblacion;
}

Individuo obtener_ruta_optima(const vector<vector<int>> & grafo) {
    Individuo solucion;

    vector<Individuo> poblacion = generar_poblacion_inicial(grafo);


    return solucion;
}

int main() {
    const vector<vector<int>> grafo {
        {0, 4, 0, 3, 0, 0, 0},
        {0, 0, 2, 0, 0, 0, 0},
        {8, 0, 0, 0, 2, 0, 0},
        {0, 0, 0, 0, 6, 0, 0},
        {0, 0, 0, 0, 0, 5, 0},
        {0, 0, 0, 0, 0, 0, 2},
        {3, 0, 0, 0, 0, 0, 0},
    };

    Individuo solucion = obtener_ruta_optima(grafo);

    return 0;
}
