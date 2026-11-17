#include <iostream>
#include <iomanip>
#include <cmath>
#include <algorithm>
#include <vector>
#include <map>
#include <climits>
#include <ctime>
#include <cstdlib>

#define TOTAL_GENERACIONES 5
#define CAPACIDAD_POBLACION 10
#define TASA_SELECCION 0.3
#define PUNTO_CASAMIENTO 0.5

using namespace std;

// Forma de solucion: Verifica que un cromosoma binario represente una asignacion
// valida, comprobando que ningun camion supere su capacidad maxima y que cada
// paquete este asignado a lo sumo a un camion.
bool es_cromosoma_valido(const vector<int> & candidato, const vector<int> & paquetes, const vector<int> & camiones) {
    int cantidad_camiones = camiones.size();
    int cantidad_paquetes = paquetes.size();

    // Verificar que la suma de pesos en cada camion no exceda su capacidad
    for (int c = 0; c < cantidad_camiones; c++) {
        int uso = 0;
        for (int p = 0; p < cantidad_paquetes; p++) {
            int idx = c * cantidad_paquetes + p;
            if (candidato[idx] == 1) {
                uso += paquetes[p];
            }
        }
        if (uso > camiones[c]) return false;
    }

    // Verificar que cada paquete aparezca como maximo en un camion
    for (int p = 0; p < cantidad_paquetes; p++) {
        int cuenta = 0;
        for (int c = 0; c < cantidad_camiones; c++) {
            int idx = c * cantidad_paquetes + p;
            if (candidato[idx] == 1) {
                cuenta++;
            }
        }
        if (cuenta > 1) return false;
    }

    return true;
}

// Forma de solucion: Construye la poblacion inicial asignando aleatoriamente cada
// paquete a un camion (cromosoma binario 0/1) y aceptando solo los individuos validos.
vector<vector<int>> construir_poblacion_inicial(const vector<int> & paquetes, const vector<int> & camiones) {
    int cantidad_camiones = camiones.size();
    int cantidad_paquetes = paquetes.size();
    int longitud_cromosoma = cantidad_paquetes * cantidad_camiones;

    vector<vector<int>> poblacion;

    while (poblacion.size() < CAPACIDAD_POBLACION) {
        vector<int> candidato(longitud_cromosoma, 0);
        for (int p = 0; p < cantidad_paquetes; p++) {
            int c = rand() % cantidad_camiones;
            candidato[c * cantidad_paquetes + p] = 1;
        }

        if (es_cromosoma_valido(candidato, paquetes, camiones)) {
            poblacion.push_back(candidato);
        }
    }

    return poblacion;
}

// Forma de solucion: Calcula la aptitud sumando el peso total de los paquetes
// cargados; maximizar la carga total equivale a minimizar el desperdicio en todos los camiones.
int calcular_aptitud(const vector<int> & cromosoma, const vector<int> & paquetes, const vector<int> & camiones) {
    int cantidad_camiones = camiones.size();
    int cantidad_paquetes = paquetes.size();

    int aptitud = 0;

    for (int c = 0; c < cantidad_camiones; c++) {
        for (int p = 0; p < cantidad_paquetes; p++) {
            int idx = c * cantidad_paquetes + p;
            if (cromosoma[idx] == 1) aptitud += paquetes[p];
        }
    }

    return aptitud;
}

// Forma de solucion: Selecciona progenitores mediante el metodo de la ruleta
// proporcional a la aptitud de cada individuo, segun TASA_SELECCION.
vector<vector<int>> seleccionar_progenitores(const vector<vector<int>> & poblacion, const vector<int> & paquetes, const vector<int> & camiones) {
    vector<vector<int>> progenitores;

    vector<int> puntajes(poblacion.size());
    int suma_puntajes = 0;

    for (int i = 0; i < poblacion.size(); i++) {
        puntajes[i] = calcular_aptitud(poblacion[i], paquetes, camiones);
        suma_puntajes += puntajes[i];
    }

    int cantidad_padres = (int)round(poblacion.size() * TASA_SELECCION);

    // Evitar division por cero si toda la poblacion tuviera aptitud 0
    if (suma_puntajes == 0) {
        for (int k = 0; k < cantidad_padres; k++) {
            progenitores.push_back(poblacion[rand() % poblacion.size()]);
        }
        return progenitores;
    }

    for (int k = 0; k < cantidad_padres; k++) {
        int tiro_ruleta = rand() % suma_puntajes;
        int acumulado = 0;

        for (int i = 0; i < poblacion.size(); i++) {
            acumulado += puntajes[i];
            if (acumulado > tiro_ruleta) {
                progenitores.push_back(poblacion[i]);
                break;
            }
        }
    }

    return progenitores;
}

// Forma de solucion: Realiza un cruce de un punto entre dos cromosomas padres
// usando PUNTO_CASAMIENTO para combinar sus genes binarios en un nuevo hijo.
vector<int> cruzar_padres(const vector<int> & padre, const vector<int> & madre) {
    vector<int> hijo;
    int punto_corte = (int)round(padre.size() * PUNTO_CASAMIENTO);

    for (int i = 0; i < punto_corte; i++) {
        hijo.push_back(padre[i]);
    }

    for (int i = punto_corte; i < madre.size(); i++) {
        hijo.push_back(madre[i]);
    }

    return hijo;
}

