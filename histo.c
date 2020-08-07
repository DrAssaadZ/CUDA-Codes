#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>


#define SIZE (100*1024*1024)

unsigned char *  big_random_block (int size) {
   int i;
   unsigned char * buffer = (unsigned char *) malloc (size*sizeof(unsigned char));
   for(i=0; i< size; i++) {
       buffer[i]=rand()%256;
      // printf("%d ", buffer[i]);
   }
   return buffer;
}


__global__ void histo_kernel( unsigned char *buffer, long size, unsigned int *histo ) {
    int i = threadIdx.x + blockIdx.x * blockDim.x;
    int stride = blockDim.x * gridDim.x;
    while (i < size) {
         atomicAdd( &(histo[buffer[i]]), 1 );
         i += stride ;
    }
}


int main( void ) {

    srand(time(NULL));

    unsigned char *buffer = (unsigned char*) big_random_block( SIZE );
    
    cudaEvent_t start, stop;
    cudaEventCreate( &start );
    cudaEventCreate( &stop );
   // cudaEventRecord( start, 0 );

   // allocate memory on the GPU for the file's data
    unsigned char *dev_buffer;
    unsigned int *dev_histo;
    
    cudaMalloc( (void**)&dev_buffer, SIZE );
    cudaMemcpy( dev_buffer, buffer, SIZE, cudaMemcpyHostToDevice );
    cudaMalloc( (void**)&dev_histo, 256 * sizeof( long ));
    cudaMemset( dev_histo, 0, 256 * sizeof( int ));
    
    cudaDeviceProp prop;
    cudaGetDeviceProperties( &prop, 0 );
    int blocks = prop.multiProcessorCount;
    
   cudaEventRecord( start, 0 );

    histo_kernel<<<blocks*2,256>>>( dev_buffer, SIZE, dev_histo );

 cudaEventRecord( stop, 0 );
    cudaEventSynchronize( stop );
    
    unsigned int histo[256];
    cudaMemcpy( histo, dev_histo,256*sizeof( int ),cudaMemcpyDeviceToHost);
    
    // get stop time, and display the timing results
   // cudaEventRecord( stop, 0 );
   // cudaEventSynchronize( stop );
    float elapsedTime;
    cudaEventElapsedTime( &elapsedTime, start, stop );
    printf( "Time to generate: %3.1f ms\n", elapsedTime );
    
    long histoCount = 0 ;
    for (int i=0; i<256; i++)
        histoCount += histo[i];
    printf( "Histogram Sum: %ld\n", histoCount );

   // verify that we have the same counts via CPU

    for (int i=0; i<SIZE; i++)
        histo[buffer[i]]--;

    for (int i=0; i<256; i++) {
       if (histo[i] != 0) 
          printf( "Failure at %d!\n", i );
    }

    cudaEventDestroy( start );
    cudaEventDestroy( stop );
    cudaFree( dev_histo ); 
    cudaFree( dev_buffer );
    free( buffer );

    return 0;
}
