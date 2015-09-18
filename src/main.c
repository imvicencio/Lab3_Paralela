#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <mpi.h>
#include <getopt.h>

#define master 0

// Funciones a utilizar

int **crearTablero(int x, int y);
void mostraTablero(int **tablero, int x, int y);
int **pedirMemoria(int a, int b);
int RevisarVecinos(int **tablero,int i, int j, int x, int y, int *vector);


// Main
int main(int argc, char  *argv[]) {

  FILE *fichero;
  int **unTablero;
  int **nodosEstudio;
  int **nodoMaster;
  int iteraciones, leido, auxIter = 0;
  int cantidadNodos;
  int tamanoX, tamanoY;
  char c;
  char *ruta;
  int contadorRecibidos = 0;

  // Inicia MPI

  int rank, nprocs;

  MPI_Init (&argc, &argv);
  MPI_Comm_rank (MPI_COMM_WORLD, &rank);
  MPI_Comm_size (MPI_COMM_WORLD, &nprocs);

  MPI_Request request[nprocs];
  MPI_Status status[nprocs];

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

    if(nprocs > tamanoX*tamanoY ){
      printf("Error existen mas procesos que elementos a paralelizar...\n");
      MPI_Abort(MPI_COMM_WORLD, 1);
      MPI_Finalize();
    }

    // Inicializa el contador inicial de valores

    int cantidadNodosProc[nprocs];
    for (int i = 0; i < nprocs; i++) {
      /* code */
      cantidadNodosProc[i] = 0;
    }
    //memset(cantidadNodosProc, 0, nprocs);

    // Variable local solo para el proceso master
    unTablero = crearTablero(tamanoX, tamanoY);

    printf("Cargando datos del tablero\n");
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
       MPI_Isend(&cantidadNodosProc[i],1,MPI_INT,i,0,MPI_COMM_WORLD,&request[i]);
    }

  }
  MPI_Barrier(MPI_COMM_WORLD); // Detiene los procesos que estan bajo el mismo comunicador
  MPI_Bcast(&iteraciones, 1,MPI_INT,master,MPI_COMM_WORLD);
  MPI_Bcast(&tamanoX, 1,MPI_INT,master,MPI_COMM_WORLD); /* Envia las variables a todos los procesos */
  MPI_Bcast(&tamanoY, 1,MPI_INT,master,MPI_COMM_WORLD);
  //printf("Proceso %d var X=%d Y=%d\n", rank, tamanoX, tamanoY);
  MPI_Irecv(&cantidadNodos,1,MPI_INT,master,0,MPI_COMM_WORLD, &request[rank]);
  printf("Proceso %d leera %d nodos en %d\n", rank, cantidadNodos, iteraciones);


  // cada proceso tiene un puntero a un vector de int

  nodosEstudio = crearTablero(cantidadNodos,8);

  if(rank == master){
    nodoMaster = crearTablero(tamanoX*tamanoY,3);
  }

MPI_Barrier(MPI_COMM_WORLD);

