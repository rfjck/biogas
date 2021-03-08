#include "vespeno.hpp"

Configuracion::Configuracion(int id_base){
	id_predio_base= id_base;
}

Combinacion::Combinacion(Configuracion &config){
	configs.push_back(config);
	precalc.valido=false;
}

int Combinacion::get_hash(int id_predio_base_extra){
	vector<int> id_preds;
	if(id_predio_base_extra>=0) id_preds.push_back(id_predio_base_extra);
	for(int k=0;k<configs.size();k++){
		id_preds.push_back(configs[k].id_predio_base);
	}
	sort(id_preds.begin(),id_preds.end());
	int a=0;
	int hash=0;
	for(int p:id_preds){
		int extra= (p<<((10*a)%21));
		hash^= extra;
		a++;
	}
	return hash;
}

Predio Caso::get_predio(int id) const{
	return predios[id];
}

float Caso::get_distancia(int id1, int id2) const{
	return distancias[id1*predios.size()+id2];
}

int Caso::get_cantidad_predios() const{
	return predios.size();
}

float Caso::get_costo_transporte() const{
	return costo_transporte;
}
float Caso::get_precio_megawatthora() const{
	return precio_megawatthora;
}

void Caso::multiplicar_vacas(float factor){
	for(int k=0;k<predios.size();k++){
		predios[k].vacas = (int) round(predios[k].vacas*factor);
	}
}

int distancia_correlacion(Caso &caso, Combinacion &comb1, Combinacion &comb2){
	// Si la cantidad de configuraciones es diferente lanzar error.
	if(comb1.configs.size()!=comb2.configs.size()){
		cout << "Comparando combinaciones de tamaño diferente." << endl;
		throw;
	}
	// Asignar a cada configuracion de comb1 una de comb2 minimizando la suma
	// de las distancias (Hungarian algorithm).
	int size= comb1.configs.size();
	int **mtrx= (int**) malloc(sizeof(int*)*size);
	int *vals= (int*) malloc(sizeof(int)*size*size);
	for(int k=0;k<size;k++) mtrx[k]= &(vals[size*k]);
	for(int p1=0;p1<size;p1++){
		for(int p2=0;p2<size;p2++){
			vals[p1*size+p2]= (int) caso.get_distancia(comb1.configs[p1].id_predio_base,comb2.configs[p2].id_predio_base);
		}
	}
	// Resolver asignación.
	hungarian_problem_t hprob;
	hungarian_init(&hprob, mtrx, size, size, HUNGARIAN_MODE_MINIMIZE_COST);
	hungarian_solve(&hprob);
	// Sumar distancias con la matriz de asignación resultante.
	int cost=0;
	for(int p1=0;p1<size;p1++){
		for(int p2=0;p2<size;p2++){
			cost+= vals[p1*size+p2]*hprob.assignment[p1][p2];
		}
	}
	// Liberar memoria.
	free(vals);
	free(mtrx);
	hungarian_free(&hprob);
	// Retornar inverso del costo calculado.
	return cost;
}

bool operator== (Combinacion &c1, Combinacion &c2){
	if(c1.configs.size()!=c2.configs.size()) return false;
	bool iguales=true;
	for(int k=0;k<c1.configs.size();k++){
		bool presente=false;
		int prbase= c1.configs[k].id_predio_base;
		for(int k2=0;k2<c2.configs.size();k2++){
			if(c2.configs[k2].id_predio_base==prbase){
				presente=true;
				break;
			}
		}
		if(!presente){
			iguales=false;
			break;
		}
	}
	return iguales;
}
bool operator!= (Combinacion &c1, Combinacion &c2){
	return !(c1==c2);
}
