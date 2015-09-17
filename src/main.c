#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <mpi.h>
#include <getopt.h>

#define master 0

// Funciones a utilizar

int **crearTablero(int x, int y, char *dir);

void mostraTablero(int **tablero, int x, int y);

int main(int argc, char  *argv[]) {

  FILE *fichero;
  int **unTablero;
  int iteraciones, leido, auxIter = 0;
  int cantidadNodos;
  int tamanoX, tamanoY;
  char c;
  char *ruta;
  int contadorRecibidos = 0;

  // Inicia MPI

  int rank, nprocs;
  MPI_Request request;
  MPI_Status status;

  MPI_Init (&argc, &argv);
  MPI_Comm_rank (MPI_COMM_WORLD, &rank);
  MPI_Comm_size (MPI_COMM_WORLD, &nprocs);

  MPI_Barrier(MPI_COMM_WORLD);

  if (rank == master) {
    printf("Bienvenido al juego de la vida....\n");
    printf("El juego sera ejecutado por un total de %d procesos\n", nprocs);
    printf("El proceso master con ID %d comenzara la ejecucion del codigo, presione una tecla para continuar\n", rank);
    getchar();
    while((c = getopt(argc, argv, "f:i:")) != -1){

      switch(c){

        case 'i':
          iteraciones = atoi(optarg);
          break;
        case 'f':
          ruta = malloc(strlen(optarg)*sizeof(char));
          ruta = strcpy(ruta, optarg);
          break;
        default:
          printf("Error en los argumentos\n");
          return 0;
          break;
      }

    }

    if(c == 1){

      return 0;

    }

    printf("Numero de iteraciones: %d\n",  iteraciones);
    printf("leyendo ruta...\n" );
    fichero = fopen(ruta, "r");
    fscanf(fichero, "%d %d", &tamanoX, &tamanoY);
    printf("Tamano x = %d  y = %d\n",  tamanoX, tamanoY);


    // Inicializa el contador inicial de valores

    int cantidadNodosProc[nprocs];
    for (int i = 0; i < nprocs; i++) {
      /* code */
      cantidadNodosProc[i] = 0;
    }
    //memset(cantidadNodosProc, 0, nprocs);

    // Variable local solo para el proceso master
    unTablero = crearTablero(tamanoX, tamanoY, ruta);

    printf("Cargando datos del tablero\n");
    // Carga el tablero desde archivo de texto

    printf("Inicia carga del tablero\n");

    for (int i = 0; i < tamanoX; i++)
    {
      for (int j = 0; j < tamanoY; j++)
      {

          leido = fgetc(fichero) - 48;
          while(leido < 0 || leido > 1){
            leido = fgetc(fichero) - 48;
          }
          unTablero[i][j] = leido;
      }
    }

    // muestra el tablero inicial del juego de la vida.

    mostraTablero(unTablero, tamanoY, tamanoY);
    printf("Tablero Cargado con exito...\n");
    fclose(fichero);


    //printf("Cada proceso analizara las siguientes columnas...\n");
    for (int i = 0; i < tamanoX*tamanoY; i++) {
        //printf("Proceso %d se encarga de la columnas %d\n", ((i) % nprocs),  i) ;
        cantidadNodosProc[(i) % nprocs]++;
    }

    for (int i = 0; i < nprocs; i++) {
       MPI_Isend(&cantidadNodosProc[i],1,MPI_INT,i,0,MPI_COMM_WORLD,&request);
    }

  }
  MPI_Barrier(MPI_COMM_WORLD); // Detiene los procesos que estan bajo el mismo comunicador
  MPI_Bcast(&iteraciones, 1,MPI_INT,master,MPI_COMM_WORLD);
  MPI_Bcast(&tamanoX, 1,MPI_INT,master,MPI_COMM_WORLD); /* Envia las variables a todos los procesos */
  MPI_Bcast(&tamanoY, 1,MPI_INT,master,MPI_COMM_WORLD);
  //printf("Proceso %d var X=%d Y=%d\n", rank, tamanoX, tamanoY);
  MPI_Irecv(&cantidadNodos,1,MPI_INT,master,0,MPI_COMM_WORLD, &request);
  printf("Proceso %d leera %d nodos en %d\n", rank, cantidadNodos, iteraciones);


/****************************************************************************/
/****************************************************************************/
/****************************************************************************/
/****************************************************************************/
/****************************************************************************/

  while(auxIter < iteraciones){ // Iteraciones generales del programa


        /* el proceso master debe construir un vector de tamaño 11 int con los siguientes datos

          coordenadas x e y
          valor en las coordenadas
          8 datos de tipo int con los valores de los vecinos a la coordenada x , y

          El proceso master debe enviar a cada proceso todos los datos para el posterior calculo

        */

        if(rank == 0){ // Proceso master




        }





        while(contadorRecibidos < cantidadNodos){
          contadorRecibidos++;
        }


        MPI_Barrier(MPI_COMM_WORLD);

        /*
          cada proceso envia los nuevos valores al proceso master
          se envia un vector de tamaño 3 con los siguientes datos
          pos 0 = coordenada x
          pos 1 = coordenada y
          pos 2 = value en coordenadas x , y

        */

        for (int i = 0; i < cantidadNodos; i++) {
          /* code */
        }

        MPI_Barrier(MPI_COMM_WORLD);

        if(rank == 0){
            printf("Master: Termino la Iteracion %d\n", auxIter+1 );
            auxIter++;
            mostraTablero(unTablero, tamanoX, tamanoY);

        }
        MPI_Barrier(MPI_COMM_WORLD);
        MPI_Bcast(&auxIter, 1,MPI_INT,master,MPI_COMM_WORLD);
        MPI_Barrier(MPI_COMM_WORLD);
  }

  MPI_Barrier(MPI_COMM_WORLD);
  MPI_Finalize();

  return 0;
}

/************************************************************************/
/************************************************************************/
/************************************************************************/
/************************************************************************/
/************************************************************************/

void mostraTablero(int **tablero, int x, int y){


	for (int i = 0; i < x; i++)
	{

		for (int j = 0; j < y; j++)
		{

			// Esta linea se puede descomentar para mostrar la matriz con los digitos 0 y 1 para representar
			// Las celulas del tablero

			//printf(" %d ", tablero[i][j]);

			// Se puede comentar el codigo entre "Inicio mostrar" y "termino mostrar" para ocultar la forma alternativa
			// de mostrar el tablero de juego

			// Inicio mostrar

			if(tablero[i][j] == 0){
				printf("[   ]");
			}else{
				printf("[ * ]");
			}

			// termino mostrar

		}

		printf("\n");
	}

}

int **crearTablero(int x, int y, char *dir){

  int **matriz = (int**)malloc(sizeof(int*)*x);
  for (int i = 0; i < x; i++) {
    matriz[i] = (int*)malloc(sizeof(int)*y);
  }

  return matriz;

}
