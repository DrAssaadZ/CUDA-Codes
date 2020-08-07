#include <stdio.h>
#include <stdlib.h>

static void HandleError( cudaError_t err, const char *file, int line ) {
    if (err != cudaSuccess) {
        printf( "%s in %s at line %d\n", cudaGetErrorString( err ), file, line );
        exit( EXIT_FAILURE );
    }
}
#define HANDLE_ERROR( err ) (HandleError( err, __FILE__, __LINE__ ))

__global__ void add(int *a, int *b, int *c) {
        *c = *a + *b;
}
int main(void) {
	int a, b, c;   // host copies of a, b, c
	int *d_a, *d_b, *d_c; // device copies of a, b, c
        size_t size = 1024*1024*1024*sizeof(char);
        //size_t size = sizeof(char);
	// Allocate space for device copies of a, b, c
        cudaMalloc((void **)&d_a, size);
	HANDLE_ERROR(cudaMalloc((void **)&d_b, size));
	HANDLE_ERROR(cudaMalloc((void **)&d_c, size));
	// Setup input values
        printf("Enter a and b values:\n");
        scanf("%d",&a); scanf("%d",&b);
	//a = 2; b = 7;
	// Copy inputs to device
	HANDLE_ERROR(cudaMemcpy(d_a, &a, size, cudaMemcpyHostToDevice));
	HANDLE_ERROR(cudaMemcpy(d_b, &b, size, cudaMemcpyHostToDevice));
	// Launch add() kernel on GPU
	add<<<1,1>>>(d_a, d_b, d_c);
	// Copy result back to host
	HANDLE_ERROR(cudaMemcpy(&c, d_c, 1, cudaMemcpyDeviceToHost));
	printf("addition result: %d\n", c);
        // Cleanup
	cudaFree(d_a); cudaFree(d_b); cudaFree(d_c);
	return 0;
}
