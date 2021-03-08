#ifndef ECONOMIA_H
#define ECONOMIA_H

#include "vespeno.hpp"
#include "lib/irr/irr.h"
#include <math.h>
#include <vector>

using namespace std;

//@@@@@@@@@@@@> CONSTANTES BASICAS:

#define PORCENTAJE_DISPONIBILIDAD_BIOMASA 80
	// Porcentaje de disponibilidad de biomasa. NOTA: Sólo debería usarse en main.c
#define C_TRANSPORTE_POR_TONKM 0.3
	// Ida y vuelta, en dólares. NOTA: Sólo debería usarse en main.c
#define PRECIO_MEGAWATTHORA 100.0
	// Valor de la energía. NOTA: Sólo debería usarse en main.c



#define KG_ESTIERCOL_POR_VACA 10.0
	// Se calcula como 10% del peso de la vaca (600 Kg) dividido en 24, por 4.
#define COEF_RUGOCIDAD 0.41
	// Se agrega a la distancia euclidiana.
#define COEF_SOLIDOS_TOTALES 0.245
	// Que se pueden extraer del estiercol.
#define COEF_CENIZAS 0.20
	// Que se pueden extraer de los sólidos totales, lo que no, son sólidos volátiles.
#define COEF_CH4 0.55
#define COEF_CO2 0.45
	// Coeficientes de gases en biogas.
#define PM_CH4 16.0
#define PM_CO2 44.0
	// Pesos moleculares.
#define PODER_CALORICO_CH4 9.96
	// Poder calorico de metano Kwh/m3
#define COEF_DIAS_EFECTIVOS 0.9
	// Coeficiente de días que se genera energía.
#define COEF_ENERGIA_DEMANDADA 0.15
	// Energía que se demanda.
#define ANIOS_PLANTILLA 20
	// Años para la plantilla.
#define VALOR_TERRENO 40667.0
	// Valor del terreno en dólares.
#define COEF_IMPREVISTOS 0.15
	// Coeficiente por sobre la inversión inicial para asignación de imprevistos
#define COSTO_VENTA_BONOS_DE_CARBONO 0.4
	// Coef, costo de venta de bonos de carbono (revisar como se calcula ganancia por bono de carbono)
#define COEF_COSTO_TRANSMISION 0.05
	// Coeficiente de coste de transmisión por sobre las ganancias por venta de energía eléctrica.
#define IMPUESTO_IVA 0.19
	// Impuesto al valor agregado.
#define COSTO_MANTENCION_Y_REPARACION_EQUIPOS 60000.0
#define SUELDOS_Y_LEYES_SOCIALES 20000.0
#define OTROS_GASTOS 5000.0
	// en dólares, por planta.
#define ANIOS_DEPRECIACION 3.0
	// años para depreciacion (aunque se expresa como float debe ser entero).
#define COEF_CAPITAL_DE_TRABAJO 0.2
	// capital de trabajo con respecto a la inversion.
#define IMPUESTO_RENTA 0.2
	// coeficiente correspondiente al impuesto a la renta.
#define COEF_VAN 0.1
	// tipo de interés para cálculo del van.

//@@@@@@@@@@@@> CONSTANTES DERIVABLES:

#define TONELADAS_POR_VACA (KG_ESTIERCOL_POR_VACA/1000.0)

#define H2O_POR_VACA (KG_ESTIERCOL_POR_VACA*(1.0-COEF_SOLIDOS_TOTALES))
#define CENIZAS_POR_VACA (KG_ESTIERCOL_POR_VACA*COEF_SOLIDOS_TOTALES*COEF_CENIZAS)
#define SOLIDOS_VOLATILES_POR_VACA (KG_ESTIERCOL_POR_VACA*COEF_SOLIDOS_TOTALES-CENIZAS_POR_VACA)

#define MASA_MOLAR ((PM_CH4*COEF_CH4)+(PM_CO2*COEF_CO2))
	// En g/Mol, del biogas.
#define DENSIDAD (MASA_MOLAR/22.4)
	// g/l o Kg/m3.
#define PRODUCCION_POR_VACA (SOLIDOS_VOLATILES_POR_VACA*450.0/1000.0)
	// m3 de biogas por Kg de sólidos volátiles.
