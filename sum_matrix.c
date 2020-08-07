#include <stdio.h>
#include <stdlib.h>

#define N 10 
#define M 8 
#define B 4
__global__ void add (int *a, int *sv){
    
   int idx =blockIdx.x, i;
   for (i=0; i< N*M/B; i++){
        sv[idx]+=a[idx*(N*M/B)+i];
   }
}


int main (void) {
     int *a, *sv;
     int *dev_sv, *dev_a;
     int sum, i;
     //allocaion de mémloire sur le Host

     a=(int *) malloc(N*M*sizeof(int));
     sv=(int *) malloc(N*sizeof(int));

     // allocation de mémoire sur le GPU
     cudaMalloc((void **)&dev_a, N*M*sizeof(int));
     cudaMalloc((void **)&dev_sv, N*sizeof(int));
     cudaMemset(dev_sv,0,N*sizeof(int));
     // remplissage de a 

     for(i=0; i<N*M; i++){
        a[i]= i;
     }

     // copie des tableaux a et b sur GPU

     cudaMemcpy(dev_a, a, N*M*sizeof(int), cudaMemcpyHostToDevice);

     // appel du kernal
     add<<<B,1>>>(dev_a, dev_sv);

     // copie du tableau c du GPU vers CPU

     cudaMemcpy(sv, dev_sv, N*sizeof(int), cudaMemcpyDeviceToHost);

     // affichage
     sum=0;
     for(int i=0; i<B; i++){
         printf ("%d  ", sv[i]);
         sum+=sv[i];
     }
     printf(" la somme des éléments de la matrice est %d \n", sum);

     // libération mémoire sur Device

     cudaFree(dev_a);
     cudaFree(dev_sv);
     
     //libération mémoire sur Host

     free(a);
     free(sv);

     return 0;
}
