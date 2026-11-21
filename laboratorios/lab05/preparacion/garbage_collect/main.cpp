#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <stack>
#include <vector>

#define TASA_SELECCION 0.3
#define CAPACIDAD_POBLACION 10
#define N_GENERACIONES 1000
#define PUNTO_CASAMIENTO 0.5
#define TASA_INVERSION 0.15

using namespace std;

struct Punto {
    int peso;
    int tiempo;
};

bool es_individuo_valido(const vector<int> & individuo, const vector<Punto> & puntos, int peso_maximo, int tiempo_maximo) {
    for (int i = 0; i < individuo.size(); i++) {
        if (individuo[i]) {
            peso_maximo -= puntos[i].peso;
            tiempo_maximo -= puntos[i].tiempo;
            if (peso_maximo < 0 || tiempo_maximo < 0) return false;
        }
    }
    return true;
}

vector<vector<int>> construir_poblacion_inicial(const vector<Punto> & puntos, int peso_maximo, int tiempo_maximo) {
    vector<vector<int>> poblacion;
    while (poblacion.size() < CAPACIDAD_POBLACION) {
        vector<int> individuo;
        for (int j = 0; j < puntos.size(); j++) {
            individuo.push_back(rand() % 2);
        }

        if (es_individuo_valido(individuo, puntos, peso_maximo, tiempo_maximo)) {
            poblacion.push_back(individuo);
        }
    }

    return poblacion;
}

double calcular_aptitud(const vector<int> & individuo, const vector<Punto> & puntos, int peso_maximo, int tiempo_maximo) {
    int suma_pesos = 0;
    int suma_tiempos = 0;

    for (int i = 0; i < individuo.size(); i++) {
        if (individuo[i]) {
            suma_pesos += puntos[i].peso;
            suma_tiempos += puntos[i].tiempo;
        }
    }

    double aptitud = (double)suma_pesos/suma_tiempos;
    return aptitud;
}

vector<vector<int>> seleccionar_progenitores(const vector<vector<int>> & poblacion, const vector<Punto> & puntos, int peso_maximo, int tiempo_maximo) {
    vector<vector<int>> progenitores;

    vector<double> aptitudes(poblacion.size());
    double suma_aptitudes = 0.0;
    for (int i = 0; i < poblacion.size(); i++) {
        aptitudes[i] = calcular_aptitud(poblacion[i], puntos, peso_maximo, tiempo_maximo);
        suma_aptitudes += aptitudes[i];
    }

    int cantidad_progenitores = (int)round(poblacion.size() * TASA_SELECCION);

    for (int i = 0; i < cantidad_progenitores; i++) {
        double umbral = ((double)rand() / RAND_MAX) * suma_aptitudes;
        double suma_acumulada = 0;
        int indice_seleccionado = -1;
        for (int j = 0; j < poblacion.size(); j++) {
            double aptitud = calcular_aptitud(poblacion[j], puntos, peso_maximo, tiempo_maximo);
            suma_acumulada += aptitud;
            if (suma_acumulada > umbral) {
                progenitores.push_back(poblacion[j]);
                indice_seleccionado = j;
                break;
            }
        }
        if (indice_seleccionado == -1) {
            progenitores.push_back(poblacion[poblacion.size() - 1]);
        }

    }

    return progenitores;
}

vector<int> cruzar_padres(const vector<int> & padre, const vector<int> & madre) {
    int punto_corte = (int)round(padre.size() * PUNTO_CASAMIENTO);
    vector<int> hijo;
    for (int i = 0; i < punto_corte; i++) {
        hijo.push_back(padre[i]);
    }
    for (int j = punto_corte; j < madre.size(); j++) {
        hijo.push_back(madre[j]);
    }
    return hijo;
}

vector<int> invertir(const vector<int> & individuo) {
    vector<int> to_return(individuo.size());
    for (int i = 0 ; i < individuo.size(); i++) {
        to_return[i] = 1 - individuo[i];
    }
    return to_return;
}