#define MASA_POR_VACA (PRODUCCION_POR_VACA*DENSIDAD)
	// Kg de biogas diario.
#define SVB_POR_VACA (SOLIDOS_VOLATILES_POR_VACA-MASA_POR_VACA)
	// Kg solidos volatiles en digestato.
#define DIGESTATO_POR_VACA (H2O_POR_VACA+SVB_POR_VACA+CENIZAS_POR_VACA)
	// en Kg
#define PORCENTAJE_SVB  (100.0*SVB_POR_VACA/DIGESTATO_POR_VACA)
	// Porcentaje de sólidos volátiles en digestato.
#define ENERGIA_POR_VACA (PRODUCCION_POR_VACA*COEF_CH4*PODER_CALORICO_CH4)
	// Energia disponible por dia de acuerdo a poder calorico de metano.
#define POTENCIA_POR_VACA (ENERGIA_POR_VACA/24.0)
	// Kwh/h
#define POTENCIA_ELECTRICA_POR_VACA (POTENCIA_POR_VACA*0.4)
	// coeficiente es fracción de energía que es eléctrica para precálculo de la eficiencia.
#define POTENCIA_TERMICA_POR_VACA (POTENCIA_POR_VACA*0.45)
	// coeficiente es fracción de energía que es térmica para precálculo de la eficiencia.

//@@@@@@@@@@@@> EFICIENCIAS:

#define POTENCIA_INVERSION_MINIMA 500.0
	// Potencia de la planta mínima.
#define INVERSION_MINIMA 1050166.0
	// Inversion para costear la planta mínima.
#define CRECIMIENTO_INVERSION_POR_POTENCIA 2100.332
	// Costo de inversion agregada por cada KWatt

#define COEF_POSICION_EFICIENCIA 0.3069029834
	// Eficiencia mínima, con potencia tendiendo a 0.
#define FACT_MULTIPLICADOR_LOGARITMO_NATURAL_EFICIENCIA 0.0184870004
	// Factor que multiplica el logaritmo natural en el cálculo de la eficiencia.

//@@@@@@@@@@@@> CONSTANTES DEL ALGORITMO:

#define TAMANIO_MAXIMO_COMBINACION 20
	// Cantidad máxima de plantas hasta las que se calculará.
	// Colocar -1 si no existe límite.

#define LIMITE_DE_INVERSION_EN_EXPANSION 100000000
	// Límite que, si la inversion requerida de una combinación lo alcanza, ya no se prueba a agregar más plantas a esta.
	// Colocar -1 si no existe límite.

#define CONTROLAR_PREDIOS_APORTADORES_OBSESIVAMENTE 1
	// Al comprobar si pasar cada predio de un lado a otro en las colisiones, comprobar también si, por ejemplo, por efectos
	// de la diferencia de eficiencia, soltar y recuperar predios fronterizos que pudieran pasar a aportar negativamente.
	// 0= No probar en nungún momento.
	// 1= Sólo probar eliminar predios frontera que aportan negativamente al final.
	// 2= Probar en cada paso y recuperar en cada paso.

//@@@@@@@@@@@@> FUNCIONES

inline float calcular_inversion(float potencia){
	if(potencia<POTENCIA_INVERSION_MINIMA){
		return INVERSION_MINIMA;
	}else{
		return INVERSION_MINIMA+(potencia-POTENCIA_INVERSION_MINIMA)*CRECIMIENTO_INVERSION_POR_POTENCIA;
	}
}

inline float calcular_eficiencia(float potencia){
	float res= FACT_MULTIPLICADOR_LOGARITMO_NATURAL_EFICIENCIA*log(potencia)+COEF_POSICION_EFICIENCIA;
	return MAX(0,res);
}

//################## HEADER #################>

struct Calculo{
	float inversion_total;
	float van_final;
	float tam_planta;
	float flujo_de_caja_del_periodo[ANIOS_PLANTILLA];
	int id_predio_base;
	Calculo();
	ostream &imprimir(ostream &os,const Caso &caso, bool header);
	void aniadir(const Calculo &calc);
};

namespace Economia{
	float calcular_ganancia(const Caso &caso,const Combinacion &combi, vector<Calculo> *calculos=NULL, int abort_on_negative_conf=VALUE_0_ON_NEGATIVE_CONF);
}

#endif
