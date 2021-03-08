#include "economia.hpp"

Calculo::Calculo(){
	inversion_total=0;
	van_final=0;
	tam_planta=0;
	for(int k=0;k<ANIOS_PLANTILLA;k++) flujo_de_caja_del_periodo[k]=0;
	id_predio_base=-1;
}

ostream &Calculo::imprimir(ostream &os,const Caso &caso, bool header){
	if(header){
		os << "ORIG_INDX,ORIG_X,ORIG_Y,INVERSION,POTENCIA,VAN,TIR,";
		for(int k=0;k<ANIOS_PLANTILLA;k++){
			os << "FLUJO_" << (k+1);
			if(k<ANIOS_PLANTILLA-1){
				os<<",";
			}
		}
		os << endl;
	}
	if(id_predio_base==-1){
		os << "todos,-,-,";
	}else{
		Predio pred= caso.get_predio(id_predio_base);
		os << id_predio_base+1 << "," << pred.pos_x << "," << pred.pos_y << ",";
	}
	os << (long long int)inversion_total << ",";
	os << (long long int)tam_planta << ",";
	os << (long long int)van_final << ",";
	os << (float)computarIRR(inversion_total,flujo_de_caja_del_periodo,ANIOS_PLANTILLA) << ","; //TODO: reemplazar por TIR.
	for(int k=0;k<ANIOS_PLANTILLA;k++){
		os << (long long int)flujo_de_caja_del_periodo[k];
		if(k<ANIOS_PLANTILLA-1){
			os<<",";
		}
	}
	os << endl;
	return os;
}
void Calculo::aniadir(const Calculo &calc){
	inversion_total+= calc.inversion_total;
	van_final+= calc.van_final;
	tam_planta+= calc.tam_planta;
	for(int k=0;k<ANIOS_PLANTILLA;k++){
		flujo_de_caja_del_periodo[k]+= calc.flujo_de_caja_del_periodo[k];
	}
	id_predio_base=-1;
}


