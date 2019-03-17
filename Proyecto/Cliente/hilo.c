#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>

#include "imagen.h"
#include "defs.h"
#include "datos.h"

extern unsigned char *imagenRGB,*imagenGray,*imagenFiltro;
extern uint32_t width;
extern uint32_t height;

void * RGBtoGray(void *arg)
{

	int nh = *(int*) arg;

	int tamBloque = height/NUM_HILOS;
	int iniBloque = nh * tamBloque; 
	int finBloque = iniBloque + tamBloque;

	register int x,y;
	int indiceRGB,indiceGray;
	unsigned char grayLevel;

	if(imagenGray == NULL)
	{
		perror("Error al asignar memoria");
		exit(EXIT_FAILURE);
	}

	for (y = iniBloque; y < finBloque; y++)
	{
		for (x = 0; x < width; x++)
		{

			indiceGray = (y*width +x);
			//indiceRGB  = indiceGray * 3;
			indiceRGB  =  ((indiceGray << 1) + indiceGray);
			grayLevel  = (30*imagenRGB[indiceRGB] + 59*imagenRGB[indiceRGB+1] + 11*imagenRGB[indiceRGB+2])/100;
			imagenGray[indiceGray] =grayLevel;
		}

	}

	pthread_exit(arg);
}


void * GraytoRGB(void *arg)
{
	register int x,y;
	int indiceRGB,indiceGray;

	int nh = *(int*) arg;
	int tamBloque = height/NUM_HILOS;
	int iniBloque = nh * tamBloque; 
	int finBloque = iniBloque + tamBloque;
	
	for (y = iniBloque; y < finBloque; y++)
	{
		for (x = 0; x < width; x++)
		{
			indiceGray = (y*width +x);
			indiceRGB  = indiceGray * 3;
			imagenRGB[indiceRGB+0] = imagenFiltro[indiceGray];
			imagenRGB[indiceRGB+1] = imagenFiltro[indiceGray];
			imagenRGB[indiceRGB+2] = imagenFiltro[indiceGray];
		}

	}

	pthread_exit(arg);
}
