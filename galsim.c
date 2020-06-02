//#include "graphics.h"
#include "read_file.h"
#include "quadTree.c"
//#include <GLUT/glut.h>
#include <math.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
//#define MAX_THREADS 4


//declaration
void Force_calc();
void updateBarnes();
struct force particleForce(const int i, treeNode *node);
void worker();
void *updatebarnes_parallel(void *arg);

//PTHREAD
typedef struct data{
	int from;
	int to;
	treeNode **root;
}data_t;
void *status;
struct force Force;
int NUM_THREADS;

// Force struct
struct force {
	double Xforce;
	double Yforce;
};
// Constant parameters
int N;
double graphics;
int nsteps;
double delta_t;
double theta_max;
double e0 = 1e-3;
double G;
int iter;


// Data
double *p;
double *x,*y,*mass,*forceX,*forceY,*u,*v, *brightness;
treeNode* rootNode;

// Timers
double totalRunTime = 0;    // time of running the program
double totalCodeTime = 0;   // time of running my code
double startRunTime;
double startCodeTime;
double endTime;

// Timing function
static double get_wall_seconds() {
	struct timeval tv;
	gettimeofday(&tv, NULL);
	double seconds = tv.tv_sec + (double)tv.tv_usec / 1000000;
	return seconds;
}


int main(int argc, char *argv[]) {

	startRunTime = get_wall_seconds();

	// Check input parameters
	if (argc != 8) {
		printf("Input Error!\nRun: $ ./galsim N filename nsteps delta_t theta_max graphics num_threads\n");
		return -1;
	}

	// Read input parameters
	sscanf(argv[4],"%lf",&delta_t);
	N = atoi(argv[1]);
	nsteps = atoi(argv[3]);
	graphics = atoi(argv[6]);
	sscanf(argv[5], "%lf", &theta_max);
	NUM_THREADS = atoi(argv[7]);
	// Calculate gravitational constant
	G = 100.0/N;

	// Allocate memory for variables
	x = (double *)malloc(N*sizeof(double));
	y = (double *)malloc(N*sizeof(double));
	u = (double *)malloc(N*sizeof(double));
	v = (double *)malloc(N*sizeof(double));
	mass = (double *)malloc(N*sizeof(double));
	brightness = (double *)malloc(N*sizeof(double));
	forceX = (double *)malloc(N*sizeof(double));
	forceY = (double *)malloc(N*sizeof(double));
	rootNode = (treeNode *)malloc(sizeof(treeNode));
	//Force = (force *)malloc(sizeof(force));
	// Use read_input_to_vector to read the values from file to the variables
	read_input_to_vector(N, argv[2], x, y, u, v, mass, brightness);

	// Starting the coding time
	startCodeTime = get_wall_seconds();
	rootNode = NULL;
	// Update force
	worker();
	// writting the result to the file "result.gal

	endTime = get_wall_seconds();
	//totalRunTime =  endTime - startRunTime;
	totalCodeTime +=  endTime - startCodeTime;
	//printf("Total running time: %f\n", totalRunTime);
	printf("Time for algorithm: %f\n", totalCodeTime);

	write_from_vector(N, "result.gal", x, y, u, v, mass, brightness);
	printf("Result has been writen!\n");
	//free_mat(N, matrix);
	free(mass);
	free(x);
	free(y);
	free(u);
	free(v);
	free(forceX);
	free(forceY);
	free(brightness);

	// TIME
	endTime = get_wall_seconds();
	totalRunTime =  endTime - startRunTime;

	//totalCodeTime +=  get_wall_seconds() - startCodeTime;
	printf("Total Run time::%f\n", totalRunTime);
	times_to_file(N, "times.txt", totalRunTime, totalCodeTime);	

	return 0;
}



// THE METHOD THAT CALCULATED THE ACCELERATION, VELOCITIES AND POSITIONS
void worker(){
	iter = 0;
	while (iter != nsteps){
		//updateForce();
		updateBarnes();
		double ax, ay;

		for(int i = 0; i < N; i++) {
			// Acceleration
			ax=forceX[i]/mass[i];
			ay=forceY[i]/mass[i];
			// Positions and velocities
			u[i] += ax*delta_t;
			v[i] += ay*delta_t;
			x[i] += u[i]*delta_t;
			y[i] += v[i]*delta_t;
		}

		iter++;
	}
}


// HERE WE UPDATE THE FORCE, CALCULATING THE FORCE.
void Force_calc(){
    double sumX, sumY;
    double denom;
    double distX, distY;
    double norm;
    double xi, yi, xj,yj;
    
    for(int i = 0 ; i<N;i++){
        sumX = sumY = 0;
        xi = x[i];
        yi = y[i];
        for(int j = 0; j<N ;j++){
            if(i != j){
                xj = x[j];
                yj = y[j];
                distX = xi - xj;
                distY = yi - yj;
                norm = sqrt(distX*distX + distY*distY);
                denom = norm + e0;
                sumX += mass[j]*distX/(denom*denom*denom);
                sumY += mass[j]*distY/(denom*denom*denom);
            }
        }
        forceX[i] = -G*mass[i]*sumX;
        forceY[i] = -G*mass[i]*sumY;
    }
}



