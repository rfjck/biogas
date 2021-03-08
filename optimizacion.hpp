#ifndef OPTIMIZACION_H
#define OPTIMIZACION_H

#include <climits>
#include <algorithm>
#include <vector>
#include <list>
#include <set>
#include <sstream>
#include "vespeno.hpp"
#include "economia.hpp"
#include "output.hpp"

struct ParDistancia{
	float distancia;
	int id_predio;
};

Configuracion mejor_configuracion(Caso &caso, int id_predio);
void ordenar_por_ganancia(Caso &caso, vector<Combinacion> &combis);
void eliminar_similares(Caso &caso, vector<Combinacion> &combis, int cantidad_objetivo, int rango_vision=-1);
vector<Combinacion> obtener_mejores_combinaciones(Caso &caso, int tamanio_pool, int cantidad_mejores, int obsesividad, int rango_vision=-1, const char *carpeta_intermedios=NULL);

inline float calcular_ganancia_aprovechando_precalc(const Caso &caso, const Combinacion &combi){
	if(combi.precalc.valido) return combi.precalc.ganancia;
	else return Economia::calcular_ganancia(caso,combi);
}

inline float calcular_ganancia_y_guardar_precalc(const Caso &caso, Combinacion &combi){
	float ganancia= calcular_ganancia_aprovechando_precalc(caso,combi);
	combi.precalc.valido=true;
	combi.precalc.ganancia= ganancia;
	return ganancia;
}



#endif
