#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

/* Funcion que libera memoria de los vectores

	Se utilizara para liberar la memoria que se requiere para generar el tablero original y el que se genera
	en cada iteracion.
*/

void limpiar(int **vector, int x){

	for (int i = 0; i < x; i++)
	{
		free(vector[i]);
	}

	free(vector);

}

/* Funcion que copia el tablero siguiente en el original para volver a iterar


	Se utiliza al momento de terminar una iteración para volver al ciclo de trabajo.

*/
int copiaTablero(int **original, int **copia, int x, int y){
	

	for (int i = 0; i < x; i++)
	{
		for (int j = 0; j < y; j++)
		{
			original[i][j] = copia[i][j] ;
		}
	}

	return 1;
}

/* Funcion encargada de contar el numero de vecinos vivos que tiene una celula en particular
	
	Argumentos:

	i posicion X en la matriz
	j posicion Y en la matriz

	X filas de la matriz
	Y columnas de la matriz

	Retorna el numero de vecinos vivos que tiene una celula en el tablero.

*/

int RevisarVecinos(int **tablero,int i, int j, int x, int y){

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

// Funcion calcula nuevos valores al tablero
/*
	Recibe como argumento el tablero inicial de la iteracion, un tablero de destino que corresponde al resultado de la iteración
	y el tamaño del tablero para controlar los limites a explorar.
	Para cada celula del tablero, obtiene la cantidad de vecinos vivos que tiene, y aplica las reglas del juego para determinar
	el siguiente estado de cada celula.
*/

void calcularVida(int **tableroInicial, int **tableroDestino, int x, int y){


	int vecinosVivos = 0;
	int valor = 0;
	int status = 0;

	for (int i = 0; i < x; i++)
	{
		for (int j = 0; j < y; j++)
		{
			
				// Obtiene la cantidad de vecinos vivos de una celula
				vecinosVivos = RevisarVecinos(tableroInicial,i,j,x,y);
				// Obtiene el estado actual de la celula a analizar
				valor = tableroInicial[i][j];

				if (valor == 1) // Celula esta viva
				{
					if (vecinosVivos < 2)
					{	
						// Celula muere	por soledad
						status = 0;
					}else{

						if (vecinosVivos > 3)
						{
							// Celula muere por sobrepoblación
							status = 0;
						}else{
							if(vecinosVivos == 2 || vecinosVivos == 3){

								status = 1;
									
							}
							
						}

					}
				}
				else{ // Celula esta muerta

					if(vecinosVivos == 3){
						// La celula cambia de muerta a viva en el siguiente turno
						status = 1;
					}
					else{
						status = 0;
					}
				}

				
				// Actualiza la posicion de la matriz de copia
				tableroDestino[i][j] = status;
				status = 0;
		}
	}

	int copiar = copiaTablero(tableroInicial,tableroDestino, x,y);
}

/* Funcion Mostrar Tablero
	
	Es la encargada de mostrar por pantalla el estado del tablero en un momento dado.
	Se representa con un simbolo entre [ ].
	Si la celula esta muerta se representa por --> [   ]
	Si la celula esta viva se representa por --> [ * ] 

*/
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


// Funcion Principal

int main(int argc, char *argv[])
{
	int ContadorIteracion = 0;
	int iteraciones;
	char *ruta;
	char c;
	int tamanoX;
	int tamanoY;
	int leido;
	FILE *fichero;

	// Obtener argumentos
	/*
		
		f = Ruta del fichero
		i = numero de iteraciones

	*/


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

	// Leer el archivo recibido como parametro.

	fichero = fopen(ruta, "r");
	fscanf(fichero, "%d %d", &tamanoX, &tamanoY);

	printf("Tamano x = %d  y = %d\n",  tamanoX, tamanoY);


	// Solicita memoria para generar el tablero.

	int  **unTablero = (int **)malloc(tamanoX * sizeof(int*));
	int  **tableroCopia = (int **)malloc(tamanoX * sizeof(int*));


	for (int i = 0; i < tamanoX; i++)
	{
		unTablero[i] = (int*)malloc(tamanoY * sizeof(int));
		tableroCopia[i] = (int*)malloc(tamanoY * sizeof(int));
	}



	printf("Bienvenido al juego de la vida....\n");
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
	printf("Presione una tecla para iniciar el juego de la vida\n");
	fclose(fichero);
	getchar();

	while(ContadorIteracion < iteraciones){

		system("clear");
		printf("Juego de la vida...\n");
		printf("Iteracion: %d\n", ++ContadorIteracion);

		// Calcula las nuevas posiciones para el tablero en "unTablero"
		calcularVida(unTablero, tableroCopia, tamanoX, tamanoY);

		// Muestra el tablero al terminar una iteracion
		mostraTablero(unTablero, tamanoX, tamanoY);

	}

	limpiar(unTablero, tamanoX);
	limpiar(tableroCopia,tamanoX);
	printf("Fin del programa\n");

	return 0;
}