// UPDATE THE FORCE USING BARNES HUT ALGORITHM (QUADTREE)
void updateBarnes(){
	if(rootNode){
		free_tree(rootNode);
	}
	rootNode = (treeNode*)malloc(sizeof(treeNode));
	create_tree(N);
	get_CenterOfMass(rootNode);

	pthread_t threads[NUM_THREADS];

	pthread_attr_t attr;
	pthread_attr_init(&attr);

	data_t argarr[NUM_THREADS];
	
	for (int i=0; i<NUM_THREADS; i++){
		if (N%NUM_THREADS == 0){
			argarr[i].from = i*N/NUM_THREADS;
			argarr[i].to = (i + 1)*N/NUM_THREADS;
		} else {
			int divisable = N - N%NUM_THREADS;
			argarr[i].from = i*divisable/NUM_THREADS;
			argarr[i].to = (i+1)*divisable/NUM_THREADS;
			argarr[NUM_THREADS - 1].to += N%NUM_THREADS;
		}
		pthread_create(&threads[i], &attr, updatebarnes_parallel, (void *)&argarr[i]);
	}
	//printf("to %d", argarr[NUM_THREADS - 1].to);
	pthread_attr_destroy(&attr);

	for (int t=0; t<NUM_THREADS; t++){
		pthread_join(threads[t], &status);
	}

}


void *updatebarnes_parallel(void *arg){
	data_t *arg_data = (data_t *)arg;
	//printf("%d\n", arg_data->from);
	//int i = arg_data->from;
	for (int i = arg_data->from; i < arg_data->to; i++){
		struct force Force = particleForce(i,(rootNode));
		forceX[i]=Force.Xforce;
		forceY[i]=Force.Yforce;

	}
	pthread_exit(NULL);
}


// THIS IS THE METHOD THAT UPDATE BARNES USES TO CALCULATE THE FORCE, USING THETA VALUE
struct force particleForce(const int i, treeNode *node){
	struct force Force;
	double r_x, r_y,avst,denom;
	if(node->isLeaf ){// Om löv finns
		// Kolla hur många partiklar
		if(node->numOfPar == 0 || node->id == i){
			Force.Xforce = 0;
			Force.Yforce = 0;
			return Force;
		}else{
			int j = node->id;
			r_x = x[i] - x[j];
			r_y = y[i] - y[j];
			// Beräkna kraften som förut
			avst = sqrt(r_x*r_x + r_y * r_y);
			denom = (avst + e0);
			// The forces 
			Force.Xforce = -G*mass[i]*mass[j]*r_x/(denom*denom*denom);
			Force.Yforce = -G*mass[i]*mass[j]*r_y/(denom*denom*denom);
		}
	}else{
		// width of current box containing the particles
		double widht = node->Rborder - node->Lborder; //The width of the group
		// Distance from particle to center of box
		double center_box_x = node->Lborder+widht/2;
		double center_box_y = node->Dborder+widht/2;

		double r1 = center_box_x - x[i];
		double r2 = center_box_y - y[i];
		double distance = sqrt(r1*r1 + r2*r2);

		// Calculating theta
		double Theta = widht/distance;

		// Checking the consition that whether theta is between 0,1;
		if(Theta < theta_max){
			//box is treated as an equivalent mass
			r_x = x[i] - node->centerX;
			r_y = y[i] - node->centerY; 
			// distance between part x and y
			avst = sqrt(r_x*r_x + r_y*r_y); 
			// denomerator
			denom = avst + e0;
			Force.Xforce = -G*mass[i]*node->nodeMass*r_x/(denom*denom*denom);
			Force.Yforce = -G*mass[i]*node->nodeMass*r_y/(denom*denom*denom);
			return Force;
		}else{
			// Recursively compute the force on every particle
			/*
			   treeNode *tmp_node = malloc(sizeof(treeNode));
			   node = tmp_node;*/
			struct force ForceNW = particleForce(i, node->NW);
			struct force ForceSW = particleForce(i, node->SW);
			struct force ForceNE = particleForce(i, node->NE);
			struct force ForceSE = particleForce(i, node->SE);

			Force.Xforce  = ForceNW.Xforce + ForceSW.Xforce + ForceNE.Xforce + ForceSE.Xforce;
			Force.Yforce  = ForceNW.Yforce + ForceSW.Yforce + ForceNE.Yforce + ForceSE.Yforce;
		}
	}
	return Force;

}

