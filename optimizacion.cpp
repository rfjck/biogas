#include "optimizacion.hpp"

bool menor_distancia(const ParDistancia &p1,const ParDistancia &p2){
	return p1.distancia<p2.distancia;
}
Configuracion mejor_configuracion(Caso &caso, int id_predio){
	// Para el predio con la id dada, encuentra la mejor configuración que lo usa como predio-planta.
	// Ordenar los otros predios de menor a mayor distancia a este predio-planta.
	vector<ParDistancia> dists;
	for(int k=0;k<caso.get_cantidad_predios();k++){
		ParDistancia par;
		par.distancia=caso.get_distancia(id_predio,k);
		par.id_predio=k;
		dists.push_back(par);
	}
	sort(dists.begin(),dists.end(),menor_distancia);

	// Crear una configuración vacía.
	Configuracion confi(id_predio);
	Configuracion mejor_configuracion=confi;
	float mejor_ganancia= -1.0/0.0;
	for(int p=0;p<dists.size();p++){
		// Modificar la convinación para agregar el siguiente predio más cercano.
		confi.id_predios.insert(dists[p].id_predio);
		Combinacion combi= Combinacion(confi);
		float ganancia= calcular_ganancia_y_guardar_precalc(caso,combi);
		if(ganancia>=mejor_ganancia){
			// Guardar la configuración si es la de mejor ganancia.
			mejor_configuracion= confi;
			mejor_ganancia= ganancia;
		}
	}
	// Entregar la configuración de mejor ganancia.
	return mejor_configuracion;
}

