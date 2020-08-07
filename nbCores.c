#include <stdio.h>
#include <stdlib.h>

#define N 10



int getSPcores(cudaDeviceProp devProp)
{  
    int cores = 0;
    int mp = devProp.multiProcessorCount;
    switch (devProp.major){
     case 2: // Fermi
      if (devProp.minor == 1) cores = mp * 48;
      else cores = mp * 32;
      break;
     case 3: // Kepler
      cores = mp * 192;
      break;
     case 5: // Maxwell
      cores = mp * 128;
      break;
     case 6: // Pascal
      if (devProp.minor == 1) cores = mp * 128;
      else if (devProp.minor == 0) cores = mp * 64;
      else printf("Unknown device type\n");
      break;
     default:
      printf("Unknown device type\n"); 
      break;
      }
    return cores;
}

int main (void) {
int count;
cudaDeviceProp prop;

cudaGetDeviceCount(&count);
printf("\n Il y a %d GPU(s) compatible CUDA \n", count);
for(int i=0; i< count; i++) {
   cudaGetDeviceProperties(&prop, i);
   printf("\n Il y a %d CUDA cores dans le device numero %d \n\n", getSPcores(prop), i);

}

return 0;
}
