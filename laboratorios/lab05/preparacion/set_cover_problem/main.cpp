#include <iostream>
#include <iomanip>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <map>
#include <algorithm>
#include <cmath>

#define N_INDIVIDUOS 30
#define N_GENERACIONES 100
#define TASA_MUTACION 0.15
#define TASA_SELECCION 0.3
#define PUNTO_CASAMIENTO 0.5

using namespace std;

bool es_cromosoma_valido(const vector<int> & candidato, const vector<int> & universo, const vector<vector<int>> & subconjuntos) {

	vector<int> cubiertos;

	for (int i = 0; i < candidato.size(); i++) {
		if (candidato[i]) {
			for (int j = 0; j < subconjuntos[i].size(); j++) {
				cubiertos.push_back(subconjuntos[i][j]);
			}
		}
	}

	for (int i = 0; i < universo.size(); i++) {
		if (find(cubiertos.begin(), cubiertos.end(), universo[i]) == cubiertos.end()) return false;
	}

	return true;
}

vector<vector<int>> construir_poblacion_inicial(const vector<int> & universo, const vector<vector<int>> & subconjuntos) {
	const int numero_elementos = universo.size();
	const int numero_subconjuntos = subconjuntos.size();

	const int longitud_cromosoma = numero_subconjuntos;

	vector<vector<int>> poblacion;

	while (poblacion.size() < N_INDIVIDUOS) {
		vector<int> candidato(longitud_cromosoma, 0);

		for (int i = 0; i < numero_subconjuntos; i++) {
			candidato[i] = rand() % 2;
		}

		if (es_cromosoma_valido(candidato, universo, subconjuntos)) {
			poblacion.push_back(candidato);
		}
	}

	return poblacion;
}

int calcular_aptitud(const vector<int> & individuo) {
	int aptitud = 0;
	for (int i = 0; i < individuo.size(); i++) {
		if (!individuo[i]) aptitud++;
	}
	return aptitud;
}

vector<vector<int>> seleccionar_progenitores(const vector<vector<int>> & poblacion,
                                             const vector<int> & universo, const vector<vector<int>> & subconjuntos) {
	vector<int> puntajes(poblacion.size());
	int suma_puntajes = 0;
	for (int i = 0; i < poblacion.size(); i++) {
		puntajes[i] = calcular_aptitud(poblacion[i]);
		suma_puntajes += puntajes[i];
	}

	int cantidad_padres = (int)round(poblacion.size() * TASA_SELECCION);

	vector<vector<int>> progenitores;

	for (int i = 0; i < cantidad_padres; i++) {
		int tiro_ruleta = rand() % suma_puntajes;
		int acumulado = 0;

		for (int j = 0; j < poblacion.size(); j++) {
			acumulado += puntajes[j];
			if (acumulado > tiro_ruleta) {
				progenitores.push_back(poblacion[j]);
				break;
			}
		}
	}

	return progenitores;
}

vector<int> cruzar_padres(const vector<int> & padre, const vector<int> & madre) {
	vector<int> hijo;
	int punto_corte = (int)round(padre.size() * PUNTO_CASAMIENTO);

	for (int i = 0; i < punto_corte; i++) {
		hijo.push_back(padre[i]);
	}

	for (int j = punto_corte; j < madre.size(); j++) {
		hijo.push_back(madre[j]);
	}
	return hijo;
}

void reproducir_poblacion(vector<vector<int>> & poblacion, const vector<vector<int>> & progenitores,
                          const vector<int> & universo, const vector<vector<int>> & subconjuntos) {
	for (int i = 0; i < progenitores.size(); i++) {
		for (int j = 0; j < progenitores.size(); j++) {
			if (i != j) {
				vector<int> hijo = cruzar_padres(progenitores[i], progenitores[j]);
				if (es_cromosoma_valido(hijo, universo, subconjuntos)) {
					poblacion.push_back(hijo);
				}
			}
		}
	}
}

void eliminar_duplicados(vector<vector<int>> & poblacion) {
	sort(poblacion.begin(), poblacion.end());
	poblacion.erase(unique(poblacion.begin(), poblacion.end()), poblacion.end());
}

void renovar_poblacion(vector<vector<int>> & poblacion, const vector<int> & universo, const vector<vector<int>> & subconjuntos) {
	eliminar_duplicados(poblacion);

	sort(poblacion.begin(), poblacion.end(), [](const vector <int> & a, const vector <int> & b) {
		return calcular_aptitud(a) > calcular_aptitud(b);
	});

	if (poblacion.size() > N_INDIVIDUOS) {
		poblacion.erase(poblacion.begin() + N_INDIVIDUOS, poblacion.end());
	}
}

vector<int> resolver_set_cover_problem(const vector<int> & universo, const vector<vector<int>> & subconjuntos) {
	const int numero_elementos = universo.size();
	const int numero_subconjuntos = subconjuntos.size();

	vector<vector<int>> poblacion = construir_poblacion_inicial(universo, subconjuntos);

	vector<int> solucion;
	int aptitud_maxima = -1;

	for (int i = 0; i < N_GENERACIONES; i++) {
		vector<vector<int>> progenitores = seleccionar_progenitores(poblacion, universo, subconjuntos);
		reproducir_poblacion(poblacion, progenitores, universo, subconjuntos);
		renovar_poblacion(poblacion, universo, subconjuntos);

		int aptitud_actual = calcular_aptitud(poblacion[0]);
		if (aptitud_actual > aptitud_maxima) {
			aptitud_maxima = aptitud_actual;
			solucion = poblacion[0];
		}
	}

	return solucion;
}

void imprimir_solucion(const vector<int> & solucion, const vector<int> & universo,
					   const vector<vector<int>> & subconjuntos) {
	int cantidad_subconjuntos = 0;
	for (int i = 0; i < solucion.size(); i++) {
		if (solucion[i]) {
			cout << "Subconjunto " << i+1 << endl;
			cantidad_subconjuntos++;
		}
	}
	cout << "Total: " << cantidad_subconjuntos << endl;
}

int main() {
	// Problema de la cobertura de conjuntos
	// Objetivo: Seleccionar la menor cantidad de subconjuntos que "cubran" al conjunto U

	srand(time(nullptr));

	const vector<int> universo = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

	const vector<vector<int>> subconjuntos = {
		{1, 2, 3},
		{2, 4, 5},
		{3, 5, 6, 7},
		{1, 6, 8},
		{4, 7, 9, 10},
		{8, 9, 10},
		{1, 2, 5, 8}
	};

	const vector<int> solucion = resolver_set_cover_problem(universo, subconjuntos);

	imprimir_solucion(solucion, universo, subconjuntos);

	return 0;
}
