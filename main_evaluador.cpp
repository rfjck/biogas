#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <iostream>
#include <sstream>

#include "optimizacion.hpp"
#include "economia.hpp"
#include "input.hpp"
#include "output.hpp"

using namespace std;

int main(int argc, char const *argv[]) {
    Caso caso;

    // Leer csv.
    char bases_fname[1000];
    printf("Inserte archivo con bases originales (ejemplo \"input/bases.csv\") :\n");
    scanf("%s",bases_fname);

    // Leer disponibilidad de biomasa
    float factor_masa;
    printf("Inserte %% biomasa (ejemplo \"100.0\") : \n");
    scanf("%f",&factor_masa);
    factor_masa /= 100.0;

    // Leer precio energia
    float precio_energia;
    printf("Inserte precio MWh (ejemplo: \"86.0\") : \n");
    scanf("%f",&precio_energia);

    // Leer costo transporte
    float costo_transporte;
    printf("Inserte costo transporte tonKm (ejemplo: \"0.3\") : \n");
    scanf("%f",&costo_transporte);


    printf("Leyendo \"%s\"...\n",bases_fname);
	leer_predios_scv(caso,bases_fname,precio_energia,costo_transporte);
    caso.multiplicar_vacas(factor_masa);

    // Hacer que el caso precompute su matriz de distancias:
	obtener_distancias(caso,NULL);


    // Leer archivo de Nabuco
    char argis_fname[1000];
    printf("Inserte arhivo del arcGis (ejemplo \"input/yave10000.txt\") :\n");
    scanf("%s",argis_fname);

    vector<int> id_plantas;
    vector<int> id_predios;

    printf("Leyendo \"%s\"...\n",argis_fname);

    {
        ifstream archivo(argis_fname);
	    if(!archivo.good()){
            printf("No se puede leer \"%s\".",argis_fname);
            exit(1);
        }
        string valor;
        while(archivo.good()){
            char buffer[1000];
		    getline(archivo,valor,';');
            strcpy(buffer,valor.c_str());
            buffer[8] = '\0';
            if(strcmp(buffer,"Location")==0){
                int a,b;
                int read = sscanf(valor.c_str(),"%*s %d %*s %*s %d",&a,&b);
                id_plantas.push_back(a-1);
                id_predios.push_back(b-1);
            }
        }
    }

    // Ver si se descartan asignaciones malas
    int modo;
    printf("Modos: \n");
    printf("    0 = No cambiar asignaciones.\n");
    printf("    1 = Solo eliminar aportadores negativos.\n");
    printf("    2 = Reevaluar asignaciones.\n");
    printf("Inserte modo: \n");
    scanf("%d",&modo);
    assert(modo==0 || modo==1 || modo==2);

    vector<Configuracion> configs;
    for(int i=0;i<id_plantas.size();i++){
        int id_p = id_plantas[i];

        int config_id = -1;
        for(int k=0;k<configs.size();k++){
            if(id_p==configs[k].id_predio_base) config_id = k;
        }
        if(modo==0 || modo==1){
            if(config_id==-1){
                configs.push_back(Configuracion(id_p));
                config_id = configs.size()-1;
            }
            configs[config_id].id_predios.insert(id_predios[i]);
        }else if(modo==2){
            if(config_id==-1){
                Configuracion confi = mejor_configuracion(caso,id_p);
    		    if(calcular_ganancia_aprovechando_precalc(caso,Combinacion(confi))>0) configs.push_back(confi);
                config_id = configs.size()-1;
            }
        }
    }

    if(configs.size()==0){
        printf("No es rentable. No se guardará archivo.\n");
        exit(1);
    }

    if(modo==1){
        printf("Eliminando aportadores negativos...\n");
        for(int i=0;i<configs.size();i++){
            configs[i].eliminar_aportadores_negativos(caso);
        }
    }

    Combinacion combi(configs[0]);
    for(int i=1;i<configs.size();i++){
        if(modo==2) printf("Agregando configuración %d/%d...\n",i,(int)configs.size());
        combi.agregar_configuracion(caso,configs[i],CONTROLAR_PREDIOS_APORTADORES_OBSESIVAMENTE);
    }

    // Escribir calculos finales

    printf("Calculando VAN...\n");
    vector<Calculo> calcs;
    float ganancia = Economia::calcular_ganancia(caso,combi,&calcs);
    cout << "ganancia :\t" << ganancia << endl;

    // move after the first "/", also delete after the dot in argis_fname
    char *keyname_start = argis_fname;
    for(int i=0;i<strlen(argis_fname);i++){
        if(argis_fname[i]=='.') argis_fname[i]='\0';
        if(argis_fname[i]=='/') keyname_start = argis_fname+i+1;
    }

    ostringstream nombre_archivo;
    nombre_archivo << argv[1] << "/" << keyname_start << "_" << factor_masa << "_" << precio_energia << "_" << costo_transporte << "_" << modo << "_predios.csv";
    escribir_combinacion_en_archivo(caso,combi,nombre_archivo.str().c_str());

    ostringstream nombre_archivo_econom;
    nombre_archivo_econom << argv[2] << "/" << keyname_start << "_" << factor_masa << "_" << precio_energia << "_" << costo_transporte << "_" << modo << "_econom.csv";
    escribir_calculos(caso,calcs,nombre_archivo_econom.str().c_str());

    printf("Hecho!\n");
}
