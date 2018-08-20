#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#define MUESTRAS 512

void genera_seno (float seno[]);
void guarda_datos (float datos[]);

int main(){
    float seno[MUESTRAS];

    genera_seno(seno);
    guarda_datos(seno);

    return 0;
}

void genera_seno(float seno[]){
    float f = 1000, fs = 45000;
    register int n; //register es un especificador de acceso, permiten acceder a los datos de una manera muy específica [register/static/extern]

    for(n=0; n<MUESTRAS; n++){
        seno[n] = sinf(2*n*M_PI*f/fs);
    }
}

void guarda_datos(float datos[]){
    FILE *ap_arch;
    register int n;

    ap_arch = fopen("seno.dat", "w");
    
    if(!ap_arch){
        perror("Error al abrir el archivo");
	exit(EXIT_FAILURE);
    }

    for(n=0; n<MUESTRAS; n++){
	fprintf(ap_arch, "%f \n", datos[n]);
    }

    fclose(ap_arch);
}
