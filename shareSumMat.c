#include <stdio.h>
#include <stdlib.h>

#define N 1024
#define M 1024 
#define threadsPerBlock N/4
__global__ void add (int *a, long *sv){
    __shared__ long cache[threadsPerBlock];
   int idx =threadIdx.x+blockIdx.x*blockDim.x;
   long tmp=0, i;    
   for (i=0; i< M; i++){
        tmp+=a[idx*M+i];
   }
   cache[threadIdx.x]=tmp;
   __syncthreads();
   
  /* if (threadIdx.x==0)
      for (i=0; i<blockDim.x; i++)
       sv[blockIdx.x]+=cache[i]; 
  */
    i= blockDim.x /2;
   while (i != 0){
       if(threadIdx.x<i)
       cache[threadIdx.x]+=cache[threadIdx.x + i];
       __syncthreads();
       i /= 2;
   }
   if (threadIdx.x==0)
       sv[blockIdx.x]+=cache[0];
   

}


int main (void) {
     int *a,i; 
     long sv[4];
     long *dev_sv;
     int *dev_a;
     long sum;
     //déclaration et création des événements de début et fin 
     cudaEvent_t start, stop;
     cudaEventCreate(&start); 
     cudaEventCreate(&stop);
  
     //allocaion de mémloire sur le Host

     a=(int *) malloc(N*M*sizeof(int));

     // remplissage de a   
     for(i=0; i<N*M; i++){
        a[i]= i;
     }
  
     //récupération de l'instant de départ
     cudaEventRecord(start, 0);
  
     // allocation de mémoire sur le GPU
     cudaMalloc((void **)&dev_a, N*M*sizeof(int));
     cudaMalloc((void **)&dev_sv, 4*sizeof(long));
     cudaMemset(dev_sv, 0, 4*sizeof(long));

     // copie du tableau a sur GPU

     cudaMemcpy(dev_a, a, N*M*sizeof(int), cudaMemcpyHostToDevice);

     // appel du kernal
     add<<<4,N/4>>>(dev_a, dev_sv);

     cudaMemcpy(sv, dev_sv, 4*sizeof(long), cudaMemcpyDeviceToHost);

     // récupération de l'instant de fin de calcul sur GPU
     cudaEventRecord(stop, 0);

     // synchronisation CPU/GPU
     cudaEventSynchronize(stop);
      
     //affichage du temps écoulé
     float elapsedTime;
     cudaEventElapsedTime(&elapsedTime, start, stop);
     
     sum=0;
     for(int i=0; i<4; i++){
         sum+=sv[i];
     }
     
     // affichage
     printf(" la somme des éléments de la matrice est %ld \n", sum);

     printf(" le temps écoulé dans le GPU %3.3f ms\n", elapsedTime);     

     //libération des événements 
     cudaEventDestroy(start);
     cudaEventDestroy(stop);

     // libération mémoire sur Device

     cudaFree(dev_a);
     cudaFree(dev_sv);
     
     //libération mémoire sur Host

     free(a);

     return 0;
}
