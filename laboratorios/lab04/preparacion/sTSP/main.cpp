#include <iostream>
#include <vector>
#include <cstdlib>
#include <climits>
#include <ctime>

using namespace std;

int construirCamino(const vector<vector<int>> & distancias, vector<int> & recorrido, double alfa) {
    int nroCiudades = distancias.size();
    int distanciaTotal = 0;
    vector<bool> visitados(nroCiudades, false);
    recorrido.clear();

    int actual = 0;
    recorrido.push_back(actual);
    visitados[actual] = true;

    while (recorrido.size() < (size_t)nroCiudades) {
        vector<int> candidatos;
        for (int i = 0; i < nroCiudades; i++) {
            if (!visitados[i] && distancias[actual][i] > 0) {
                candidatos.push_back(i);
            }
        }

        // Si no hay candidatos alcanzables, el camino es inviable
        if (candidatos.empty()) return -1;

        int beta = INT_MAX;
        int tau = INT_MIN;
        for (int c : candidatos) {
            int distancia = distancias[actual][c];
            if (distancia < beta) beta = distancia;
            if (distancia > tau) tau = distancia;
        }

        double umbral = beta + alfa * (tau - beta);
        vector<int> rcl;
        for (int c : candidatos) {
            if (distancias[actual][c] <= umbral) {
                rcl.push_back(c);
            }
        }

        if (rcl.empty()) return -1;

        int destino = rcl[rand() % rcl.size()];
        distanciaTotal += distancias[actual][destino];
        recorrido.push_back(destino);
        visitados[destino] = true;
        actual = destino;
    }

    // Cierre del ciclo hamiltoniano hacia el origen
    int origen = recorrido[0];
    if (distancias[actual][origen] > 0) {
        recorrido.push_back(origen);
        return distanciaTotal + distancias[actual][origen];
    }

    return -1;
}

void grasp(const vector<vector<int>> & distancias, vector<int> & recorrido) {
    double alfa = 0.3;
    int maxIntentos = 30000;
    int menorDistancia = INT_MAX;
    vector<int> mejorRecorrido;
    vector<int> tempRecorrido;

    for (int i = 0; i < maxIntentos; i++) {
        int distancia = construirCamino(distancias, tempRecorrido, alfa);
        if (distancia != -1 && distancia < menorDistancia) {
            menorDistancia = distancia;
            mejorRecorrido = tempRecorrido;
        }
    }

    recorrido = mejorRecorrido;
}

void imprimir(const vector<int> & recorrido) {
    if (recorrido.empty()) {
        cout << "No se encontro un recorrido factible tras las iteraciones." << endl;
        return;
    }

    const vector<char> nombres = {'a', 'b', 'c', 'd', 'e', 'f'};
    cout << "Recorrido obtenido: ";
    for (size_t i = 0; i < recorrido.size(); ++i) {
        cout << nombres[recorrido[i]];
        if (i + 1 < recorrido.size()) {
            cout << " -> ";
        }
    }
    cout << endl;
}

int main() {
    srand(time(nullptr));

    const vector<vector<int>> distancias = {
        //     a   b   c  d   e   f
        {   0,  6,  0, 9, 10, 12 }, // a
        {   6,  0,  5, 8,  0, 11 }, // b
        {   0,  5,  0, 7,  0,  8 }, // c
        {   9,  8,  7, 0,  7,  6 }, // d
        {  10,  0,  0, 7,  0,  9 }, // e
        {  12, 11,  8, 6,  9,  0 }  // f
    };

    vector<int> recorrido;
    grasp(distancias, recorrido);
    imprimir(recorrido);

    return 0;
}