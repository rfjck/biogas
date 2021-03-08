#include <string>
#include <iostream>
#include <sstream>
#include "optimizacion.hpp"
#include "economia.hpp"
#include "input.hpp"
#include "output.hpp"

#define TAMANIO_POOL 1000
#define CANTIDAD_MEJORES_BUSCADA 100
#define RANGO_DETECCION_SEMEJANTES -1

using namespace std;

int main(int argc, char const *argv[]) {
	// Comprobar que la cantidad de parámetros de entrada está correcta
	if(argc!=4){
		cout << "usage: " << argv[0] << " <bases_file> <predios_out_folder> <econom_out_folder>";
		exit(1);
	}
	//
	Caso caso;
	vector<Configuracion> configs;
    // Leer csv.
	leer_predios_scv(caso,(char *)argv[1],PRECIO_MEGAWATTHORA,C_TRANSPORTE_POR_TONKM);
	// Reducir % de biomasa
	caso.multiplicar_vacas(PORCENTAJE_DISPONIBILIDAD_BIOMASA/100.0);
    // Hacer que el caso precompute su matriz de distancias:
	obtener_distancias(caso,NULL);
    // Llamada principal al algoritmo:
	vector<Combinacion> combis= obtener_mejores_combinaciones(caso, TAMANIO_POOL, CANTIDAD_MEJORES_BUSCADA, CONTROLAR_PREDIOS_APORTADORES_OBSESIVAMENTE, RANGO_DETECCION_SEMEJANTES, NULL);
    // Salida a archivos:
	for(int k=0;k<combis.size();k++){
		vector<Calculo> calcs;
		float ganancia= Economia::calcular_ganancia(caso,combis[k],&calcs);
		cout << "ganancia " << k << ":\t" << ganancia << endl;
		cout << "pre-calc " << k << ":\t" << calcular_ganancia_aprovechando_precalc(caso,combis[k]) << endl;

		ostringstream nombre_archivo;
		nombre_archivo << argv[2] << "/mejor_" << (k+1) << "_predios.csv";
		escribir_combinacion_en_archivo(caso,combis[k],nombre_archivo.str().c_str());

		ostringstream nombre_archivo_econom;
		nombre_archivo_econom << argv[3] << "/mejor_" << (k+1) << "_econom.csv";
		escribir_calculos(caso,calcs,nombre_archivo_econom.str().c_str());
	}
	return 0;
}
