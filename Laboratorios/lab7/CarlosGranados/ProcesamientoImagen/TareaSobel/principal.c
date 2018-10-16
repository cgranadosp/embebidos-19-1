#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#include "datos.h"
#include "hilo.h"
#include "defs.h"

unsigned char *imagenRGB,*imagenGray,*imagenFiltro;
uint32_t width;
uint32_t height;
int main(){

	int nhs[NUM_HILOS], *res;
	register int nh;
	pthread_t tids[NUM_HILOS];

	bmpInfoHeader info;
	

	imagenRGB = abrirBMP("calle1.bmp",&info);
	width=info.width;
	height=info.height;

	displayInfo( &info);
	//imagenGray=RGBtoGray(imagenRGB ,info.width,info.height);
//	brilloImagen(imagenGray,info.width,info.height);
	imagenFiltro=reservarMemoria(info.width,info.height);
	imagenGray=reservarMemoria(info.width,info.height);
//	filtroImagen(imagenGray,imagenFiltro,info.width,info.height);
	
//	GraytoRGB(imagenFiltro , imagenRGB , info.width , info.height );
	


	for (nh = 0; nh < NUM_HILOS; nh++)
	{
		nhs[nh] = nh;
		//pthread_create(&tids[nh], NULL, funHilo, (void *)&nhs[nh]);
		pthread_create(&tids[nh], NULL, RGBtoGray, (void *)&nhs[nh]);
	}

	for (nh = 0; nh < NUM_HILOS; nh++)
	{
		pthread_join(tids[nh], (void **)&res);
		printf("Hilo %d terminado\n", *res);
	}


	for (nh = 0; nh < NUM_HILOS; nh++)
	{
		nhs[nh] = nh;
		//pthread_create(&tids[nh], NULL, funHilo, (void *)&nhs[nh]);
		pthread_create(&tids[nh], NULL, filtroImagen, (void *)&nhs[nh]);
	}

	for (nh = 0; nh < NUM_HILOS; nh++)
	{
		pthread_join(tids[nh], (void **)&res);
		printf("Hilo %d terminado\n", *res);
	}


	for (nh = 0; nh < NUM_HILOS; nh++)
	{
		nhs[nh] = nh;
		//pthread_create(&tids[nh], NULL, funHilo, (void *)&nhs[nh]);
		pthread_create(&tids[nh], NULL, GraytoRGB, (void *)&nhs[nh]);
	}

	for (nh = 0; nh < NUM_HILOS; nh++)
	{
		pthread_join(tids[nh], (void **)&res);
		printf("Hilo %d terminado\n", *res);
	}

	guardarBMP("calleFS.bmp", &info , imagenRGB);

	free(imagenRGB);
	free(imagenGray); 
	free(imagenFiltro);

	return 0;
}

