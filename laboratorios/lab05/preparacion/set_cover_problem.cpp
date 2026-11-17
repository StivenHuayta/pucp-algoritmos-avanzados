#include <iostream>
#include <iomanip>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <map>
#include <algorithm>

#define N_INDIVIDUOS 30
#define N_GENERACIONES 100
#define TASA_MUTACION 0.15

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

vector<vector<int>> resolver_set_cover_problem(const vector<int> & universo, const vector<vector<int>> & subconjuntos) {
	const int numero_elementos = universo.size();
	const int numero_subconjuntos = subconjuntos.size();
	
	vector<vector<int>> poblacion = construir_poblacion_inicial(universo, subconjuntos);
	
	vector<vector<int>> solucion;
	
	return solucion;
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
	
	const vector<vector<int>> solucion = resolver_set_cover_problem(universo, subconjuntos);
}