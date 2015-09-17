#include <stdio.h>
#include <mpi.h>

int main(int argc, char  *argv[]) {
  /* code */
  int nprocs;			/* numero de procesos */
  int rank;			/* Id de cada proceso */

  int i, mysum, sum, imin, imax;
  const int N = 60;

/* initialize the MPI environment: */

MPI_Init (&argc, &argv);
MPI_Comm_rank (MPI_COMM_WORLD, &rank); /* Obtiene el id del proceso */
MPI_Comm_size (MPI_COMM_WORLD, &nprocs);  /* Obtiene la cantidad de procesos en total*/

printf ("Hello, this is process %d of a total of %d\n", rank, nprocs);

imin = (N / nprocs) * rank + 1;
imax = imin + N / nprocs -1;
mysum = mysum + 1;
printf("Proceso %d la suma es %d antes del broadcast\n", rank, mysum );


MPI_Reduce (&mysum,&sum,1,MPI_INT,MPI_SUM,0,MPI_COMM_WORLD);
MPI_Barrier(MPI_COMM_WORLD);
if (rank == 0){
    printf ("process zero reports: sum is %d\n", sum);

}
MPI_Bcast(&sum, 1,MPI_INT,0,MPI_COMM_WORLD);
MPI_Barrier(MPI_COMM_WORLD);


printf ("process %d reports: mysum is %d and sum is %d\n", rank, mysum,
	  sum);

MPI_Allreduce (&mysum, &sum, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);

printf ("process %d reports again: mysum is %d and sum is %d\n",
   rank, mysum, sum);

MPI_Finalize ();




  return 0;
}
