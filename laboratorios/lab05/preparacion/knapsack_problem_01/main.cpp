#include <iostream>
#include <iomanip>
#include <vector>
#include <set>
#include <algorithm>
#include <ctime>
#include <cstdlib>
#include <cmath>

#define MAX_GENERACIONES 10
#define TAM_POBLACION 10
#define PROPORCION_PADRES 0.5
#define PUNTO_CRUCE_RATIO 0.5
#define TASA_MUTACION 0.3


using namespace std;

bool es_solucion_valida(const vector<int> & candidato, int capacidad_mochila, const vector<int> & pesos_elementos) {
    int peso_acumulado = 0;
    for (int i = 0; i < candidato.size(); i++) {
        if (candidato[i] == 1) peso_acumulado += pesos_elementos[i];
    }
    return peso_acumulado <= capacidad_mochila;
}

void eliminar_duplicados(vector<vector<int>> & poblacion) {
    set<vector<int>> soluciones_unicas(poblacion.begin(), poblacion.end());
    poblacion.assign(soluciones_unicas.begin(), soluciones_unicas.end());
}

void inicializar_poblacion(vector<vector<int>> & poblacion, const vector<int> & pesos_elementos, int capacidad_mochila) {
    int cantidad_elementos = pesos_elementos.size();

    int intentos = 0;
    const int limiteIntentos = 1000;

    while (poblacion.size() < TAM_POBLACION && intentos < limiteIntentos) {
        vector<int> candidato(cantidad_elementos);
        for (int i = 0; i < cantidad_elementos; i++) {
            candidato[i] = rand() % 2;
        }

        if (es_solucion_valida(candidato, capacidad_mochila, pesos_elementos)) {
            poblacion.push_back(candidato);
            eliminar_duplicados(poblacion);
        }

        intentos++;
    }
}

int evaluar_aptitud(const vector<int> & individuo, const vector<int> & pesos_elementos) {
    int peso_acumulado = 0;
    for (int i = 0; i < individuo.size(); i++) {
        if (individuo[i] == 1) peso_acumulado += pesos_elementos[i];
    }
    return peso_acumulado;
}

void seleccionarProgenitores(const vector<vector<int>> & poblacion, vector<vector<int>> & padres, const vector<int> & pesos_elementos) {
    vector<int> puntajes(poblacion.size());
    int suma_puntajes = 0;

    for (int i = 0; i < poblacion.size(); i++) {
        puntajes[i] = evaluar_aptitud(poblacion[i], pesos_elementos);
        suma_puntajes += puntajes[i];
    }

    int cantidad_padres = (int)round(poblacion.size() * PROPORCION_PADRES);

    for (int p = 0; p < cantidad_padres; p++) {
        int valor_ruleta = rand() % suma_puntajes;
        int acumulado = 0;

        for (int i = 0; i < poblacion.size(); i++) {
            acumulado += puntajes[i];
            if (acumulado > valor_ruleta) {
                padres.push_back(poblacion[i]);
                break;
            }
        }
    }
}

vector<int> recombinar_individuos(const vector<int> & padre_a, const vector<int> & padre_b) {
    vector<int> hijo;

    int punto_corte = round(padre_a.size() * PUNTO_CRUCE_RATIO);

    for (int i = 0; i < punto_corte; i++) {
        hijo.push_back(padre_a[i]);
    }

    for (int i = punto_corte; i < padre_b.size(); i++) {
        hijo.push_back(padre_b[i]);
    }

    return hijo;
}

void reproducir_poblacion(vector<vector<int>> & poblacion, const vector<vector<int>> & padres, const vector<int> & pesos_elementos, int capacidad_mochila) {
    for (int i = 0; i < padres.size(); i++) {
        for (int j = 0; j < padres.size(); j++) {
            if (i != j) {
                vector<int> hijo = recombinar_individuos(padres[i], padres[j]);
                if (es_solucion_valida(hijo, capacidad_mochila, pesos_elementos)) {
                    poblacion.push_back(hijo);
                }
            }
        }
    }
}

