#include <stdio.h>
#include <mpi.h>

int main(int argc, char *argv[]) {
  /* code */
  int left, right, send_data, recv_data;
  int rank, nprocs;

  MPI_Status status;

  MPI_Init (&argc, &argv);
  MPI_Comm_rank (MPI_COMM_WORLD, &rank);
  MPI_Comm_size (MPI_COMM_WORLD, &nprocs);

  left = rank - 1;
  right = rank + 1;

  // Repara los limites
  if (left < 0){
      left = nprocs - 1;
    }
 if (right > nprocs - 1){
    right = 0;
  }


send_data = rank;
recv_data = -1;

//envio de datos
MPI_Sendrecv (&send_data,1,MPI_INT,right,0,&recv_data,1,MPI_INT,left,0,MPI_COMM_WORLD,&status);

printf ("Process %d received %d from %d and sent %d to %d\n", rank, recv_data, left, send_data, right);

MPI_Finalize ();

  return 0;
}
