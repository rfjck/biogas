#ifndef INPUT_H
#define INPUT_H

#include <cmath>
#include <fstream>
#include "vespeno.hpp"

void leer_predios_scv(Caso &caso, char *nombre_archivo);
/* Obtiene el vector (lista) de todos los predios que se utilizarán en el
programa desde el archivo.
*/

void obtener_distancias(Caso &caso, char *nombre_archivo);
/* Obtiene la matriz de distancias entre todos los predios.
*/

#endif