void reproducir_poblacion(vector<vector<int>> & poblacion, const vector<vector<int>> & progenitores,
                          const vector<Punto> & puntos, int peso_maximo, int tiempo_maximo) {
    vector<vector<int>> hijos;
    for (int i = 0; i < progenitores.size(); i++) {
        for (int j = 0; j < progenitores.size(); j++) {
            if (i != j) {
                vector<int> hijo = cruzar_padres(progenitores[i], progenitores[j]);
                if (es_individuo_valido(hijo, puntos, peso_maximo, tiempo_maximo)) {
                    poblacion.push_back(hijo);
                    hijos.push_back(hijo);
                }
            }
        }
    }
    int cantidad_inversiones = (int)round(hijos.size() * TASA_INVERSION);
    for (int i = 0; i < cantidad_inversiones; i++) {
        vector<int> invertido = invertir(hijos[i]);
        if (es_individuo_valido(invertido, puntos, peso_maximo, tiempo_maximo)) {
            poblacion.push_back(invertido);
        }
    }
}

void eliminar_duplicados(vector<vector<int>> & poblacion) {
    sort(poblacion.begin(), poblacion.end());
    poblacion.erase(unique(poblacion.begin(), poblacion.end()), poblacion.end());
}

void ordenar_reducir(vector<vector<int>> & poblacion, const vector<Punto> & puntos, int peso_maximo, int tiempo_maximo) {
    sort(poblacion.begin(), poblacion.end(), [puntos, peso_maximo, tiempo_maximo](const vector<int> & a, const vector<int> & b) {
       return calcular_aptitud(a, puntos, peso_maximo, tiempo_maximo) > calcular_aptitud(b, puntos, peso_maximo, tiempo_maximo);
    });

    if (poblacion.size() > CAPACIDAD_POBLACION) {
        poblacion.erase(poblacion.begin() + CAPACIDAD_POBLACION, poblacion.end());
    }
}

vector<int> elegir_puntos(const vector<Punto> & puntos, int peso_maximo, int tiempo_maximo) {
    vector<vector<int>> poblacion = construir_poblacion_inicial(puntos, peso_maximo, tiempo_maximo);

    vector<int> mejor_individuo;
    double mejor_aptitud = -1;

    for (int i = 0; i < N_GENERACIONES; i++) {
        vector<vector<int>> progenitores = seleccionar_progenitores(poblacion, puntos, peso_maximo, tiempo_maximo);
        reproducir_poblacion(poblacion, progenitores, puntos, peso_maximo, tiempo_maximo);
        eliminar_duplicados(poblacion);
        ordenar_reducir(poblacion, puntos, peso_maximo, tiempo_maximo);

        if (!poblacion.empty()) {
            vector<int> candidato = poblacion[0];
            double aptitud = calcular_aptitud(candidato, puntos, peso_maximo, tiempo_maximo);
            if (aptitud > mejor_aptitud) {
                mejor_aptitud = aptitud;
                mejor_individuo = candidato;
            }
        }
    }

    return mejor_individuo;
}

void imprimirPuntos(const vector<int> & seleccionados) {
    cout << "Seleccionados: ";
    for (int i = 0; i < seleccionados.size(); i++) {
        if (seleccionados[i]) {
            cout << i+1 << " ";
        }
    }
}

int main() {
    srand(time(nullptr));

    const vector<Punto> puntos = {
        {80, 15},
        {45, 10},
        {120, 25},
        {60, 12},
        {95, 20},
        {35, 8},
        {110, 22},
        {50, 14},
        {75, 18},
        {40, 9}
    };

    const int peso_maximo = 500;
    const int tiempo_maximo = 90;

    vector<int> seleccionados = elegir_puntos(puntos, peso_maximo, tiempo_maximo);
    imprimirPuntos(seleccionados);
    return 0;
}
