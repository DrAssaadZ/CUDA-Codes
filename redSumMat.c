#include <stdio.h>
#include <stdlib.h>

#define N 10 
#define M 8 

__global__ void add (int *a, int *s){
   __shared__ int sv[M]; 
   int idx =threadIdx.x  + blockIdx.x*blockDim.x, i;
   int stride= blockDim.x*gridDim.x;
   sv[threadIdx.x]=0;
   while (idx<N*M){
        sv[threadIdx.x]+=a[idx];
        idx+=stride;
   }
   __syncthreads();
   idx=threadIdx.x;
   i=blockDim.x/2;
   while (i != 0){
      if(idx < i){ 
         sv[idx]+=sv[idx+i];
      }
      __syncthreads();
      i /=2;
   }    
   
   if (idx== 0);
       atomicAdd(s,sv[0]);
}


int main (void) {
     int *a, *dev_a;
     int *sum, *dev_sum, i;
     //allocaion de mémloire sur le Host

     a=(int *) malloc(N*M*sizeof(int));
     sum= (int*) malloc(sizeof(int));

     // allocation de mémoire sur le GPU
     cudaMalloc((void **)&dev_a, N*M*sizeof(int));
     cudaMalloc((void **)&dev_sum, sizeof(int));
     cudaMemset(dev_sum, 0, sizeof(int));
     // remplissage de a 

     for(i=0; i<N*M; i++){
        a[i]= i;
     }

     // copie des tableaux a et b sur GPU

     cudaMemcpy(dev_a, a, N*M*sizeof(int), cudaMemcpyHostToDevice);

     // appel du kernal
     add<<<4,4>>>(dev_a, dev_sum);
     cudaMemcpy(sum, dev_sum, sizeof(int), cudaMemcpyDeviceToHost);
		
     // copie du tableau c du GPU vers CPU

     // affichage
     printf(" la somme des éléments de la matrice est %d \n", *sum);

     // libération mémoire sur Device

     cudaFree(dev_a);
     
     //libération mémoire sur Host

     free(a);
     free(sum);

     return 0;
}
