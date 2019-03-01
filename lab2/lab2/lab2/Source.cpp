#define _CRT_SECURE_NO_WARNINGS
#include <mpi.h> 
#include <stdio.h> 

void show(double** a, int n) {
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			printf("a[%d][%d] = %lg, ", i, j, a[i][j]);
		}
		printf("\n");
	}
}

double f1(double** a, int* map, int myrank, int nprocs, int n) {

	for (int k = 0; k < n - 1; k++) {
		if (map[k] == myrank) {
			for (int i = k + 1; i < n; i++) {
				a[k][i] /= a[k][k];
			}
		}

		MPI_Bcast(&a[k][k + 1], n - k - 1, MPI_DOUBLE, map[k], MPI_COMM_WORLD);

		for (int i = k + 1; i < n; i++) {
			if (map[i] == myrank) {
				for (int j = k + 1; j < n; j++) {
					a[i][j] -= a[i][k] * a[k][j];
				}
			}
		}
	}

	//if (myrank == 0) {
	//	show(a, n);
	//}

	return 0;
}

// ������������ �� �������
double f2(double** a, int* map, int myrank, int nprocs, int n) {

	for (int k = 0; k < n - 1; k++) {
		int indMax = k;
		if (map[k] == myrank) {

			double max = a[k][indMax];

			for (int j = k + 1; j < n; j++) {
				if (map[j] == myrank && a[k][j] > max) {
					max = a[k][j];
					indMax = j;
				}
			}

			if (k != indMax) {
				for (int j = 0; j < n; j++) {
					double temp = a[j][k];
					a[j][k] = a[j][indMax];
					a[j][indMax] = temp;
				}
			}

			for (int i = k + 1; i < n; i++) {
				a[k][i] /= a[k][k];
			}
		}

		MPI_Status status;
		for (int j = 0; j < n; j++) {
			if (map[k] == myrank) {
				if (map[j] != myrank) {
					MPI_Send(&a[j][k], 1, MPI_DOUBLE, map[j], 1, MPI_COMM_WORLD);
					MPI_Send(&indMax, 1, MPI_INT, map[j], 1, MPI_COMM_WORLD);
					MPI_Send(&a[j][indMax], 1, MPI_DOUBLE, map[j], 1, MPI_COMM_WORLD);
				}

			}
			else {
				if (map[j] == myrank) {
					int  recvInd;
					MPI_Recv(&a[j][k], 1, MPI_DOUBLE, map[k], 1, MPI_COMM_WORLD, &status);
					MPI_Recv(&recvInd, 1, MPI_INT, map[k], 1, MPI_COMM_WORLD, &status);
					MPI_Recv(&a[j][recvInd], 1, MPI_DOUBLE, map[k], 1, MPI_COMM_WORLD, &status);
				}

			}
			//MPI_Bcast(&a[j][k], 1, MPI_DOUBLE, map[k], MPI_COMM_WORLD);
			//MPI_Bcast(&a[j][indMax], 1, MPI_DOUBLE, map[k], MPI_COMM_WORLD);
		}
		
		MPI_Barrier(MPI_COMM_WORLD);

		for (int i = k + 1; i < n; i++) {
			if (map[i] == myrank) {
				for (int j = k + 1; j < n; j++) {
					a[i][j] -= a[i][k] * a[k][j];
				}
			}
		}
	}

	return 0;
}

// ������������ �� �������
double f3(double** a, int* map, int myrank, int nprocs, int n) {

	for (int k = 0; k < n - 1; k++) {
		int indMax = k;
		if (map[k] == myrank) {

			double max = a[indMax][k];

			for (int j = k + 1; j < n; j++) {
				if (map[j] == myrank && a[j][k] > max) {
					max = a[j][k];
					indMax = j;
				}
			}

			if (k != indMax) {
				double* temp = a[k];

				a[k] = a[indMax];
				a[indMax] = temp;
			}

			for (int i = k + 1; i < n; i++) {
				a[k][i] /= a[k][k];
			}
		}

		MPI_Bcast(&a[indMax][0], n, MPI_DOUBLE, map[k], MPI_COMM_WORLD);
		MPI_Bcast(&a[k][k + 1], n - k - 1, MPI_DOUBLE, map[k], MPI_COMM_WORLD);
		MPI_Barrier(MPI_COMM_WORLD);

		for (int i = k + 1; i < n; i++) {
			if (map[i] == myrank) {
				for (int j = k + 1; j < n; j++) {
					a[i][j] -= a[i][k] * a[k][j];
				}
			}
		}
	}

	return 0;
}