struct Colision{
	int id_predio;
	int propietario_original;
	float distancia_nuevo;
	float distancia_original;
};
bool comparar_colisiones(const Colision &c1, const Colision &c2){
	// Esta función se utiliza para ordenar los predios en colisión del más relativamente lejano al predio nuevo, al más relativamente cercano (relativo, porque se considera la distancia al predio antiguo).
	return (c1.distancia_original*c2.distancia_nuevo)<(c2.distancia_original*c1.distancia_nuevo);
}
void Combinacion::agregar_configuracion(Caso &caso, Configuracion &config, int obsesividad){
	precalc.valido=false;
	vector<Colision> colisiones;
	//Encontrar colisiones:
	for(int id:config.id_predios){
		for(int k=0;k<configs.size();k++){
			if(configs[k].id_predios.count(id)){
				Colision coli;
				coli.id_predio=id;
				coli.propietario_original=k;
				colisiones.push_back(coli);
			}
		}
	}
	//Calcular distancias para las colisiones:
	for(auto &coli:colisiones){
		coli.distancia_nuevo= caso.get_distancia(coli.id_predio,config.id_predio_base);
		coli.distancia_original= caso.get_distancia(coli.id_predio,configs[coli.propietario_original].id_predio_base);
	}
	//Ordenar colisiones de la más relativamente lejana al predio nuevo a la menos.
	sort(colisiones.begin(),colisiones.end(),comparar_colisiones);
	//Eliminar todos los predios en disputa de las configuraciones originales.
	for(auto &coli:colisiones){
		configs[coli.propietario_original].id_predios.erase(coli.id_predio);
	}
	precalc.valido=false;
	if(obsesividad>=2){
		//Para cada configuracion original, eliminar los predios que ahora aportan negativo,
		//pero guardarlos.
		vector<vector<int> > aportadores_eliminados;
		for(Configuracion &configuracion_original:configs){
			aportadores_eliminados.push_back(configuracion_original.eliminar_aportadores_negativos(caso));
		}
		//Agregar configuracion nueva a la combinacion.
		configs.push_back(config);
		Configuracion &confi_interna= configs[configs.size()-1];
		//Evaluar para cada predio en el orden dado si conviene regresarlo a la configuracion original:
		float ganancia_actual= calcular_ganancia_aprovechando_precalc(caso,*this);
		vector<set<int> > predios_actual; for(Configuracion &confi:configs) predios_actual.push_back(confi.id_predios);
		for(auto &coli:colisiones){
			if(confi_interna.id_predios.count(coli.id_predio)){
				confi_interna.id_predios.erase(coli.id_predio);
				confi_interna.eliminar_aportadores_negativos(caso);
				configs[coli.propietario_original].id_predios.insert(coli.id_predio);
				configs[coli.propietario_original].agregar_aportadores_positivos(caso,aportadores_eliminados[coli.propietario_original]);
				float nueva_ganancia= calcular_ganancia_aprovechando_precalc(caso,*this);
				if(nueva_ganancia>ganancia_actual){
					ganancia_actual= nueva_ganancia;
					//Actualizar predios_actual.
					predios_actual[configs.size()-1]= confi_interna.id_predios;
					predios_actual[coli.propietario_original]= configs[coli.propietario_original].id_predios;
				}else{
					//Restablecer paso.
					confi_interna.id_predios= predios_actual[configs.size()-1];
					configs[coli.propietario_original].id_predios= predios_actual[coli.propietario_original];
				}
			}
		}
	}else{
		//Agregar configuracion nueva a la combinacion.
		configs.push_back(config);
		Configuracion &confi_interna= configs[configs.size()-1];
		//Evaluar para cada predio en el orden dado si conviene regresarlo a la configuracion original:
		float ganancia_actual= calcular_ganancia_y_guardar_precalc(caso,*this);
		for(auto &coli:colisiones){
			confi_interna.id_predios.erase(coli.id_predio);
			configs[coli.propietario_original].id_predios.insert(coli.id_predio);
			precalc.valido=false;
			float nueva_ganancia= calcular_ganancia_y_guardar_precalc(caso,*this);
			if(nueva_ganancia>ganancia_actual){
				ganancia_actual= nueva_ganancia;
			}else{
				configs[coli.propietario_original].id_predios.erase(coli.id_predio);
				confi_interna.id_predios.insert(coli.id_predio);
			}
		}
	}
	if(obsesividad>=1){
		for(Configuracion &configuracion:configs){
			configuracion.eliminar_aportadores_negativos(caso);
		}
		precalc.valido=false;
	}
}
vector<int> Configuracion::eliminar_aportadores_negativos(Caso &caso){
	//Elimina los predios desde los mas lejanos a los más cercanos por si algún
	//factor hizo que el predio deje de ser rentable.
	vector<int> borrados;
	//Obtener los predios actuales ordenados por distancia:
	vector<ParDistancia> dists;
	for(int id_predio:id_predios){
		ParDistancia par;
		par.id_predio=id_predio;
		par.distancia=caso.get_distancia(id_predio_base,id_predio);
		dists.push_back(par);
	}
	sort(dists.begin(),dists.end(),menor_distancia);
	//v Calcular la ganancia inicial.
	Combinacion comb(*this);
	float ganancia_actual= Economia::calcular_ganancia(caso,comb);
	for(int k=dists.size()-1;k>=0;k--){
		//Borrar el predio más lejano.
		id_predios.erase(dists[k].id_predio);
		Combinacion combi(*this);
		float nueva_ganancia= Economia::calcular_ganancia(caso,combi);
		//v Si la ganancia queda menor, detenerse y revertir.
		if(nueva_ganancia<ganancia_actual){
			id_predios.insert(dists[k].id_predio);
			break;
		}
		//v Si la ganancia queda mayor, actualizar la ganancia actual.
		ganancia_actual=nueva_ganancia;
		borrados.push_back(dists[k].id_predio);
	}
	return borrados;
}
void Configuracion::agregar_aportadores_positivos(Caso &caso, vector<int> &predios){
	//Evalúa si vale la pena insertar perdios a la configuracion, en orden.
	if(predios.size()==0) return;
	Combinacion comb(*this);
	float ganancia_actual= Economia::calcular_ganancia(caso,comb);
	for(int k=predios.size()-1;k>=0;k--){
		if(!id_predios.count(predios[k])){
			id_predios.insert(predios[k]);
			Combinacion combi(*this);
			float nueva_ganancia= Economia::calcular_ganancia(caso,combi);
			if(nueva_ganancia<ganancia_actual){
				id_predios.erase(predios[k]);
				break;
			}
			ganancia_actual=nueva_ganancia;
		}
	}
}

void ordenar_por_ganancia(Caso &caso, vector<Combinacion> &combis){
	// Ordena una serie de combinaciones según ganancia.
	//Precalcular para evitar recálculos en cada sort.
	for(int k=0;k<combis.size();k++) calcular_ganancia_y_guardar_precalc(caso,combis[k]);
	sort(combis.begin(),combis.end(),[caso](const Combinacion &c1,const Combinacion &c2){ return calcular_ganancia_aprovechando_precalc(caso,c1)>calcular_ganancia_aprovechando_precalc(caso,c2);});
}

