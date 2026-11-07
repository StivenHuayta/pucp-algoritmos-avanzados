#include <iostream>
#include <iomanip>
#include <vector>
#include <climits>
#include <cstdlib>
#include <algorithm>
#include <ctime>

using namespace std;

struct Caja {
    vector<int> pesos;
    int peso_acumulado = 0;
};

struct Solucion {
    vector<Caja> cajas;
    int cantidad_contenedores = INT_MAX;
};

Solucion graspConstruccion(vector<int> & pesos, int capacidad_maxima, double alfa) {
    sort(pesos.begin(), pesos.end(), greater<int>());

    Solucion solucion;
    int numero_objetos = (int)pesos.size();
    int numero_cajas = 0;

    for (int i = 0; i < numero_objetos; i++) {
        int peso_actual = pesos[i];

        vector<int> candidatos; // Guarda los indices de las opciones factibles
        vector<int> desperdicios;

        // Evaluar en que cajas abiertas cabe el objeto
        for (int j = 0; j < numero_cajas; j++) {
            if (solucion.cajas[j].peso_acumulado + peso_actual <= capacidad_maxima) {
                candidatos.push_back(j);
                int desp = capacidad_maxima - (solucion.cajas[j].peso_acumulado + peso_actual);
                desperdicios.push_back(desp);
            }
        }

        // Siempre es factible abrir una nueva caja (se representa con el indice numero_cajas)[cite: 4]
        int opcion_nueva_caja = numero_cajas;
        candidatos.push_back(opcion_nueva_caja);
        desperdicios.push_back(capacidad_maxima - peso_actual);

        // Encontrar beta (minimo desperdicio) y tau (maximo desperdicio)
        int beta = INT_MAX;
        int tau = INT_MIN;
        for (int d : desperdicios) {
            if (d < beta) beta = d;
            if (d > tau) tau = d;
        }

        // Construir la RCL para minimizacion: beta <= desperdicio <= beta + alfa * (tau - beta)[cite: 2]
        double umbral = (double)beta + alfa * (double)(tau - beta);
        vector<int> rcl;
        for (size_t k = 0; k < candidatos.size(); k++) {
            if ((double)desperdicios[k] <= umbral) {
                rcl.push_back(candidatos[k]);
            }
        }

        // Seleccion aleatoria dentro de la RCL[cite: 2]
        int indice_elegido = rcl[rand() % rcl.size()];

        // Aplicar la decision
        if (indice_elegido == opcion_nueva_caja) {
            // Abrir y registrar una nueva caja[cite: 4]
            Caja nueva_caja;
            nueva_caja.pesos.push_back(peso_actual);
            nueva_caja.peso_acumulado = peso_actual;
            solucion.cajas.push_back(nueva_caja);
            numero_cajas++;
        } else {
            // Asignar a la caja existente[cite: 4]
            solucion.cajas[indice_elegido].pesos.push_back(peso_actual);
            solucion.cajas[indice_elegido].peso_acumulado += peso_actual;
        }
    }

    solucion.cantidad_contenedores = (int)solucion.cajas.size();
    return solucion;
}

Solucion grasp(vector<int> & pesos, int capacidad_maxima) {
    double alfa = 0.3;
    int numero_iteraciones = 30000;
    Solucion mejor_solucion;
    for (int i = 0; i < numero_iteraciones; i++) {
        Solucion solucion_actual = graspConstruccion(pesos, capacidad_maxima, alfa);
        if (solucion_actual.cantidad_contenedores < mejor_solucion.cantidad_contenedores) {
            mejor_solucion = solucion_actual;
        }
    }
    return mejor_solucion;
}

void imprimirSolucion(const Solucion & solucion, int capacidad_maxima) {
    cout << "Capacidad maxima: " << capacidad_maxima << " kg." << endl;
    cout << "Cantidad de contenedores utilizados: " << solucion.cantidad_contenedores << endl << endl;

    for (size_t i = 0; i < solucion.cajas.size(); i++) {
        cout << "Contenedor " << (i + 1) << " (Ocupado: "
             << setw(2) << solucion.cajas[i].peso_acumulado << "/" << capacidad_maxima << " kg): [ ";
        for (int p : solucion.cajas[i].pesos) {
            cout << p << " ";
        }
        cout << "]" << endl;
    }
}

int main() {
    srand(time(nullptr));

    vector<int> pesos = {4, 8, 1, 4, 2, 1};
    int capacidad_maxima = 10;

    Solucion solucion = grasp(pesos, capacidad_maxima);
    imprimirSolucion(solucion, capacidad_maxima);

    return 0;
}