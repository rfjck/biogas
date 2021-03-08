TARGETS = economia.cpp input.cpp optimizacion.cpp output.cpp vespeno.cpp

clear:
	# Clears all output
	rm -rf build 2> /dev/null || true
	rm resultados.zip 2> /dev/null || true
	rm -rf mmedia 2> /dev/null || true

init:
	# Creates all output folder
	mkdir -p build
	mkdir -p build/out
	mkdir -p build/econom
	mkdir -p build/interm
	mkdir -p build/eval
	mkdir -p mmedia

compil: init
	# Compile libhungarian
	cd lib/libhungarian; make
	# Compile the solver
	g++ $(TARGETS) main.cpp lib/irr/irr.cpp -std=c++11 -lm -L lib/libhungarian -lhungarian -o build/vespeno -D VALUE_0_ON_NEGATIVE_CONF=1 #-D MAPA_COMBIS
	# Compile the evaluator
	g++ $(TARGETS) main_evaluador.cpp lib/irr/irr.cpp -std=c++11 -lm -L lib/libhungarian -lhungarian -o build/evaluador -D VALUE_0_ON_NEGATIVE_CONF=0

test: clear compil
	# Cluster specific: run test on cluster
	screen -S vespeno bash -c './build/vespeno build/out build/econom'

run: compil
	# Executes the solver and outputs facilitites to build/out and build/econom
	./build/vespeno input/bases.csv build/out build/econom

evaluar: compil
	# Executes the evaluator
	./build/evaluador build/eval build/eval

fotos:
	rm -rf mmedia 2> /dev/null || true
	mkdir mmedia
	python extra/drawer.py "input/bases.csv" || true
join:
	python extra/joiner.py
compress:
	rm resultados.zip 2> /dev/null || true
	zip resultados.zip build/out/* build/econom/*
mail: fotos join compress
	cat economia.hpp main.cpp | grep '#define' | mutt -s "Resultados Vespeno" -a resultados.zip $(shell find mmedia -type f -printf '%T@ %p\n' | sort -n | tail -1 | cut -f2- -d" ") build/final.csv -- renato.casas@ulagos.cl franciscojacb@gmail.com