/****************************************************************************/
/****************************************************************************/
/****************************************************************************/
/****************************************************************************/
/****************************************************************************/

  while(auxIter < iteraciones){ // Iteraciones generales del programa

        MPI_Barrier(MPI_COMM_WORLD);

        /* el proceso master debe construir un vector de tamaño 11 int con los siguientes datos

          coordenadas x e y
          valor en las coordenadas
          8 datos de tipo int con los valores de los vecinos a la coordenada x , y

          El proceso master debe enviar a cada proceso todos los datos para el posterior calculo

        */


        MPI_Barrier(MPI_COMM_WORLD);

        if(rank == master){ // Proceso master

          int numeroElemento = 0;

          for (int i = 0; i < tamanoX; i++) {
            for (int j = 0; j < tamanoY; j++) {
              // Por cada posicion del tablero debe de construir el elemento
              int destino = (numeroElemento) % nprocs;
              int arrayTempo[8];
              arrayTempo[0] = i;
              arrayTempo[1] = j;
              arrayTempo[2] = unTablero[i][j];
              //printf("Valores enviados %d %d %d \n", arrayTempo[0],arrayTempo[1],arrayTempo[2]);

              //busca los vecinos del punto seleccionado


              MPI_Isend(&arrayTempo,3,MPI_INT,destino,destino,MPI_COMM_WORLD,&request[destino]);
              numeroElemento++;
            }
          }
        }

        MPI_Barrier(MPI_COMM_WORLD);
        MPI_Barrier(MPI_COMM_WORLD);

        while(contadorRecibidos < cantidadNodos){
          //printf("Proc: %d entradno al while\n", rank);
          int temporal = 0;
          MPI_Iprobe(master, rank, MPI_COMM_WORLD, &temporal,&status[rank]);
          //printf("rank %d verificando... Iteracion %d \n", rank, contadorRecibidos);
          if(temporal){
            // recibe el mensaje
            MPI_Irecv(nodosEstudio[contadorRecibidos],3,MPI_INT,master,rank,MPI_COMM_WORLD,&request[rank]);
              //printf("LLego dato a %d numero %d value %d %d %d\n", rank , contadorRecibidos+1, nodosEstudio[contadorRecibidos][0],nodosEstudio[contadorRecibidos][1],nodosEstudio[contadorRecibidos][2]);
            temporal = 1;
            contadorRecibidos++;
          }
        }

        MPI_Barrier(MPI_COMM_WORLD);
        MPI_Barrier(MPI_COMM_WORLD);

        //El proceso muestra el elemento recibido
        /*if(rank == 2){
          for (int i = 0; i < cantidadNodos; i++) {
              printf("Value %d", );
          }

        }*/

        MPI_Barrier(MPI_COMM_WORLD);
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
        MPI_Barrier(MPI_COMM_WORLD);


        if(rank == 0){
            //system("clear");
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

int **crearTablero(int x, int y){

  int **matriz = (int**)malloc(sizeof(int*)*x);
  for (int i = 0; i < x; i++) {
    matriz[i] = (int*)malloc(sizeof(int)*y);
  }

  return matriz;

}

int RevisarVecinos(int **tablero,int i, int j, int x, int y, int *vector){

	int contador = 0;

	/*

		Calcular todos lo vecinos de una celula que esten en los rangos permitidos de la matriz. En este punto
		Se considera la matriz de forma finita.

		Los puntos extremos o de conflicto, quedan semi explorados en este punto.

	*/
	for (int m = i-1; m <= i+1; m++)
	{
		for (int n = j-1; n <= j+1 ; n++)
		{

			if(!(m == i && n == j) && m >= 0 && n >= 0 && m < x && j < y){

				contador = contador + tablero[m][n];

			}

		}
	}

	/*

		A continuacion se procede a buscar los vecinos de las celulas que estan en los limites del
		tablero. Se detallan los 8 casos extremos y los puntos del tablero que requieren para completar la exploracion
		de vecinos vivos. Se realiza de esta forma para su posterior ejecucion en BSP.

	*/


	// Caso 1 superior izquierdo

	if(i == 0 && j == 0){

		// Pide los valores que estan fuera del rango
		contador = contador + tablero[x-1][1];
		contador = contador + tablero[x-1][0];
		contador = contador + tablero[x-1][y-1];
		contador = contador + tablero[0][y-1];
		contador = contador + tablero[1][y-1];


	}else{

		// Caso 2 superior derecha
		if (i == 0 && j == y-1)
		{
			// Pide los valores que estan fuera del rango
			contador = contador + tablero[x-1][y-2];
			contador = contador + tablero[x-1][y-1];
			contador = contador + tablero[x-1][0];
			contador = contador + tablero[0][0];
			contador = contador + tablero[1][0];

		}else{

			// Caso 3 inferior izquierdo
			if (i == x-1 && j == 0)
			{
				// Pide los valores que estan fuera del rango
				contador = contador + tablero[x-2][y-1];
				contador = contador + tablero[x-1][y-1];
				contador = contador + tablero[0][y-1];
				contador = contador + tablero[0][0];
				contador = contador + tablero[0][1];

			}else{

				// Caso 4 inferior derecha
				if (i == x-1 && j == y-1)
				{
					// Pide los valores que estan fuera del rango
					contador = contador + tablero[x-2][0];
					contador = contador + tablero[x-1][0];
					contador = contador + tablero[0][0];
					contador = contador + tablero[0][y-1];
					contador = contador + tablero[0][y-2];

				}else{

					// Caso 5 Lado superior
					if (i == 0)
					{
						// Pide los valores que estan fuera del rango
						contador = contador + tablero[x-1][j-1];
						contador = contador + tablero[x-1][j];
						contador = contador + tablero[x-1][j+1];

					}else{

						// Caso 6 Lado izquierdo
						if (j == 0)
						{
							// Pide los valores que estan fuera del rango
							contador = contador + tablero[i-1][y-1];
							contador = contador + tablero[i][y-1];
							contador = contador + tablero[i+1][y-1];

						}else{

							// Caso 7 Lado derecho
							if (j == y-1)
							{
								// Pide los valores que estan fuera del rango
								contador = contador + tablero[i-1][0];
								contador = contador + tablero[i][0];
								contador = contador + tablero[i+1][0];

							}else{
								// Caso 8 Lado inferior
								if(i == x-1){

									// Pide los valores que estan fuera del rango
									contador = contador + tablero[0][j-1];
									contador = contador + tablero[0][j];
									contador = contador + tablero[0][j+1];

								}
							}
						}
					}
				}
			}
		}
	}

	// Retorna el numero de vecinos vivos que tiene una celula

	return contador;
}