float Economia::calcular_ganancia(const Caso &caso,const Combinacion &combi, vector<Calculo> *calculos, int abort_on_negative_conf){
	vector<Calculo> calcs(combi.configs.size(),Calculo());

	float costo_transporte_por_vaca_por_metro = 365.0*(TONELADAS_POR_VACA/1000.0*caso.get_costo_transporte()*(1.0+COEF_RUGOCIDAD));

	int k=0;
	for(auto conf:combi.configs){
		Calculo &calc= calcs[k];
		calc.id_predio_base= conf.id_predio_base;

		int vacas=0;
		float metrosvaca=0;
		for(auto id_otro_predio:conf.id_predios){
			Predio otro_predio= caso.get_predio(id_otro_predio);
			vacas+= otro_predio.vacas;
			metrosvaca+= otro_predio.vacas*caso.get_distancia(conf.id_predio_base,id_otro_predio);
		}

		float costo_transporte= costo_transporte_por_vaca_por_metro*metrosvaca;
		float potencia_corregida= calcular_eficiencia(POTENCIA_ELECTRICA_POR_VACA*vacas)*POTENCIA_POR_VACA*vacas;
		calc.tam_planta= potencia_corregida;
		float produccion_anual= potencia_corregida*COEF_DIAS_EFECTIVOS*8760.0;
		float produccion_demandada= produccion_anual*COEF_ENERGIA_DEMANDADA;
		float energia_venta= produccion_anual-produccion_demandada;
		float ingresos_electricidad= energia_venta*caso.get_precio_megawatthora()/1000.0;
		calc.inversion_total= calcular_inversion(potencia_corregida);
		float valor_terreno= VALOR_TERRENO;
		float imprevistos= calc.inversion_total*COEF_IMPREVISTOS;
		float van[ANIOS_PLANTILLA];
		float acumulado_siguiente=0;

		float energia_electrica= ingresos_electricidad;

		float potencia_electrica= 0;
		float energia_termica= 0;
		float bonos_de_carbono= 0;
		float costo_de_venta_bonos_de_carbono= COSTO_VENTA_BONOS_DE_CARBONO*bonos_de_carbono;
		float costo_de_transmision_electrica= energia_electrica*COEF_COSTO_TRANSMISION;
		float iva_credito_1= IMPUESTO_IVA*(costo_de_venta_bonos_de_carbono+costo_de_transmision_electrica);
		float mantencion_y_reparacion_de_equipos= COSTO_MANTENCION_Y_REPARACION_EQUIPOS;
		float sueldos_y_leyes_sociales= SUELDOS_Y_LEYES_SOCIALES;
		float otros_gastos= OTROS_GASTOS;
		float iva_credito_2=(mantencion_y_reparacion_de_equipos+sueldos_y_leyes_sociales)*IMPUESTO_IVA;
		float inversion=0;
		float impuesto_a_la_renta=0;

		float flujo_de_caja= 0;
		float flujo_de_caja_acumulado= 0;
		float ventas_netas= energia_electrica+potencia_electrica+energia_termica+bonos_de_carbono;
		float iva_debito_1= IMPUESTO_IVA*ventas_netas;
		float ventas_brutas= ventas_netas+iva_debito_1;
		float costo_de_venta_neto= costo_de_transmision_electrica+costo_de_venta_bonos_de_carbono;
		float costo_de_venta_bruto= costo_de_venta_neto+iva_credito_1;
		float margen_de_explotacion_bruto= ventas_brutas-costo_de_venta_bruto;
		float gastos_de_explotacion_neto= mantencion_y_reparacion_de_equipos+sueldos_y_leyes_sociales+costo_transporte+otros_gastos;
		float gastos_de_explotacion_bruto= gastos_de_explotacion_neto+iva_credito_2;
		float total_iva_debito= iva_debito_1;
		float flujo_neto_de_explotacion= ventas_netas-costo_de_venta_neto-gastos_de_explotacion_neto;
		float total_iva_credito= iva_credito_1+iva_credito_2;

		float iva_neto_del_periodo= total_iva_debito-total_iva_credito;
		float pago_de_iva= iva_neto_del_periodo;
		float utilidad_bruta;
		float depreciacion;
		float impuesto_acumulado;
		float impuesto_a_pagar;
		float capital_de_trabajo;
		float utilidad_neta;


		for(int anio=1;anio<=ANIOS_PLANTILLA;anio++){
			if(anio<=ANIOS_DEPRECIACION){
				depreciacion= (calc.inversion_total-imprevistos-valor_terreno)/ANIOS_DEPRECIACION;
			}else{
				depreciacion=0;
			}
			if(anio==1){
				capital_de_trabajo= -calc.inversion_total*COEF_CAPITAL_DE_TRABAJO;
			}else if(anio==ANIOS_PLANTILLA){
				capital_de_trabajo= calc.inversion_total*COEF_CAPITAL_DE_TRABAJO;
			}else{
				capital_de_trabajo= 0;
			}
			utilidad_bruta= flujo_neto_de_explotacion-depreciacion;
			if(anio==1){
				impuesto_acumulado= utilidad_bruta*IMPUESTO_RENTA;
			}else{
				impuesto_acumulado= acumulado_siguiente+utilidad_bruta*IMPUESTO_RENTA;
			}
			if(impuesto_acumulado>0){
				impuesto_a_pagar= impuesto_acumulado;
				acumulado_siguiente=0;
			}else{
				impuesto_a_pagar= 0;
				acumulado_siguiente=impuesto_acumulado;
			}
			utilidad_neta= utilidad_bruta-impuesto_a_pagar;
			calc.flujo_de_caja_del_periodo[anio-1]= capital_de_trabajo+flujo_neto_de_explotacion-impuesto_a_pagar;
			if(anio==1){
				van[anio-1]= calc.flujo_de_caja_del_periodo[anio-1]/(pow(1.0+COEF_VAN,anio));
			}else{
				van[anio-1]= calc.flujo_de_caja_del_periodo[anio-1]/(pow(1.0+COEF_VAN,anio));
			}
		}
		//Se suman los vans:
		calc.van_final= -calc.inversion_total;
		for(int anio=1;anio<=ANIOS_PLANTILLA;anio++){
			calc.van_final+= van[anio-1];
		}

		k++;
	}
	//Paso final para retornar el valor de ganancia.
	float ganancia=0;
	for(Calculo calc:calcs){
		if(calc.van_final<0 && abort_on_negative_conf){ //No vale la pena agregar una combinación con una ganancia de VAN negativo.
			if(calculos!=NULL) *calculos=calcs;
			return 0;
		}
		ganancia+=calc.van_final;
	}
	if(calculos!=NULL) *calculos=calcs;
	return ganancia;
}
