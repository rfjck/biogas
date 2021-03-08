#define LOW_RATE 0.00001
#define HIGH_RATE 0.5
#define MAX_ITERATION 1000
#define PRECISION_REQ 0.0000001

#include "irr.h"

double computarIRR(float inversion_inicial, float flujos_caja[], int anios_flujo){
	double *flujos;
	flujos= new double[anios_flujo+1];
	flujos[0]= -inversion_inicial;
	for(int anio=1;anio<=anios_flujo;anio++){
		flujos[anio]=flujos_caja[anio-1];
	}
	double devolver= computeIRR(flujos,anios_flujo+1);
	delete[] flujos;
	return devolver;
}

double computeIRR(double cf[], int numOfFlows)
{
	int i = 0,j = 0;
	double m = 0.0;
	double old = 0.00;
	double newo = 0.00;
	double oldguessRate = LOW_RATE;
	double newguessRate = LOW_RATE;
	double guessRate = LOW_RATE;
	double lowGuessRate = LOW_RATE;
	double highGuessRate = HIGH_RATE;
	double npv = 0.0;
	double denom = 0.0;
	for(i=0; i<MAX_ITERATION; i++)
	{
		npv = 0.00;
		for(j=0; j<numOfFlows; j++)
		{
			denom = pow((1 + guessRate),j);
			npv = npv + (cf[j]/denom);
		}
			/* Stop checking once the required precision is achieved */
		if((npv > 0) && (npv < PRECISION_REQ))
			break;
		if(old == 0)
			old = npv;
		else
			old = newo;
		newo = npv;
		if(i > 0)
		{
			if(old < newo)
			{
				if(old < 0 && newo < 0)
					highGuessRate = newguessRate;
				else
					lowGuessRate = newguessRate;
			}
			else
			{
				if(old > 0 && newo > 0)
					lowGuessRate = newguessRate;
				else
					highGuessRate = newguessRate;
			}
		}
		oldguessRate = guessRate;
		guessRate = (lowGuessRate + highGuessRate) / 2;
		newguessRate = guessRate;
	}
	return guessRate;
}