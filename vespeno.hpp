#ifndef VESPENO_H
#define VESPENO_H

#include "lib/libhungarian/hungarian.h"
#include "general.hpp"

#include <set>
#include <vector>
#include <iostream>
#include <string>
#include <algorithm>

#include <stdio.h>
#include <math.h>

using namespace std;

struct PreCalc{
	bool valido;
	float ganancia;
};
class Caso;

struct Predio{
	int pos_x,pos_y;
	int vacas;
	string nombre;
	ostream &imprimir(ostream &os);
};

struct Configuracion{
	int id_predio_base; // Predio que tiene la planta.
	set<int> id_predios; // Conjunto con los ids de los predios que alcanza la planta.

	Configuracion(int id_base); // Contructor.
	ostream &imprimir(ostream &os, Caso &caso);

	//| definido en optimización.cpp
	vector<int> eliminar_aportadores_negativos(Caso &caso);
	// ^ Elimina los predios que restan ganancia (los más lejanos).

	// | definido en optimización.cpp
	void agregar_aportadores_positivos(Caso &caso, vector<int> &predios);
	// ^ Agrega predios que aumenta la ganancia (en el borde).
};

struct Combinacion{
	vector<Configuracion> configs;
	PreCalc precalc;

	// Crea una combinación de 1 configuración:
	Combinacion(Configuracion &config);

	// Agrega una configuración adicional a la configuración, repartiendo los predios en disputa y reajustando los predios de cada configuración producto de modificaciones de la eficiencia.
	void agregar_configuracion(Caso &caso, Configuracion &config, int obsesividad);
	//^ definido en optimización.cpp

	ostream &imprimir(ostream &os, Caso &caso);
	ostream &imprimir_resumido(ostream &os);
	int get_hash(int id_predio_base_extra=-1);
	friend bool operator== (Combinacion &c1, Combinacion &c2);
	friend bool operator!= (Combinacion &c1, Combinacion &c2);
};
int distancia_correlacion(Caso &caso, Combinacion &comb1, Combinacion &comb2);

class Caso{
	float precio_megawatthora;
	float costo_transporte;
	friend void leer_predios_scv(Caso &caso, char *nombre_archivo, float precio_megawatt, float costo_transp);
	friend void obtener_distancias(Caso &caso, char *nombre_archivo);
	vector<int> distancias;
	vector<Predio> predios;

public:
	Predio get_predio(int id) const;
	float get_distancia(int id1, int id2) const;
	int get_cantidad_predios() const;
	float get_costo_transporte() const;
	float get_precio_megawatthora() const;
	void multiplicar_vacas(float factor);
};

inline ostream& Predio::imprimir(ostream& os){
	os << "(" << pos_x << "," << pos_y << "):" << vacas << "\t" << nombre;
	return os;
}

inline ostream &Configuracion::imprimir(ostream &os, Caso &caso){
	os << "Configuracion:" << endl;
	os << "\tPredio base: " << id_predio_base << endl;
	os << "\t\t"; caso.get_predio(id_predio_base).imprimir(os); os << endl;
	os << "\tPredios alcanzados:" << endl;
	for(auto idp:id_predios){
		os << "\t\t"; caso.get_predio(idp).imprimir(os); os <<endl;
	}
	return os;
}

inline ostream &Combinacion::imprimir(ostream &os, Caso &caso){
	os << "Combinacion:" << endl;
	for(auto &confi: configs){
		os << "\tConfiguracion:" << endl;
		os << "\t\tPredio base: " << confi.id_predio_base << endl;
		os << "\t\t\t"; caso.get_predio(confi.id_predio_base).imprimir(os); os << endl;
		os << "\t\tPredios alcanzados:" << endl;
		for(auto idp:confi.id_predios){
			os << "\t\t\t"; caso.get_predio(idp).imprimir(os); os <<endl;
		}
	}
	return os;
}

inline ostream &Combinacion::imprimir_resumido(ostream &os){
	for(auto &confi: configs){
		os << confi.id_predio_base << " ";
	}
	os << endl;
	return os;
}

#endif
