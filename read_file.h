#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void read_input(int N, char input_file[], double **matrix){
	FILE *file;
	int i, j, block = 6;
	double buff[6];
	file = fopen(input_file,"r");
	for (i=0;i<N;i++){
		fread(buff, sizeof(double), block, file);
		//matrix[i] = *buff;
		for (j=0;j<block;j++){
			matrix[i][j] = buff[j];
		}
	}
	fclose(file);
}

void read_input_to_vector(int N, char input_file[], double x[], double y[], double v_x[], double v_y[], double mass[], double brightness[]){
	FILE *file;
	int i, block=6;
	double buff[6];
	file = fopen(input_file, "r");
	for (i=0;i<N;i++){
		fread(buff, sizeof(double), block, file);
		x[i] = buff[0];
		y[i] = buff[1];
		mass[i] = buff[2];
		v_x[i] = buff[3];
		v_y[i] = buff[4];
		brightness[i] =buff[5];
	}
	fclose(file);
}

void write_from_vector(int N, char output_name[], double x[], double y[], double v_x[], double v_y[], double mass[], double brightness[]){
	FILE *file;
	int i, block=6;
	double buff[6];
	file = fopen(output_name, "w");
	for (i=0;i<N;i++){
		buff[0] = x[i];
		buff[1] = y[i];
		buff[2] = mass[i];
		buff[3] = v_x[i];
		buff[4] = v_y[i];
		buff[5] = brightness[i];
		fwrite(buff, sizeof(double), block, file);
	}
	fclose(file);
}


void write_to_res(int N, char output_name[], double **matrix){
	FILE *file;
	int i,j, block = 6;
	file = fopen(output_name, "w");
	double buff[6];
	for (i=0;i<N;i++){
		for (j=0;j<block;j++){
			buff[j] = matrix[i][j];
		}
		fwrite(buff, sizeof(double), block, file);

	}
	fclose(file);
}


double** init_mat(int n){
	int i, num_col = 6;
	double** matrix = (double **)malloc(n*sizeof(double *));
	for (i=0;i<n;i++)
		matrix[i] = (double *)malloc(num_col*sizeof(double *));
	return matrix;
}

void print_matrix(double** matrix, int n){
	int i,j, num_col = 6;
	for(i=0;i<n;i++){
		for(j=0;j<num_col;j++)
			printf(" %f", matrix[i][j]);
		putchar('\n');
	}
}

void free_mat(int n, double **matrix){
	for (int i = 0; i < n; i++)
		free(matrix[i]);
	free(matrix);
}

void times_to_file(int N, char file_name[], double run_time, double alg_time){
	FILE *file;
	char buff[3];
	file = fopen(file_name, "a");
	char str_buff[30];
	sprintf(str_buff, "%d", N);
	fprintf(file,"%s" ,str_buff);
	fprintf(file, "%s", " ");
	sprintf(str_buff, "%f", run_time);
	fprintf(file, "%s", str_buff);
	fprintf(file, "%s", " ");
	sprintf(str_buff, "%f", alg_time);
	fprintf(file , "%s", str_buff);
	fprintf(file, "%s", "\n");
	fclose(file);
}



/*
   int main(){
// skapa variabel dubbel ptr
double **matrix;
// n ska komma från argv bara för test
int n = 20;
//alloc mem
matrix = init_mat(n);
//printar bara för att se vad som händer
//print_matrix(matrix, n);
read_input(n, "t.gal", matrix);
print_matrix(matrix, n);
write_to_res(n, "result.gal", matrix);
free(matrix);
}
*/