void eliminar_similares(Caso &caso, vector<Combinacion> &combis, int cantidad_objetivo, int rango_vision){
	// Este algoritmo trabaja como clustering de combinaciones por agregación, sin calcular la distancia entre todas las combinaciones, puesto que aprovecha el ordenamiento por ganancia como una pista para encontrar los clusters (Sólo se comparan Combinaciones de ganancias parecidas). Además, el algoritmo no busca exactamente grupos, sino reducir una población de Combinaciones a miembros representativos de sus propios grupos, es decír, cuando se realiza una "agregación", en realidad se elimina el agregado con menor ganancia, quedando solo el con mayor, esta aproximación parece razonable dado que es probable que la Combinación de mayor ganancia de un grupo también sea el medoide del mismo (por lo tanto, mantenerlo asegurará el no hacer aparecer grupos previamente inexistentes (producto de la eliminación de los datos en este orden)).
	cout << "\tOrdenando por ganancia..." << endl;
	ordenar_por_ganancia(caso,combis);
	int cantidad_combis= combis.size();
	if(rango_vision<0) rango_vision= combis.size()/cantidad_objetivo+1; //Cantidad recomendada.
	cout << "\tCalculando distancia de correlacion (con rango "<< rango_vision << ")..." << endl;
	// Calcular las distancias correlación para cada Combinacion entre sus
	// siguientes, dentro del rango de visión (también aprovechar de guardar el
	// valor mínimo para cada una.
	vector< vector<int> > distancias(combis.size());
	vector<int> minimas_distancias(combis.size());
	for(int i=0;i<combis.size();i++){
		if(i%2000==0) cout << "\t\tCalculando:\t"<< i << endl;
		int min_dist= INT_MAX-1; //INT_MAX se reservará para combinaciones virtualmente eliminadas.
		for(int j=i+1;j<MIN(combis.size(),i+rango_vision);j++){
			int distancia= distancia_correlacion(caso,combis[i],combis[j]);
			distancias[i].push_back(distancia);
			if(distancia<min_dist) min_dist=distancia;
		}
		minimas_distancias[i]= min_dist;
	}
	// Ir eliminando agregados de menor distancia entre todas las que se tienen.
	cout << "\tReduciendo Combinaciones totales (con rango "<< rango_vision << ")..." << endl;
	while(cantidad_combis>cantidad_objetivo){
		if(cantidad_combis%2000==0){
			cout << "\t\tActualmente:\t" << cantidad_combis << endl;
			#ifdef MAPA_COMBIS
				for(int k=0;k<distancias.size();k++){
					cout << (minimas_distancias[k]!=INT_MAX);
				}
				cout << endl;
			#endif
		}
		// Encontrar el mínimo entre las distancias mínimas.
		int min_index=0;
		for(int k=1;k<minimas_distancias.size();k++){
			if(minimas_distancias[k]<minimas_distancias[min_index]) min_index=k;
		}
		// Terminar de reducir si la mínima distancia podría ser de una combinacion eliminada.
		if(minimas_distancias[min_index]>=INT_MAX-1){
			// En teoría no debería pasar, pero para estar seguro.
			cout << "\tReducción máxima alcanzada." << endl;
			break;
		}
		// Encontrar el índice que provoca la mínima distancia.
		int min_sub_index=0;
		int min_sub_dist= INT_MAX;
		for(int k=0;k<distancias[min_index].size();k++){
			if(distancias[min_index][k]<min_sub_dist){
				min_sub_dist= distancias[min_index][k];
				min_sub_index= k;
			}
		}
		// Indice de la combinación que provoca la mínima distancia.
		int objetivo= min_index+(min_sub_index+1);
		{//v Eliminar la Combinación de menor distancia
			// Agregar una nueva distancia para todas las Combinaciones que están
			// al alcance de la objetivo.
			int reversa_adicional=0;
			for(int k=objetivo-1;k>=0 && k>=(objetivo-rango_vision-reversa_adicional);k--){
				if(minimas_distancias[k]<INT_MAX){
					int p= k+distancias[k].size()+1;
					while(minimas_distancias[p]==INT_MAX && p<combis.size()){
						distancias[k].push_back(INT_MAX);
						p++;
					}
					if(p<combis.size()){
						int distancia= distancia_correlacion(caso,combis[k],combis[p]);
						//cout << "Aqui3ka?" << k << endl;
						//cout << k << "\t" << distancias.size() << "\t" << minimas_distancias[k] << "\t" << distancias[k].size() << "\t"<< distancias[k].capacity() << endl;
						//for(int ro=0;ro<distancias[k].size();ro++) cout << distancias[k][ro] << " ";
						//cout << endl;
						distancias[k].push_back(distancia);
						// Actualizar minimas_distancias[k].
						//cout << "Aqui3kb?" << k << endl;
						if(distancia<minimas_distancias[k]) minimas_distancias[k]= distancia;
					}
				}else reversa_adicional++;
			}
			// Eliminar virtualmente Combinacion que provoca mínima distancia (colocando distancias "infinitas").
			cantidad_combis--;
			for(int k=objetivo-1;k>=0 && k>=(objetivo-rango_vision-reversa_adicional);k--){
				if(minimas_distancias[k]<INT_MAX){
					int index_para_distancia= objetivo-(k+1);
					if(index_para_distancia>=distancias[k].size()){
						// Comportamiento inadecuado:
						cout << k << "\t" << objetivo << "\t" << index_para_distancia << "\t" << distancias[k].size() << "\t" << minimas_distancias[k] << endl;
						cout << "wow" << endl;
						throw;
					}
					int distancia_borrada= distancias[k][index_para_distancia];
					distancias[k][index_para_distancia]= INT_MAX;
					// Actualizar minimas_distancias[k] si la eliminada corresponde
					// a minimas_distancias[k].
					if(minimas_distancias[k]==distancia_borrada){
						if(minimas_distancias[k]>distancia_borrada){
							// Comportamiento inadecuado:
							for(auto o=distancias[k].begin();o!=distancias[k].end();o++) cout << *o << " ";
							throw;
						}
						int min_dist= INT_MAX-1;
						for(auto iter= distancias[k].begin();iter!=distancias[k].end();iter++){
							if(*iter<min_dist) min_dist= *iter;
						}
						minimas_distancias[k]=min_dist;
					}
				}
			}
			minimas_distancias[objetivo]= INT_MAX;
			distancias[objetivo].clear();
		}
	}
	// Borrar todas las Combinaciones con distancia mínima infinita.
	cout << "\tEliminando combinaciones..." << endl;
	for(int k=distancias.size()-1;k>=0;k--){
		if(k%3000==0) cout << "\t\tPor comprobar:\t" << k+1 << endl;
		if(minimas_distancias[k]==INT_MAX){
			combis.erase(combis.begin()+k);
		}
	}
}

