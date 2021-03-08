#include "input.hpp"

void leer_predios_scv(Caso &caso, char *nombre_archivo, float precio_megawatt, float costo_transp){
	caso.predios.clear();
	caso.precio_megawatthora = precio_megawatt;
	caso.costo_transporte = costo_transp;
	int ronda= 0;
	ifstream archivo(nombre_archivo);
	if(!archivo.good()){
		printf("No se puede leer \"%s\".",nombre_archivo);
		exit(1);
	}
	string valor;

	Predio pred;
	while(archivo.good()){
		getline(archivo,valor,';');
		if(!archivo.good()) break;
		if(valor!="" and valor!="\r\n" and valor!="\n" and valor!="\n\n" and valor!="\r\n\r\n"){
			switch(ronda){
				case 0:
					pred.nombre= (valor[0]=='\n')? valor.substr(1):valor;
				break;
				case 1:
					pred.pos_x= atof(valor.c_str());
				break;
				case 2:
					pred.pos_y= atof(valor.c_str());
				break;
				case 3:
					pred.vacas= atoi(valor.c_str());
					caso.predios.push_back(pred);
				break;
			}
			ronda= (ronda+1)%4;
		}
	}
}

void obtener_distancias(Caso &caso, char *nombre_archivo){
	for(int k=0;k<caso.predios.size()*caso.predios.size();k++) caso.distancias.push_back(0);
	for(int p1=0;p1<caso.predios.size();p1++){
		for(int p2=p1+1;p2<caso.predios.size();p2++){
			float distancia;
			// Parte auxiliar:
			float delta_x= caso.predios[p1].pos_x-caso.predios[p2].pos_x;
			float delta_y= caso.predios[p1].pos_y-caso.predios[p2].pos_y;
			distancia= sqrt(delta_x*delta_x+delta_y*delta_y);
			//
			caso.distancias[p1*caso.predios.size()+p2]= distancia;
			caso.distancias[p2*caso.predios.size()+p1]= distancia;
		}
	}
}
