//  según la versión de gcc y opciones de optimización usadas, tal vez haga falta
//  usar gcc –fno-omit-frame-pointer si gcc quitara el marco pila (%ebp)

#include <stdint.h>
#include <string.h>
#include <strings.h>
#include <stdio.h>	// para printf()
#include <stdlib.h>	// para exit()
#include <sys/time.h>	// para gettimeofday(), struct timeval

#define SIZE (1<<22)

int lista[SIZE];
int resultado=0;

long crono(int (*func)(), char* msg){
    struct timeval tv1,tv2;	// gettimeofday() secs-usecs
    long           tv_usecs;	// y sus cuentas

    gettimeofday(&tv1,NULL);
    resultado = func(lista, SIZE);
    gettimeofday(&tv2,NULL);

    tv_usecs=(tv2.tv_sec -tv1.tv_sec )*1E6+
             (tv2.tv_usec-tv1.tv_usec);
    printf("resultado = %d\t", resultado);
    printf("%s:%9ld us\n", msg, tv_usecs);
}

int popcount_v2( unsigned* array , int len ){
  int result = 0;
  int i = 0;
  unsigned x;
  while( i < len ){
    x = array[i];
    while( x ){
      result += x & 0x1;
      x >>= 1;
    }
    i++;
  }
  return result;
}
int popcount_v1( unsigned* array , int len ){
  int result = 0;
  unsigned x;
  for( int i = 0; i < len; i++ ){
    x = array[i];
    for( int j = 0; j < 8*sizeof(int); j++ ){
      result += x & 0x1;
      x >>= 1;
    }
  }
  return result;
}
int popcount_v3(unsigned* array, int len){
  int result = 0;
  unsigned x;
  for( int i = 0; i < len; i++ ){
    x = array[i];
    asm("\n"
    "ini3:                   \n\t"
      "shr %[x]              \n\t"
      "adc $0 , %[r]         \n\t"
      "cmp $0 , %[x]         \n\t"
      "jne ini3              \n\t"
      : [r] "+r" (result)
      : [x] "r" (x)           );
  }
  return result;
}
int popcount_v4(unsigned* array , int len){
  int val;
  int res = 0;
  unsigned x;
  for( int i = 0; i < len; i++){
    x = array[i];
    val = 0;
    for (int j = 0; j < 8; j++) {
      val += x & 0x01010101;
      x >>= 1;
    }
    val += (val >> 16);
    val += (val >> 8);
    res += val & 0xFF;
  }

  return res;
}

int popcount_v5(unsigned* array, int len) {
  int val, result = 0;
  int SSE_mask[] = { 0x0f0f0f0f, 0x0f0f0f0f, 0x0f0f0f0f, 0x0f0f0f0f };
  int SSE_LUTb[] = { 0x02010100, 0x03020201, 0x03020201, 0x04030302 };

  if (len & 0x3)
    printf("leyendo 128b pero len no múltiplo de 4?n");
  for (int i = 0; i < len; i += 4) {
    asm("movdqu        %[x], %%xmm0 \n\t"
        "movdqa  %%xmm0, %%xmm1 \n\t" // dos copias de x
        "movdqu    %[m], %%xmm6 \n\t"// máscara
        "psrlw           $4, %%xmm1 \n\t"
        "pand    %%xmm6, %%xmm0 \n\t"//; xmm0 – nibbles inferiores
        "pand    %%xmm6, %%xmm1 \n\t"//; xmm1 – nibbles superiores

        "movdqu    %[l], %%xmm2 \n\t"//; ...como pshufb sobrescribe LUT
        "movdqa  %%xmm2, %%xmm3 \n\t"//; ...queremos 2 copias
        "pshufb  %%xmm0, %%xmm2 \n\t"//; xmm2 = vector popcount inferiores
        "pshufb  %%xmm1, %%xmm3 \n\t"//; xmm3 = vector popcount superiores

        "paddb   %%xmm2, %%xmm3 \n\t"//; xmm3 - vector popcount bytes
        "pxor    %%xmm0, %%xmm0 \n\t"//; xmm0 = 0,0,0,0
        "psadbw  %%xmm0, %%xmm3 \n\t"//;xmm3 = [pcnt bytes0..7|pcnt bytes8..15]
        "movhlps %%xmm3, %%xmm0 \n\t"//;xmm3 = [             0           |pcnt bytes0..7 ]
        "paddd   %%xmm3, %%xmm0 \n\t"//;xmm0 = [ no usado        |pcnt bytes0..15]
        "movd    %%xmm0, %[val] \n\t"
        : [val]"=r" (val)
        : [x] "m" (array[i]),
        [m] "m" (SSE_mask[0]),
        [l] "m" (SSE_LUTb[0])
    );
    result += val;
  }
  return result;
}

int popcount_v6(unsigned* array, int len) {
  unsigned x;
  int val, result = 0;
  for (int i = 0; i < len; i++) {
    x = array[i];
    asm("popcnt %[x], %[val]        \n\t"
        : [val]"=r"(val)
        : [x] "r" (x)
    );
    result += val;
  }
  return result;
}

int popcount_v7(unsigned* array, int len) {
  unsigned x1, x2;
  int val, result = 0;
  if (len & 0x1)
    printf("Leer 64b y len impar?n");
  for (int i = 0; i < len; i += 2) {
    x1 = array[i];
    x2 = array[i + 1];
    asm("popcnt %[x1], %[val]       \n\t"
        "popcnt %[x2], %%edi    \n\t"
        "add    %%edi, %[val]   \n\t"
        : [val]"=r"(val)
        : [x1] "r" (x1),
        [x2] "r" (x2)
        : "edi"
    );
    result += val;
  }
  return result;
}

int main()
{
    int i,k;			// inicializar array
    for (i=0; i<SIZE; i++)	// se queda en cache
	   lista[i]=i;

     crono( popcount_v1, "popcount ( con for          )" );
     crono( popcount_v2, "popcount ( con while        )" );
     crono( popcount_v3, "popcount ( con while & asm  )" );
     crono( popcount_v4, "popcount ( version libro    )" );
     crono( popcount_v5, "popcount ( version SSE3     )" );
     crono( popcount_v6, "popcount ( version SSE4     )" );
     crono( popcount_v7, "popcount ( 64b mejorar      )" );

    exit(0);
}
