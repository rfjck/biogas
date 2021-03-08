#ifndef OUTPUT_H
#define OUTPUT_H

#include <fstream>
#include <string>
#include <vector>
#include "vespeno.hpp"
#include "economia.hpp"

void escribir_combinacion_en_archivo(const Caso &caso, Combinacion &combi, const char *nombre_archivo);
void escribir_calculos(const Caso &caso, vector<Calculo> &calculos, const char *nombre_archivo);

#endif
