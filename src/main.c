#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <mpi.h>
#include <getopt.h>

#define master 0

// Funciones a utilizar
void mostraTablero(int **tablero, int x, int y);

int main(int argc, char  *argv[]) {

  FILE *fichero;
  int iteraciones, leido;
  int tamanoX, tamanoY;
  char c;
  char *ruta;

  // Inicia MPI

  int rank, nprocs;
  MPI_Request request;
  MPI_Status status;

  MPI_Init (&argc, &argv);
  MPI_Comm_rank (MPI_COMM_WORLD, &rank);
  MPI_Comm_size (MPI_COMM_WORLD, &nprocs);

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

    // Variable local solo para el proceso master

    int  **unTablero = (int **)malloc(tamanoX * sizeof(int*));
  	int  **tableroCopia = (int **)malloc(tamanoX * sizeof(int*));

    for (int i = 0; i < tamanoX; i++)
    {
      unTablero[i] = (int*)malloc(tamanoY * sizeof(int));
      tableroCopia[i] = (int*)malloc(tamanoY * sizeof(int));
    }

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
  }
  MPI_Barrier(MPI_COMM_WORLD); // Detiene los procesos que estan bajo el mismo comunicador
  MPI_Bcast(&tamanoX, 1,MPI_INT,master,MPI_COMM_WORLD); /* Envia las variables a todos los procesos */
  MPI_Bcast(&tamanoY, 1,MPI_INT,master,MPI_COMM_WORLD);
  printf("Proceso %d var X=%d Y=%d\n", rank, tamanoX, tamanoY);
  MPI_Finalize();

  return 0;
}

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
