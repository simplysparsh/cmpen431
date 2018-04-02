#include "YOURCODEHERE.h"

unsigned int log_base_two(unsigned int n) {
  if (n==0) fprintf(stderr,"ERROR: Log of 0 not allowed!\n");
  unsigned int logValue = -1;
  while (n) {
    logValue++;
    n >>= 1;
  }
  return logValue;
}

void  setSizesOffsetsAndMaskFields(cache* acache, unsigned int size, unsigned int assoc, unsigned int blocksize){
  unsigned int localVAbits=8*sizeof(uint64_t*);
  if (localVAbits!=64){
    fprintf(stderr,"Running non-portable code on unsupported platform, terminating. Please use designated machines.\n");
    exit(-1);
  }

  // YOUR CODE GOES HERE
  unsigned int numOfBlocks = size/blocksize;
  unsigned int indexSize = log_base_two(numOfBlocks/assoc);

  acache->numways = assoc;
  acache->blocksize = blocksize;
  acache->numsets = numOfBlocks/assoc;
  acache->BO = log_base_two(blocksize);
  acache->TO = acache->BO + indexSize;

  unsigned int n = 0;
  acache->VAImask = 0;
  n = acache->numsets;
  while(n>>=1) ++acache->VAImask;
  acache->VAImask = (1<<acache->VAImask)-1;
  acache->VATmask = ~(0);

}


unsigned long long getindex(cache* acache, unsigned long long address){
  for (int i = 0; i < acache->BO; i++){
    address = address >> 1;
  }

  return address & acache->VAImask;
}

unsigned long long gettag(cache* acache, unsigned long long address){
  for (int i = 0; i < acache->TO; i++){
    address = address >> 1;
  }

  return address & acache->VATmask;

}

void writeback(cache* acache, unsigned int index, unsigned int oldestway){
  // YOUR CODE GOES HERE
  unsigned long long address = 0;
  unsigned long long tag = 0;
  const int word_size = 8;
  tag = acache->sets[index].blocks[oldestway].tag;
  address = (tag << acache->TO) + (index << acache->BO);


  for(int i = 0; i < (acache->blocksize/word_size); i++){
    performaccess(acache->nextcache, (word_size*i+address), 8, 1, acache->sets[index].blocks[oldestway].datawords[i], i);
  }
}

void fill(cache * acache, unsigned int index, unsigned int oldestway, unsigned long long address){
   //YOUR CODE GOES HERE
  unsigned long long base_address = (address/acache->blocksize)*acache->blocksize;
  unsigned long long value = 0;
  const int word_size = 8;
  for(int i = 0; i < (acache->blocksize/word_size); i++){
    value = performaccess(acache->nextcache, (word_size*i+base_address), 8, 0, 0, i);
    acache->sets[index].blocks[oldestway].datawords[i]= value;
  }
}
