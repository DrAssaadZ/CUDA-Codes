#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N 1024*1000
#define nT 1024
#define nB 1000
__global__ void add (int *a, long *occ, int x){
    __shared__ long cache[nT];
   int idx = threadIdx.x+blockIdx.x*blockDim.x;
   int stride = blockDim.x*gridDim.x; 
   long nbOcc=0, i; 
   while (idx<N){
        if(a[idx]==x)
            nbOcc++;
        idx+=stride; 
   }
   cache[threadIdx.x]=nbOcc;
   __syncthreads();
   
    i= blockDim.x /2;
   while (i != 0){
       if(threadIdx.x<i)
       cache[threadIdx.x]+=cache[threadIdx.x + i];
       __syncthreads();
       i /= 2;
   }
   if (threadIdx.x==0)
       occ[blockIdx.x]+=cache[0];
}


int main (void) {
     int *a,i; 
     long occ[nB];
     long *dev_occ;
     int *dev_a;
     long sum;
     srand(time(NULL));
     //déclaration et création des événements de début et fin 
     cudaEvent_t start, stop;
     cudaEventCreate(&start); 
     cudaEventCreate(&stop);
  
     //allocaion de mémloire sur le Host

     a=(int *) malloc(N*sizeof(int));
     int x;

     // remplissage de a   
     for(i=0; i<N; i++){
        a[i]= rand()%8;
       // printf("%d ",a[i]);
     }
     printf("\n Donner la valeur de x \n");
     scanf("%d", &x);
     //récupération de l'instant de départ
     cudaEventRecord(start, 0);
  
     // allocation de mémoire sur le GPU
     cudaMalloc((void **)&dev_a, N*sizeof(int));
     cudaMalloc((void **)&dev_occ, nB*sizeof(long));
     cudaMemset(dev_occ, 0, nB*sizeof(long));

     // copie du tableau a sur GPU

     cudaMemcpy(dev_a, a, N*sizeof(int), cudaMemcpyHostToDevice);

     // appel du kernal
     add<<<nB,nT>>>(dev_a, dev_occ, x);

     cudaMemcpy(occ, dev_occ, nB*sizeof(long), cudaMemcpyDeviceToHost);

     // récupération de l'instant de fin de calcul sur GPU
     cudaEventRecord(stop, 0);

     // synchronisation CPU/GPU
     cudaEventSynchronize(stop);
      
     //affichage du temps écoulé
     float elapsedTime;
     cudaEventElapsedTime(&elapsedTime, start, stop);
     
     sum=0;
     for(int i=0; i<nB; i++){
         sum+=occ[i];
     }
     
     // affichage
     printf(" le nombre d'occurences de %d est %ld \n", x, sum);

     printf(" le temps écoulé dans le GPU %3.3f ms\n", elapsedTime);     

     //libération des événements 
     cudaEventDestroy(start);
     cudaEventDestroy(stop);

     // libération mémoire sur Device

     cudaFree(dev_a);
     cudaFree(dev_occ);
     
     //libération mémoire sur Host

     free(a);

     return 0;
}
