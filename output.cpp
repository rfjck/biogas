#include "output.hpp"

void escribir_combinacion_en_archivo(const Caso &caso, Combinacion &combi, const char *nombre_archivo){
	ofstream archivo;
	cout << "Escribiendo "<< nombre_archivo << endl;
	archivo.open(nombre_archivo);
	archivo << "ORIG_INDX,ORIG_X,ORIG_Y,OTRO_INDX,OTRO_X,OTRO_Y,VACAS" << endl;
	for(int h=0;h<combi.configs.size();h++){
		Configuracion &config= combi.configs[h];
		for(auto id_predio:config.id_predios){
			archivo << config.id_predio_base+1 << ",";
			archivo << caso.get_predio(config.id_predio_base).pos_x << "," << caso.get_predio(config.id_predio_base).pos_y << ",";
			archivo << id_predio+1 << ",";
			archivo << caso.get_predio(id_predio).pos_x << "," << caso.get_predio(id_predio).pos_y << ",";
			archivo << caso.get_predio(id_predio).vacas << endl;
		}
	}
	archivo.close();
}

void escribir_calculos(const Caso &caso, vector<Calculo> &calculos, const char *nombre_archivo){
	ofstream archivo;
	cout << "Escribiendo "<< nombre_archivo << endl;
	archivo.open(nombre_archivo);
	bool first= true;
	for(Calculo &calc:calculos){
		//Se escribe a archivo:
		calc.imprimir(archivo,caso,first);
		first=false;
	}
	//Agregar la fila suma al archivo.
	Calculo suma;
	for(Calculo &clc:calculos){
		suma.aniadir(clc);
	}
	suma.imprimir(archivo,caso,false);
	archivo.close();
}
