#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <omp.h>
#include "papi.h"


#ifdef L1
#define SIZE 52 //Matrix size to fit L1 cache
#endif

#ifdef L2
#define SIZE 147 //Matrix size to fit L2 cache
#endif

#ifdef L3
#define SIZE 1329 //Matrix size to fit L3 cache
#endif

#ifdef EXT_MEM
#define SIZE 2659 //Matrix size to fit external memory
#endif

#ifndef SIZE
#define SIZE 2659 //Default size
#endif

#define TIME_RESOLUTION 1000000
#define NUM_EVENTS 5
#define NUM_THREADS 48

long long values[NUM_EVENTS];
unsigned int Events[NUM_EVENTS]={PAPI_TOT_CYC,PAPI_TOT_INS,PAPI_L1_DCM,PAPI_L2_TCM,PAPI_L3_TCM};

#if !defined(IMP2) && !defined(IMP3)
int matrix_multiplication(float **A, float **B, float **C){
    #pragma omp parallel 
        {
        #pragma omp for simd aligned (A,B,C:16)
        for(int i = 0 ; i < SIZE ; i++){
            for(int j = 0 ; j < SIZE ; j++){
                for(int k = 0 ; k < SIZE ; k++){
                    #ifndef TRANS
		                C[i][j] += A[i][k]*B[k][j];
		            #else
		                C[i][j] += A[i][k]*B[j][k];
		            #endif
                } 
            }
        }
    }

    return 0;
}
#endif

#ifdef IMP2
int matrix_multiplication(float **A, float **B, float **C){
    #pragma omp parallel 
        {
        #pragma omp for simd aligned (A,B,C:16)
        for(int i = 0 ; i < SIZE ; i++){
            for(int k = 0 ; k < SIZE ; k++){
	            for(int j = 0 ; j < SIZE ; j++){
		            C[i][j] += A[i][k]*B[k][j];
                }
            }
        }
    }

    return 0;
}
#endif

#ifdef IMP3
int matrix_multiplication(float **A, float **B, float **C){
    #pragma omp parallel
        {
        #pragma omp for simd aligned (A,B,C:16)
        for(int j = 0 ; j < SIZE ; j++){
            for(int k = 0 ; k < SIZE ; k++){
                for(int i = 0 ; i < SIZE ; i++){
                    #ifndef TRANS
		            C[i][j] += A[i][k]*B[k][j];
		            #else
		            C[i][j] += A[k][i]*B[j][k];
		            #endif
                }
            }
        }
    }

    return 0;
}
#endif



//Fills the matrices
void fill_matrices(float **A, float **B, float **C) {
    for (unsigned int i = 0; i < SIZE; i++) {
        for (unsigned int j = 0; j < SIZE; j++) {
            A[i][j] = ((float) rand()) / ((float) RAND_MAX);
            B[i][j] = 1;
            C[i][j] = 0;
        }
    }
}


void transpose_matrix(float **X){
    float x;
    for(unsigned int i = 1; i < SIZE ; i++){
        for(unsigned int j = 0 ; j < i ; j++){
            x = X[i][j];
            X[i][j] = X[j][i];
            X[j][i] = x;
        }
    }
}

int confirm_result(float **A, float **B, float **C)
{
    for (int i = 0; i < SIZE; i++)
    {
        for (int j = 0; j < SIZE; j++)
        {
            float cur = 0;
            for (int k = 0; k < SIZE; k++)
            {
                cur += B[i][k] * A[k][j];
            }
            if (cur != C[j][i])
            {
                printf("I: %d, J: %d\n",i,j);
                printf("%2.6f, %2.6f\n",cur,C[j][i]);
                return 0;
            }
        }
    }

    return 1;
}

int main(int argc, char* argv[]){
   
    omp_set_num_threads(NUM_THREADS);
    float **A, **B, **C;
    A = (float **) malloc(SIZE * sizeof(float *));
    B = (float **) malloc(SIZE * sizeof(float *));
    C = (float **) malloc(SIZE * sizeof(float *));

    for(unsigned int i = 0;i < SIZE;i++){
        A[i] = (float *) malloc(SIZE * sizeof(float));
        B[i] = (float *) malloc(SIZE * sizeof(float));
        C[i] = (float *) malloc(SIZE * sizeof(float));
    }
    fill_matrices(A,B,C);

    #ifdef TRANS
    #if !defined(IMP2) && !defined(IMP3)
	transpose_matrix(B);
    #endif
    #ifdef IMP3
	transpose_matrix(A);
	transpose_matrix(B);
    #endif
    #endif

    PAPI_start_counters((int*)Events,NUM_EVENTS);

    //Starts timer
    struct timeval t;
    gettimeofday(&t, NULL);
    long long unsigned int initial_time = t.tv_sec * TIME_RESOLUTION + t.tv_usec;

    matrix_multiplication(A,B,C);

    //Stops timer
    gettimeofday(&t, NULL);
    long long unsigned final_time = t.tv_sec * TIME_RESOLUTION + t.tv_usec;

    PAPI_stop_counters(values,NUM_EVENTS);

    printf("Total Cycles: %lld\nTotal Instructions: %lld\nL1 Data Cache Misses: %lld\nL2 Cache Misses: %lld\nL3 Cache Misses: %lld\n",values[0],values[1],values[2],values[3],values[4]);

    //confirm_result

    printf("Execution time: %lld \xC2\xB5s\n",final_time-initial_time);

    for(unsigned int i = 0 ; i < SIZE ; i++){
	free(A[i]);
	free(B[i]);
	free(C[i]);
    }

    free(A);
    free(B);
    free(C);

    return 0;
}