vector<Combinacion> filtrar_mejores_resultados(Caso &caso, vector< vector<Combinacion>> &generaciones){
	vector<Combinacion> resultado;
	for(int t=0;t<generaciones.size();t++){
		for(int k=0;k<generaciones[t].size();k++){
			resultado.push_back(generaciones[t][k]);
			//generaciones[t][k].imprimir_resumido(cout);
		}
	}
	ordenar_por_ganancia(caso,resultado);
	return resultado;
}
vector<Combinacion> obtener_mejores_combinaciones(Caso &caso, int tamanio_pool, int cantidad_mejores, int obsesividad, int rango_vision, const char *carpeta_intermedios){
	// Obtener mejores configuraciones:
	vector<Configuracion> configs;
	cout << "> Calculando mejores combinaciones individuales para los "<< caso.get_cantidad_predios() <<" predios." << endl;
	for(int k=0;k<caso.get_cantidad_predios();k++){
		if(k%100==0 && k!=0) cout << "\tCalculadas:\t"<< k << endl;
		Configuracion confi= mejor_configuracion(caso,k);
		Combinacion combi(confi);
		if(calcular_ganancia_aprovechando_precalc(caso,Combinacion(confi))>0) configs.push_back(confi);
	}
	// Crear combinaciones a partir de cada configuración individual para la generación 0.
	vector< vector<Combinacion> > combis;
	combis.push_back(vector<Combinacion>());
	for(int k=0;k<configs.size();k++){
		combis[0].push_back(Combinacion(configs[k]));
	}
	// Eliminar similares del tamaño 1
	if(combis[0].size()>tamanio_pool){
		cout << "> Eliminando similares de entre " << combis[0].size() << "..." << endl;
		eliminar_similares(caso, combis[0], tamanio_pool, rango_vision); //Disminuír visión con el tamaño.
	}
	// Expandir generaciones.
	int tamanio=1;
	while(true){
		//Extraer a carpeta de intermedios
		if(carpeta_intermedios!=NULL){
			cout << "> Escribiendo archivos de intermedios." << endl;
			vector<Combinacion> resultado= filtrar_mejores_resultados(caso,combis);
			for(int k=0;k<MIN(resultado.size(),cantidad_mejores);k++){
				ostringstream nombre_archivo;
				nombre_archivo << carpeta_intermedios << "/paso_"<< tamanio <<"_mejor_" << (k+1) << "_predios.csv";
				escribir_combinacion_en_archivo(caso, resultado[k], nombre_archivo.str().c_str());
				vector<Calculo> calculos;
				Economia::calcular_ganancia(caso,resultado[k],&calculos);
				ostringstream nombre_archivo_econom;
				nombre_archivo_econom << carpeta_intermedios << "/paso_"<< tamanio <<"_mejor_" << (k+1) << "_econom.csv";
				escribir_calculos(caso, calculos, nombre_archivo_econom.str().c_str());
			}
		}
		if(!(combis[tamanio-1].size()>0 && (TAMANIO_MAXIMO_COMBINACION<0 || tamanio<TAMANIO_MAXIMO_COMBINACION))) break;
		//Comenzar a crear generación siguiente.
		cout << "> Creando generación tamaño " << (tamanio+1) << " a partir de "<< combis[tamanio-1].size() <<"..." << endl;
		combis.push_back(vector<Combinacion>());
		// Expandir individualmente cada generación.
		int hashes_exitosos=0, hashes_fallidos=0;
		set<int> hashes;
		for(int k=0;k<combis[tamanio-1].size();k++){
			if(k%100==0 && k!=0) cout << "\tActualmente " << k << " expandidos." << endl;
			float ganancia_actual= calcular_ganancia_y_guardar_precalc(caso,combis[tamanio-1][k]);
			if(LIMITE_DE_INVERSION_EN_EXPANSION<0 || ganancia_actual<LIMITE_DE_INVERSION_EN_EXPANSION){
				for(int g=0;g<configs.size();g++){
					int hash_nueva_combi= combis[tamanio-1][k].get_hash(configs[g].id_predio_base);
					//^ Hash que tendría la Combinacion candidata.
					bool presente=false;
					if(hashes.count(hash_nueva_combi)>0){ // Si el hash está presente, es posible que exista.
						Combinacion combi= combis[tamanio-1][k];
						combi.agregar_configuracion(caso,configs[g],obsesividad);
						for(int p=0;p<combis[tamanio].size();p++){
							if(combi==combis[tamanio][p]){
								presente= true;
								break;
							}
						}
						if(presente) hashes_exitosos++;
						if(!presente) hashes_fallidos++;
					}
					if(!presente){
						Combinacion combi= combis[tamanio-1][k];
						combi.agregar_configuracion(caso,configs[g],obsesividad);
						// Agregar si no está presente ya, y sólo si supera las ganancias de su padre.
						if(calcular_ganancia_y_guardar_precalc(caso,combi)>ganancia_actual){
							combis[tamanio].push_back(combi);
							hashes.insert(hash_nueva_combi);
						}
					}
				}
			}
		}
		cout << "\tHashings:\tmalos: "<< hashes_fallidos << "\tbuens: " << hashes_exitosos << endl;
		// Eliminar similares.
		if(combis[tamanio].size()>tamanio_pool){
			cout << "> Eliminando similares de entre " << combis[tamanio].size() << "..." << endl;
			eliminar_similares(caso, combis[tamanio], tamanio_pool, rango_vision); //Disminuír visión con el tamaño.
		}
		cout << "> Generación tamaño " << tamanio+1 << " completa con "<< combis[tamanio].size() << " combinaciones." << endl;
		tamanio++;
	}
	// Juntar todos los resultados en un vector.
	cout << "> Preparando resultados finales..." << endl;
	vector<Combinacion> resultado= filtrar_mejores_resultados(caso,combis);
	if(resultado.size()>cantidad_mejores){
		resultado.erase(resultado.begin()+cantidad_mejores,resultado.end());
	}
	return resultado;
}
