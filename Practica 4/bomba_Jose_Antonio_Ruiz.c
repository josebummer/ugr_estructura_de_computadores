/*
Programa Bomba de : Jose Antonio Ruiz Millan
Compilado con -O0.
*/

#include <stdio.h>	// para printf()
#include <stdlib.h>	// para exit()
#include <string.h>	// para strncmp()/strlen()
#include <sys/time.h>	// para gettimeofday(), struct timeval

char password[]="krodvrbmrvh\n";  //holasoyjose
int  passcode  = 1234;
void encripta(char *cadena){
	for( int i = 0; i < strlen(cadena);i++ ){
  	for( int j = 0; j < 3; j++ ){
			if(cadena[i] == 'z'){
				cadena[i] = 'a';
			}
			else{
				cadena[i]+=1;
			}
		}
  }
}

void boom(){
	printf("**********************\n");
	printf("*** SIGUE BUSCANDO ***\n");
	printf("**********************\n");
	exit(-1);
}

void defused(){
	printf("*************************\n");
	printf("*** OOLEEEEEEEEEEEEEE ***\n");
	printf("*************************\n");
	exit(0);
}

int main(){
#define SIZE 100
	char pass[SIZE];
	int  pasv;
#define TLIM 60

	struct timeval tv1,tv2;	// gettimeofday() secs-usecs

	gettimeofday(&tv1,NULL);

	printf("Introduce la contraseña: ");
	fgets(pass,SIZE,stdin);
	encripta(pass);
	if (strncmp(pass,password,strlen(password)))
	    boom();

	gettimeofday(&tv2,NULL);
	if (tv2.tv_sec - tv1.tv_sec > TLIM)
	    boom();

	printf("Introduce el código: ");
	scanf("%i",&pasv);
	if (pasv!=passcode) boom();

	gettimeofday(&tv1,NULL);
	if (tv1.tv_sec - tv2.tv_sec > TLIM)
	    boom();

	defused();
}
