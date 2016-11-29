//  según la versión de gcc y opciones de optimización usadas, tal vez haga falta
//  usar gcc –fno-omit-frame-pointer si gcc quitara el marco pila (%ebp)

#include <stdio.h>	// para printf()
#include <stdlib.h>	// para exit()
#include <sys/time.h>	// para gettimeofday(), struct timeval

#define SIZE (1<<22)	// tamaño suficiente para tiempo apreciable
#define WSIZE 8*sizeof(int)
unsigned lista[SIZE];
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

int paridad_v1(unsigned* array, int len) {
  int paridad;
  unsigned x;
  int result = 0;

  for (int i = 0; i < len; i++) {
    paridad = 0;
    x = array[i];
    for (int j = 0; j < WSIZE; j++) {
      paridad ^= (x & 1);
      x >>= 1;
    }
    result += paridad & 0x01;
  }
 return result;
}
int paridad_v2(unsigned* array, int len) {
  int paridad;
  unsigned entero;
  int result = 0;

  for (int i = 0; i < len; i++) {
    paridad = 0;
    entero = array[i];
    while (entero) {
      paridad ^= (entero & 1);
      entero >>= 1;
     }
     result += paridad & 0x1;
   }
 return result;
}
int paridad_v3(unsigned* array, int len) {
  int val = 0;
  unsigned x;
  int result = 0;
  for (int i = 0; i < len; i++) {
    x = array[i];
    while (x) {
      val ^= x;
      x >>= 1;
    }
    result += val & 0x1;
  }
  return result;
}
int paridad_v4(unsigned* array, int len) {
  int val;
  unsigned x;
  int result = 0;
  for (int i = 0; i < len; i++) {
    x = array[i];
    val = 0;
    asm(
      "ini3:                  \n\t"
      "xor %[x], %[v]         \n\t"
      "shr $1, %[x]           \n\t"
      "cmp $0 , %[x]          \n\t"
      "jne ini3               \n\t"
      : [v]"+r"(val) // e/s: inicialemnte 0, salida valor final
      : [x]"r"(x)// entrada: valor del elemento
     );
     result += val & 0x1;
   }
   return result;
}
int paridad_v5(unsigned* array, int len) {
  int k;
  int result = 0;
  unsigned x;
  for (int i = 0; i < len; i++) {
    x = array[i];
    for (k = 16; k >= 1; k /= 2)
      x ^= x >> k;
      result += (x & 0x1);
  }
  return result;
}
int paridad_v6(unsigned* array, int len) {
  unsigned x = 0;
  int resultado = 0;

  for (int i = 0; i < len; i++) {
    x = array[i];
    asm(
      "mov    %[x],   %%edx       \n\t"
      "shr    $16,    %%edx       \n\t"
      "xor    %[x],   %%edx       \n\t"
      "xor    %%dh,   %%dl        \n\t"
      "setpo  %%dl                \n\t"
      "movzx  %%dl,   %[x]        \n\t"
      : [x] "+r" (x) // input
      :
      : "edx"//Clobber
      );
        resultado += x;
  }
 return resultado;
}

int main()
{
    int i;			// inicializar array
    for (i=0; i<SIZE; i++)	// se queda en cache
	 lista[i]=i;

    crono(paridad_v1, "calcular paridad (bucle for          )");
    crono(paridad_v2, "calcular paridad (bucle while        )");
    crono(paridad_v3, "calcular paridad (version libro      )");
    crono(paridad_v4, "calcular paridad (C y asm            )");
    crono(paridad_v5, "calcular paridad (suma en arbol      )");
    crono(paridad_v6, "calcular paridad (C y asm mejorado   )");

    exit(0);
}
