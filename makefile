all:
	mkdir -p build
	touch build/salida
	mpicc src/main.c -o build/salida -std=c99
run:
	mpirun -np 3 build/salida ${ARGS}
clean:
	rm build/salida
