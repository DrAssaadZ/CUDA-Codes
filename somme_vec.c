#include <stdio.h>
#include <stdlib.h>

#define N 23

__global__ void add (int *a, int *b, int *c){
    
   int idx =threadIdx.x+blockIdx.x*blockDim.x;
    //int idx =blockIdx.x;     
      c[idx]=a[idx] + b[idx];
}


int main (void) {
int *a, *b, *c;
int *dev_a, *dev_b, *dev_c;

// allocation de mémoire sur le GPU
cudaMalloc((void **)&dev_a, N*sizeof(int));
cudaMalloc((void **)&dev_b, N*sizeof(int));
cudaMalloc((void **)&dev_c, N*sizeof(int));
cudaMemset(dev_c, 0, N*sizeof(int));
//cudaMemset(dev_c, N);

a= (int *) calloc (N, sizeof(int));
b= (int *) calloc (N, sizeof(int));
c= (int *) calloc (N, sizeof(int));

// remplissage de a et b

for(int i=0; i<N; i++){
  a[i]= -i+4;
  b[i]= i*i;
}

// copie des tableaux a et b sur GPU

cudaMemcpy(dev_a, a, N*sizeof(int), cudaMemcpyHostToDevice);
cudaMemcpy(dev_b, b, N*sizeof(int), cudaMemcpyHostToDevice);

// appel du kernal
add<<<(N+4)/5 ,5>>>(dev_a, dev_b, dev_c);

// copie du tableau c du GPU vers CPU

cudaMemcpy(c, dev_c, N*sizeof(int), cudaMemcpyDeviceToHost);

// affichage

for(int i=0; i<N; i++)
   printf("%d + %d = %d \n", a[i], b[i], c[i]);

// libération GPU

cudaFree(dev_a);
cudaFree(dev_b);
cudaFree(dev_c);
free(a);
free(b);
free(c);
return 0;
}
