#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <climits>
#include <cstdlib>
#include <ctime>

using namespace std;

struct Elemento {
    int id;
    string nombre;
    int peso;
    int beneficio;
    int riesgo;
    double ratio;
};

struct Solucion {
    vector<Elemento> seleccionados;
    int peso_total = 0;
    int riesgo_total = 0;
    int beneficio_total = 0;
};

Solucion construccionGrasp(const vector<Elemento>& elementos, int peso_maximo, int riesgo_maximo, double alfa) {
    int cantidad_elementos = (int)elementos.size();
    Solucion solucion;
    vector<bool> seleccionados(cantidad_elementos, false);

    int peso_acumulado = 0;
    int riesgo_acumulado = 0;

    while (true) {
        vector<Elemento> candidatos;
        for (int i = 0; i < cantidad_elementos; i++) {
            if (!seleccionados[i] &&
                peso_acumulado + elementos[i].peso <= peso_maximo &&
                riesgo_acumulado + elementos[i].riesgo <= riesgo_maximo) {
                candidatos.push_back(elementos[i]);
            }
        }

        if (candidatos.empty()) break;

        double beta = INT_MIN;
        double tau = INT_MAX;
        for (const Elemento& e : candidatos) {
            if (e.ratio > beta) beta = e.ratio;
            if (e.ratio < tau) tau = e.ratio;
        }

        double umbral = beta - alfa * (beta - tau);

        vector<Elemento> rcl;
        for (const Elemento& e : candidatos) {
            if (e.ratio >= umbral) {
                rcl.push_back(e);
            }
        }

        if (rcl.empty()) break;

        Elemento toAdd = rcl[rand() % rcl.size()];
        solucion.seleccionados.push_back(toAdd);
        solucion.beneficio_total += toAdd.beneficio;
        solucion.riesgo_total += toAdd.riesgo;
        solucion.peso_total += toAdd.peso;

        peso_acumulado += toAdd.peso;
        riesgo_acumulado += toAdd.riesgo;
        seleccionados[toAdd.id - 1] = true;
    }
    return solucion;
}

Solucion grasp(const vector<Elemento>& elementos, int peso_maximo, int riesgo_maximo) {
    double alfa = 0.3; // Tipo double para no truncar a 0
    int cantidad_iteraciones = 30000;
    Solucion mejor_solucion;

    for (int i = 0; i < cantidad_iteraciones; i++) {
        Solucion solucion_actual = construccionGrasp(elementos, peso_maximo, riesgo_maximo, alfa);
        if (solucion_actual.beneficio_total > mejor_solucion.beneficio_total) {
            mejor_solucion = solucion_actual;
        }
    }
    return mejor_solucion;
}

void imprimirSolucion(const Solucion& mejor, int peso_maximo, int riesgo_maximo) {
    cout << "=== MEJOR SOLUCION ENCONTRADA (GRASP-C) ===" << endl;
    cout << "Beneficio total: " << mejor.beneficio_total << endl;
    cout << "Peso total:      " << mejor.peso_total << " / " << peso_maximo << endl;
    cout << "Riesgo total:    " << mejor.riesgo_total << " / " << riesgo_maximo << endl;
    cout << "\nElementos seleccionados:" << endl;
    for (const Elemento& e : mejor.seleccionados) {
        cout << "  - ID " << setw(2) << e.id << ": " << left << setw(22) << e.nombre
             << " | Peso: " << setw(2) << e.peso
             << " | Riesgo: " << setw(2) << e.riesgo
             << " | Beneficio: " << setw(2) << e.beneficio
             << " | Ratio: " << fixed << setprecision(3) << e.ratio << endl;
    }
}

int main() {
    srand(time(nullptr));

    const vector<Elemento> elementos = {
        {1, "Servidor Cloud Edge", 12, 48, 6, 48.0 / (12.0 * 6.0)},
        {2, "Modulo ML Pipeline",  8, 30, 5, 30.0 / (8.0 * 5.0)},
        {3, "API Gateway V2",      5, 18, 3, 18.0 / (5.0 * 3.0)},
        {4, "Migracion DB NoSQL",  15, 50, 8, 50.0 / (15.0 * 8.0)},
        {5, "Cache Distribuido",   10, 32, 4, 32.0 / (10.0 * 4.0)},
        {6, "Motor de Reglas",     7, 21, 3, 21.0 / (7.0 * 3.0)},
        {7, "Suite de Seguridad",  14, 39, 7, 39.0 / (14.0 * 7.0)},
        {8, "Dashboard BI",        9, 23, 4, 23.0 / (9.0 * 4.0)},
        {9, "Logger Centralizado", 6, 14, 2, 14.0 / (6.0 * 2.0)},
        {10, "Pipeline ETL Legacy", 18, 36, 9, 36.0 / (18.0 * 9.0)}
    };

    const int peso_maximo = 50;
    const int riesgo_maximo = 25;

    Solucion solucion = grasp(elementos, peso_maximo, riesgo_maximo);

    imprimirSolucion(solucion, peso_maximo, riesgo_maximo);
    return 0;
}