void aplicar_mutacion_aleatoria(vector<vector<int>> & poblacion, const vector<vector<int>> & padres, const vector<int> & pesos_elementos, int capacidad_mochila) {
    int cantidad_mutaciones = (int)round(padres[0].size() * TASA_MUTACION);
    for (int i = 0; i < padres.size(); i++) {
        vector<int> copia_mutada = padres[i];
        for (int j = 0; j < cantidad_mutaciones; j++) {
            int posicion_gen = rand() % copia_mutada.size();
            copia_mutada[posicion_gen] = 1 - copia_mutada[posicion_gen];
        }
        if (es_solucion_valida(copia_mutada, capacidad_mochila, pesos_elementos)) {
            poblacion.push_back(copia_mutada);
        }
    }
}

void aplicar_inversion_completa(vector<vector<int>> & poblacion, const vector<vector<int>> & padres, const vector<int> & pesos_elementos, int capacidad_mochila) {
    for (int i = 0; i < padres.size(); i++) {
        vector<int> cromosoma_invertido = padres[i];
        for (int j = 0; j < cromosoma_invertido.size(); j++) {
            cromosoma_invertido[j] = 1 - cromosoma_invertido[j];
        }
        if (es_solucion_valida(cromosoma_invertido, capacidad_mochila, pesos_elementos)) {
            poblacion.push_back(cromosoma_invertido);
        }
    }
}

void podar_y_reemplazar_poblacion(vector<vector<int>> & poblacion, const vector<int> & pesos_elementos, int capacidad_mochila) {
    eliminar_duplicados(poblacion);
    sort(poblacion.begin(), poblacion.end(), [pesos_elementos](const vector<int> & a, const vector<int> & b)
    {return evaluar_aptitud(pesos_elementos, a) > evaluar_aptitud(pesos_elementos, b);});

    if (poblacion.size() > TAM_POBLACION) {
        poblacion.erase(poblacion.begin() + TAM_POBLACION, poblacion.end());
    }
}

void imprimir_poblacion(const vector<vector<int>> & poblacion, const vector<int> & pesos_elementos) {
    for (int i = 0; i < poblacion.size(); i++) {
        for (int j = 0; j < poblacion[i].size(); j++) {
            cout << poblacion[i][j] << " ";
        }
        cout << "Peso cargado = " << evaluar_aptitud(poblacion[i], pesos_elementos) << endl;
    }
}

int reportar_optimo_generacional(const vector<vector<int>> &comunidad, vector<int> pesos_elementos, int capacidad_mochila) {
    if (comunidad.empty()) return capacidad_mochila;

    size_t mejorIndice = 0;
    for (size_t i = 1; i < comunidad.size(); i++) {
        if (evaluar_aptitud(comunidad[mejorIndice], pesos_elementos) < evaluar_aptitud(comunidad[i], pesos_elementos)) {
            mejorIndice = i;
        }
    }

    int mejorPeso = evaluar_aptitud(comunidad[mejorIndice], pesos_elementos);
    cout << endl << "Mejor configuracion encontrada (Peso = " << mejorPeso << " / " << capacidad_mochila << "):" << endl;
    for (size_t i = 0; i < comunidad[mejorIndice].size(); i++) {
        cout << comunidad[mejorIndice][i] << "  ";
    }
    cout << endl;

    return capacidad_mochila - mejorPeso;
}

void resolver_mochila_genetico(const vector<int> & pesos_elementos, int capacidad_mochila) {
    vector<vector<int>> poblacion;
    inicializar_poblacion(poblacion, pesos_elementos, capacidad_mochila);

    for (int generacion = 0; generacion < MAX_GENERACIONES; generacion++) {
        cout << "================ GENERACION " << generacion + 1 << " ================" << endl;
        vector<vector<int>> padres;

        seleccionarProgenitores(poblacion, padres, pesos_elementos);
        reproducir_poblacion(poblacion, padres, pesos_elementos, capacidad_mochila);
        aplicar_mutacion_aleatoria(poblacion, padres, pesos_elementos, capacidad_mochila);
        aplicar_inversion_completa(poblacion, padres, pesos_elementos, capacidad_mochila);

        podar_y_reemplazar_poblacion(poblacion, pesos_elementos, capacidad_mochila);
        imprimir_poblacion(poblacion, pesos_elementos);
        reportar_optimo_generacional(poblacion, pesos_elementos, capacidad_mochila);
    }
}

int main() {
    srand(time(NULL));

    const vector<int> pesos_elementos = {1, 2, 3, 4, 5};
    int capacidad_mochila = 15;

    resolver_mochila_genetico(pesos_elementos, capacidad_mochila);

    return 0;
}