// Forma de solucion: Aplica mutacion reasignando un paquete elegido al azar a un
// nuevo camion aleatorio para mantener diversidad genetica en la poblacion.
void mutar(vector<int> & cromosoma, int cantidad_paquetes, int cantidad_camiones) {
    int p = rand() % cantidad_paquetes;
    for (int c = 0; c < cantidad_camiones; c++) {
        cromosoma[c * cantidad_paquetes + p] = 0;
    }
    int nuevo_c = rand() % cantidad_camiones;
    cromosoma[nuevo_c * cantidad_paquetes + p] = 1;
}

// Forma de solucion: Genera nuevos individuos cruzando pares distintos de
// progenitores y aplicando mutacion, agregando a la poblacion solo los hijos validos.
void reproducir_poblacion(vector<vector<int>> & poblacion, const vector<vector<int>> & progenitores,
                          const vector<int> & paquetes, const vector<int> & camiones) {
    int cantidad_paquetes = paquetes.size();
    int cantidad_camiones = camiones.size();

    for (int i = 0; i < progenitores.size(); i++) {
        for (int j = 0; j < progenitores.size(); j++) {
            if (i != j) {
                vector<int> hijo = cruzar_padres(progenitores[i], progenitores[j]);
                if (es_cromosoma_valido(hijo, paquetes, camiones)) {
                    poblacion.push_back(hijo);
                }

                // Probar una variante mutada del hijo para explorar nuevas asignaciones
                mutar(hijo, cantidad_paquetes, cantidad_camiones);
                if (es_cromosoma_valido(hijo, paquetes, camiones)) {
                    poblacion.push_back(hijo);
                }
            }
        }
    }
}

// Forma de solucion: Ordena la poblacion y elimina cromosomas repetidos usando
// sort y unique de <algorithm>.
void eliminar_duplicados(vector<vector<int>> & poblacion) {
    sort(poblacion.begin(), poblacion.end());
    poblacion.erase(unique(poblacion.begin(), poblacion.end()), poblacion.end());
}

// Forma de solucion: Depura duplicados, ordena la poblacion de mayor a menor
// aptitud y recorta el tamano al limite CAPACIDAD_POBLACION (elitismo).
void renovar_poblacion(vector<vector<int>> & poblacion, const vector<int> & paquetes, const vector<int> & camiones) {
    eliminar_duplicados(poblacion);

    sort(poblacion.begin(), poblacion.end(), [paquetes, camiones](const vector<int> & a, const vector<int> & b)
    {return calcular_aptitud(a, paquetes, camiones) > calcular_aptitud(b, paquetes, camiones);});

    if (poblacion.size() > CAPACIDAD_POBLACION) {
        poblacion.erase(poblacion.begin() + CAPACIDAD_POBLACION, poblacion.end());
    }
}

// Forma de solucion: Ejecuta el algoritmo genetico durante TOTAL_GENERACIONES
// iteraciones (seleccion, cruce/mutacion y renovacion) y decodifica el mejor cromosoma.
vector<vector<int>> resolver_asignacion_camiones(const vector<int> & paquetes, const vector<int> & camiones) {
    int cantidad_camiones = camiones.size();
    int cantidad_paquetes = paquetes.size();

    vector<vector<int>> poblacion = construir_poblacion_inicial(paquetes, camiones);

    vector<int> mejor_cromosoma;
    int mejor_aptitud = -1;

    for (int generacion = 0; generacion < TOTAL_GENERACIONES; generacion++) {
        vector<vector<int>> progenitores = seleccionar_progenitores(poblacion, paquetes, camiones);
        reproducir_poblacion(poblacion, progenitores, paquetes, camiones);
        renovar_poblacion(poblacion, paquetes, camiones);

        int aptitud_actual = calcular_aptitud(poblacion[0], paquetes, camiones);
        if (aptitud_actual > mejor_aptitud) {
            mejor_aptitud = aptitud_actual;
            mejor_cromosoma = poblacion[0];
        }
    }

    // Decodificar el mejor cromosoma binario en listas de paquetes por camion
    vector<vector<int>> asignaciones(cantidad_camiones);
    for (int c = 0; c < cantidad_camiones; c++) {
        for (int p = 0; p < paquetes.size(); p++) {
            int idx = c * cantidad_paquetes + p;
            if (mejor_cromosoma[idx]) {
                asignaciones[c].push_back(p);
            }
        }
    }
    return asignaciones;
}

// Forma de solucion: Imprime los paquetes asignados a cada camion y calcula el
// desperdicio por camion y el desperdicio total considerando todos los camiones.
void imprimirSolucion(const vector<vector<int>> & asignaciones, const vector<int> & paquetes, const vector<int> & camiones) {
    int desperdicio_total = 0;

    for (int i = 0; i < asignaciones.size(); i++) {
        int peso_usado = 0;
        cout << "Camion " << i + 1 << " (Cap: " << camiones[i] << " kg): ";
        for (int j = 0; j < asignaciones[i].size(); j++) {
            int idx_paquete = asignaciones[i][j];
            cout << idx_paquete + 1 << " ";
            peso_usado += paquetes[idx_paquete];
        }
        int desperdicio = camiones[i] - peso_usado;
        desperdicio_total += desperdicio;
        cout << "| Desperdicio: " << desperdicio << " kg" << endl;
    }

    cout << "Desperdicio total: " << desperdicio_total << " kg" << endl;
}

// Forma de solucion: Define los datos de entrada de paquetes y camiones, invoca
// al algoritmo genetico y muestra la asignacion obtenida.
int main() {
    srand(time(nullptr));

    const vector<int> paquetes = {150, 100, 180, 50, 120, 10};
    const vector<int> camiones = {250, 200, 200, 100};

    vector<vector<int>> asignaciones = resolver_asignacion_camiones(paquetes, camiones);

    imprimirSolucion(asignaciones, paquetes, camiones);
    return 0;
}