// ������������ �� �������
double f4(double** a, int* map, int myrank, int nprocs, int n) {

	for (int k = 0; k < n - 1; k++) {
		int indMaxR = k, indMaxC = k;
		if (map[k] == myrank) {

			double max = a[indMaxR][indMaxC];

			for (int i = k; i < n; i++) {
				for (int j = k; j < n; j++) {
					if (map[i] == myrank && map[j] == myrank && a[i][j] > max) {
						max = a[i][j];
						indMaxR = i;
						indMaxC = j;
					}
				}
			}

			if (k != indMaxC) {
				for (int j = 0; j < n; j++) {
					double temp = a[j][k];
					a[j][k] = a[j][indMaxC];
					a[j][indMaxC] = temp;
				}
			}

			if (k != indMaxR) {
				double* temp = a[k];

				a[k] = a[indMaxR];
				a[indMaxR] = temp;
			}

			for (int i = k + 1; i < n; i++) {
				a[k][i] /= a[k][k];
			}
		}

		MPI_Status status;
		for (int j = 0; j < n; j++) {
			if (map[k] == myrank) {
				if (map[j] != myrank) {
					MPI_Send(&a[j][k], 1, MPI_DOUBLE, map[j], 1, MPI_COMM_WORLD);
					MPI_Send(&indMaxC, 1, MPI_INT, map[j], 1, MPI_COMM_WORLD);
					MPI_Send(&a[j][indMaxC], 1, MPI_DOUBLE, map[j], 1, MPI_COMM_WORLD);
				}

			}
			else {
				if (map[j] == myrank) {
					int  recvInd;
					MPI_Recv(&a[j][k], 1, MPI_DOUBLE, map[k], 1, MPI_COMM_WORLD, &status);
					MPI_Recv(&recvInd, 1, MPI_INT, map[k], 1, MPI_COMM_WORLD, &status);
					MPI_Recv(&a[j][recvInd], 1, MPI_DOUBLE, map[k], 1, MPI_COMM_WORLD, &status);
				}

			}
			//MPI_Bcast(&a[j][k], 1, MPI_DOUBLE, map[k], MPI_COMM_WORLD);
			//MPI_Bcast(&a[j][indMax], 1, MPI_DOUBLE, map[k], MPI_COMM_WORLD);
		}

		MPI_Bcast(&a[indMaxR][0], n, MPI_DOUBLE, map[k], MPI_COMM_WORLD);
		MPI_Bcast(&a[k][k + 1], n - k - 1, MPI_DOUBLE, map[k], MPI_COMM_WORLD);
		MPI_Barrier(MPI_COMM_WORLD);

		for (int i = k + 1; i < n; i++) {
			if (map[i] == myrank) {
				for (int j = k + 1; j < n; j++) {
					a[i][j] -= a[i][k] * a[k][j];
				}
			}
		}
	}

	return 0;
}

int main(int argc, char *argv[])
{
	int myrank, nprocs;
	int N[] = { 10,50,100,500,1000 };
	FILE *f = fopen("res.txt", "w");

	MPI_Init(&argc, &argv);

	MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
	MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

	for (int fun = 0; fun < 4; fun++) {
		fprintf(f, "�������� n\t ����� ���������� (� ���.)\t �������� ����������\n");
		for (int k = 0, n = N[k]; k < 5; k++, n = N[k]) {

			int *map = new int[n];
			double **a = new double*[n];

			// �������������
			for (int i = 0; i < n; i++) {
				a[i] = new double[n];
				for (int j = 0; j < n; j++) {
					a[i][j] = 1 / (1.0 * (i + j + 1));
				}
			}

			for (int i = 0; i < n; i++) {
				map[i] = i % nprocs;
			}

			double time = 0, error = 0;
			switch (fun) {
			case 0:
				time = MPI_Wtime();
				error = f1(a, map, myrank, nprocs, n);
				time = MPI_Wtime() - time;
				break;
			case 1:
				time = MPI_Wtime();
				error = f2(a, map, myrank, nprocs, n);
				time = MPI_Wtime() - time;
				break;
			case 2:
				time = MPI_Wtime();
				error = f3(a, map, myrank, nprocs, n);
				time = MPI_Wtime() - time;
				break;
			case 3:
				time = MPI_Wtime();
				error = f4(a, map, myrank, nprocs, n);
				time = MPI_Wtime() - time;
				break;
			}

			fprintf(f, "%d\t %lg\t %lg\n", n, time, error);

			delete map, a;
		}
	}

	MPI_Finalize();
	fclose(f);

	return 0;
}