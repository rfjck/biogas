# Vespeno

This file contains the source code of a solver used for evaluating the feasibility of installing biogas plant networks in southern Chile.

# Solver instructions

The solver is used to find a good set of biogas facilitites from an input (base) file with farms.

## 1. Set input parameters

Modify the `economia.hpp` file to change the value of the parameters for different scenarios.

The 3 main parameters are:

* The percentage of available biomass:
    ```cpp
    #define PORCENTAJE_DISPONIBILIDAD_BIOMASA 80
    ```

* The transport cost (USD/tkm):
    ```cpp
    #define C_TRANSPORTE_POR_TONKM 0.3
    ```

* Price of the megawatt-hour (USD/MWh)
    ```cpp
    #define PRECIO_MEGAWATTHORA 100.0
    ```

## 2. Clear output

```
$ make clear
```

## 3. Compile solver

```
$ make compil
```

## 4. Run solver

The solver can be run using the following command:

```
$ ./build/vespeno input/bases.csv build/out build/econom
```

The first argument is the input file.
`input/bases.csv` contains the names, x, y, and number of cows of each farm in southern Chile.

The second and third arguments are the output folders. `build/out` will store the best solutions found and `build/econom` will output the NPV tables of these solutions.

# Evaluator instructions

The evaluator is used to evaluate the NVP of the ArcGIS results (assignments).

**NOTE**: The evaluator reads all the parameters form `economia.hpp` except the 3 main parameters which are manually introduced.

## 1. Compile evaluator

The evaluator is compiled with the same Makefile target.

```
$ make compil
```

## 2. Run the evaluator and insert parameters manually

The evaluator recives two command line arguments: the output folders, for instance: `build/out` and `build/econom`.

After that, parameters should be inserted manually.

The following example loads the input (base) file `input/bases.csv`, sets the biomass to `80` percent, the energy price to `100` USD/MWh, the transport cost to `0.3` (USD/tkm).

Then, it loads `input/yave25000.txt` which corresponds to the results obtained by the ArcGIS method with a 25km radious. And uses mode 2 to reevaluate all asignations to get a better NVP.

```
$ ./build/evaluador build/out build/econom

Inserte archivo con bases originales (ejemplo "input/bases.csv") :
input/bases.csv
Inserte % biomasa (ejemplo "100.0") :
80
Inserte precio MWh (ejemplo: "86.0") :
100
Inserte costo transporte tonKm (ejemplo: "0.3") :
0.3
Leyendo "input/bases.csv"...
Inserte arhivo del arcGis (ejemplo "input/yave10000.txt") :
input/yave25000.txt
Leyendo "input/yave25000.txt"...
Modos:
    0 = No cambiar asignaciones.
    1 = Solo eliminar aportadores negativos.
    2 = Reevaluar asignaciones.
Inserte modo:
2
Agregando configuración 1/11...
Agregando configuración 2/11...
Agregando configuración 3/11...
Agregando configuración 4/11...
Agregando configuración 5/11...
Agregando configuración 6/11...
Agregando configuración 7/11...
Agregando configuración 8/11...
Agregando configuración 9/11...
Agregando configuración 10/11...
Calculando VAN...
ganancia :	1.75169e+06
Escribiendo build/out/yave25000_0.8_100_0.3_2_predios.csv
Escribiendo build/econom/yave25000_0.8_100_0.3_2_econom.csv
Hecho!
```


