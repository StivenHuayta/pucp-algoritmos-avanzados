#include <iostream>
#include <iomanip>
#include <climits>
#include <vector>
#include <ctime>
#include <cstdlib>

using namespace std;

struct Solucion {
    vector<vector<int>> asignaciones;
    vector<int> tiempos_acumulados;
    int makespan = INT_MAX;
};

Solucion construccionGrasp(const vector<int> & tareas, int cantidad_maquinas, double alfa) {
    int numero_tareas = tareas.size();

    Solucion solucion;
    solucion.tiempos_acumulados.assign(cantidad_maquinas, 0);
    solucion.asignaciones.resize(cantidad_maquinas);

    for (int i = 0; i < numero_tareas; i++) {
        int tiempo_tarea = tareas[i];

        int beta = INT_MAX;
        int tau = INT_MIN;

        for (int j = 0; j < cantidad_maquinas; j++) {
            int tiempo_proyectado = solucion.tiempos_acumulados[j] + tiempo_tarea;
            if (tiempo_proyectado < beta) beta = tiempo_proyectado;
            if (tiempo_proyectado > tau) tau = tiempo_proyectado;
        }

        double umbral = (double)beta + alfa * (tau - beta);
        vector<int> rcl;
        for (int j = 0; j < cantidad_maquinas; j++) {
            if (solucion.tiempos_acumulados[j] + tiempo_tarea <= umbral) rcl.push_back(j);
        }
        int indice_maquina_escojida = rcl[rand() % rcl.size()];
        solucion.asignaciones[indice_maquina_escojida].push_back(i);
        solucion.tiempos_acumulados[indice_maquina_escojida] += tiempo_tarea;
    }

    int makespan = INT_MIN;
    for (int j = 0; j < cantidad_maquinas; j++) {
        if (solucion.tiempos_acumulados[j] > makespan) makespan = solucion.tiempos_acumulados[j];
    }
    solucion.makespan = makespan;
    return solucion;
}

Solucion grasp(const vector<int> & tareas, int cantidad_maquinas) {
    double alfa = 0.3;
    int numero_iteraciones = 30000;
    Solucion mejorSolucion;
    for (int i = 0; i < numero_iteraciones; i++) {
        Solucion solucion_actual = construccionGrasp(tareas, cantidad_maquinas, alfa);
        if (solucion_actual.makespan < mejorSolucion.makespan) {
            mejorSolucion = solucion_actual;
        }
    }
    return mejorSolucion;
}

void imprimirSolucion(const Solucion & sol, const vector<int> & tareas) {
    cout << "=== MEJOR SOLUCION ENCONTRADA (GRASP-C) ===" << endl;
    cout << "Makespan optimo: " << sol.makespan << endl;
    cout << "\nDistribucion por maquina:" << endl;
    for (size_t j = 0; j < sol.tiempos_acumulados.size(); j++) {
        cout << "  Maquina " << (j + 1) << " (Carga = " << setw(2) << sol.tiempos_acumulados[j] << "): Tareas [ ";
        for (int idx : sol.asignaciones[j]) {
            cout << "J" << (idx + 1) << "(" << tareas[idx] << ") ";
        }
        cout << "]" << endl;
    }
}

int main() {
    srand(time(nullptr));
    int cantidad_maquinas = 3;
    const vector<int> tareas = {14, 9, 10, 6, 11, 15, 13, 8};
    Solucion solucion = grasp(tareas, cantidad_maquinas);
    imprimirSolucion(solucion, tareas);
    return 0;
}
