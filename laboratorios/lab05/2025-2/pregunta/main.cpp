#include <iostream>
#include <iomanip>
#include <climits>
#include <cmath>
#include <fstream>
#include <vector>
#include <map>
#include <algorithm>
#include <cstring>

#define N_INDIVIDUOS 10
#define TASA_SELECCION 0.3
#define PUNTO_CASAMIENTO 0.5
#define N_GENERACIONES 5

using namespace std;

bool es_individuo_valido(const vector<int>& individuo,
                         const vector<int>& paquetes,
                         vector<int> camiones) {
    const int cantidad_paquetes = paquetes.size();
    const int cantidad_camiones = camiones.size();

    vector<int> asignados(cantidad_paquetes, 0);

    for (int i = 0; i < cantidad_camiones; i++) {
        for (int j = 0; j < cantidad_paquetes; j++) {
            int idx = i * cantidad_paquetes + j;

            if (individuo[idx]) {
                asignados[j]++;
                camiones[i] -= paquetes[j];
                if (camiones[i] < 0) return false;
            }
        }
    }

    // Siguiendo la idea del profesor:
    // Es válido si un paquete no se asigna (a == 0),
    // solo es inválido si está duplicado en más de un camión (a > 1).
    for (int a : asignados) {
        if (a > 1) return false;
    }

    return true;
}

vector<vector<int>> construir_poblacion_inicial(const vector<int> & paquetes, const vector<int> & camiones) {
	vector<vector<int>> poblacion;

	while (poblacion.size() < N_INDIVIDUOS) {
		vector<int> individuo(paquetes.size()*camiones.size());
		for (int i = 0; i < paquetes.size()*camiones.size(); i++) {
			individuo[i] = rand() % 2;
		}

		if (es_individuo_valido(individuo, paquetes, camiones)) {
			poblacion.push_back(individuo);
		}
	}

	return poblacion;
}

int calcular_aptitud(const vector<int> & individuo, const vector<int> & paquetes, const vector<int> & camiones) {
	const int cantidad_paquetes = paquetes.size();
	const int cantidad_camiones = camiones.size();

	int aptitud = 0;

	for (int i = 0; i < cantidad_camiones; i++) {
        for (int j = 0; j < cantidad_paquetes; j++) {
			int idx = i * cantidad_paquetes + j;
			if (individuo[idx]) {
				aptitud += paquetes[j];
			}
        }
    }

	return aptitud;
}

vector<vector<int>> seleccionar_progenitores(const vector<vector<int>> & poblacion, const vector<int> &paquetes, const vector<int> & camiones) {
	vector<int> aptitudes(poblacion.size());
	int suma_aptitudes = 0;
	for (int i = 0; i < poblacion.size(); i++) {
		aptitudes[i] = calcular_aptitud(poblacion[i], paquetes, camiones);
		suma_aptitudes += aptitudes[i];
	}

	int cantidad_progenitores = (int)round(poblacion.size() * TASA_SELECCION);

	vector<vector<int>> progenitores;

	if (suma_aptitudes == 0) {
		for (int i = 0; i < cantidad_progenitores; i++) {
			progenitores.push_back(poblacion[rand() % poblacion.size()]);
		}
		return progenitores;
	}

	for (int i = 0; i < cantidad_progenitores; i++) {
		int umbral = rand() % suma_aptitudes;
		int suma_acumulada = 0;
		for (int j = 0; j < poblacion.size(); j++) {
			suma_acumulada += aptitudes[j];
			if (suma_acumulada > umbral) {
				progenitores.push_back(poblacion[j]);
				break;
			}
		}
	}

	return progenitores;
}

vector<int> cruzar_padres(const vector<int> & padre, const vector<int> &madre) {
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

void reproducir_poblacion(vector<vector<int>> & poblacion, const vector<vector<int>> & progenitores,
						  const vector<int> & paquetes, const vector<int> & camiones) {
	for (int i = 0; i < progenitores.size(); i++) {
		for (int j = 0; j < progenitores.size(); j++) {
			if (i != j) {
				vector<int> individuo = cruzar_padres(progenitores[i], progenitores[j]);
				if (es_individuo_valido(individuo, paquetes, camiones)){
					poblacion.push_back(individuo);
				}
			}
		}
	}
}

void eliminar_duplicados(vector<vector<int>> & poblacion) {
	sort(poblacion.begin(), poblacion.end());
	poblacion.erase(unique(poblacion.begin(), poblacion.end()), poblacion.end());
}

void ordenar_reducir_poblacion(vector<vector<int>> &poblacion, const vector<int> & paquetes, const vector<int> & camiones) {
	sort(poblacion.begin(), poblacion.end(), [paquetes, camiones](const vector<int> & a, const vector<int> & b){
		return calcular_aptitud(a, paquetes, camiones) > calcular_aptitud(b, paquetes, camiones);
	});

	if (poblacion.size() > N_INDIVIDUOS) {
		poblacion.erase(poblacion.begin() + N_INDIVIDUOS, poblacion.end());
	}
}

vector<vector<int>> optimizar_asignaciones(const vector<int> & paquetes, const vector<int> & camiones) {

	vector<vector<int>> poblacion = construir_poblacion_inicial(paquetes, camiones);

	vector<int> mejor_individuo;
	int mejor_aptitud = -1;

	for (int i = 0; i < N_GENERACIONES; i++) {
		vector<vector<int>> progenitores = seleccionar_progenitores(poblacion, paquetes, camiones);
		reproducir_poblacion(poblacion, progenitores, paquetes, camiones);
		eliminar_duplicados(poblacion);
		ordenar_reducir_poblacion(poblacion, paquetes, camiones);

		vector<int> candidato = poblacion[0];
		if (calcular_aptitud(candidato, paquetes, camiones) > mejor_aptitud) {
			mejor_individuo = candidato;
			mejor_aptitud = calcular_aptitud(candidato, paquetes, camiones);
		}
	}

	vector<vector<int>> asignaciones(camiones.size());

	for (int i = 0; i < camiones.size(); i++) {
		for (int j = 0; j < paquetes.size(); j++) {
			int idx = i * paquetes.size() + j;
			if (mejor_individuo[idx]) asignaciones[i].push_back(j);
		}
	}

	return asignaciones;
}

void imprimir_asignaciones(const vector<vector<int>> & asignaciones) {
	for (int i = 0; i < asignaciones.size(); i++) {
		cout << "Camion " << i + 1 << ": ";
		for (int j = 0; j < asignaciones[i].size(); j++) {
			cout << asignaciones[i][j] + 1 << " ";
		}
		cout << endl;
	}
}

int main(int argc, char *argv[]) {
	srand(time(nullptr));

	const vector<int> paquetes = {150, 100, 180, 50, 120, 10};

	const vector<int> camiones = {250, 200, 200, 100};

	const vector<vector<int>> asignaciones = optimizar_asignaciones(paquetes, camiones);

	imprimir_asignaciones(asignaciones);
}