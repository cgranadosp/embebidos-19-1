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


// void * funHilo(void *arg){
// 	int nh = *(int*) arg;

// 	register int i = 0;
// 	int tamBloque = N/NUM_HILOS;
// 	int iniBloque = nh * tamBloque; 
// 	int finBloque = iniBloque + tamBloque;


// 	//Se agregó esto para crear una zona crítica y que los demás hilos no puedan acceder a ella mientras uno la esté ocupando
// 	pthread_mutex_lock(&bloqueo);

// 	printf("Iniciando hilo %d\n", nh);

// 	for (i = iniBloque; i<finBloque; i++)
// 	{
// 		C[i] = A[i] * B[i];
// 	}

// 	printf("Terminando hilo %d\n", nh);


// 	pthread_mutex_unlock(&bloqueo);

// 	pthread_exit(arg);
// }





void * RGBtoGray(void *arg)
{

	int nh = *(int*) arg;

	int tamBloque = height/NUM_HILOS;
	int iniBloque = nh * tamBloque; 
	int finBloque = iniBloque + tamBloque;

	register int x,y;
	int indiceRGB,indiceGray;
	unsigned char grayLevel;
	//unsigned char *imagenGray;
	//imagenGray=(unsigned char*) malloc(width*height*sizeof(unsigned char));
	//imagenGray=reservarMemoria(width,height);
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






void * filtroImagen(void *arg)
{
	int nh = *(int*) arg;
	register int x, y, xm, ym, indicem;
	int conv, indice;

	//Se utiliza una máscara de 3x3. Hay que convolucionar los valores de la imagen con los valores de la máscara
	//Utilizando el filtro Gaussiano
	unsigned char mascara[] = 
							{1,  4,  7,  4, 1,
	 						 4, 16, 26, 16, 4,
	 						 7, 26, 41, 26, 7,
	 						 4, 16, 26, 16, 4,
	 						 1,  4,  7,  4, 1,};

	int tamBloque =height/NUM_HILOS;
	int iniBloque = nh * tamBloque; 
	int finBloque = iniBloque + tamBloque;
	if(nh<=2)
	{
	for (y = iniBloque; y < finBloque; y++)
	{
		for(x = 0; x <= width-DIMASK; x++){ 
			//Para recorrer la máscara
			conv = 0;
			indicem = 0;

			for(ym = 0; ym < DIMASK; ym++){
				for(xm = 0; xm < DIMASK; xm++){
					indice = (y+ym)*width+(x+xm);
					conv += imagenGray[indice] * mascara[indicem++];
				}
			}

			conv = conv / 273; //Coeficiente dado por el filtro Gaussiano
			indice = ((y+1)*width + (x+1));
			imagenFiltro[indice] = conv;

		}
	}
	}
	else
	{
	for (y = iniBloque; y < finBloque-DIMASK; y++)
		{
		for(x = 0; x <= width-DIMASK; x++){ 
			//Para recorrer la máscara
			conv = 0;
			indicem = 0;

			for(ym = 0; ym < DIMASK; ym++){
				for(xm = 0; xm < DIMASK; xm++){
					indice = (y+ym)*width+(x+xm);
					conv += imagenGray[indice] * mascara[indicem++];
				}
			}

			conv = conv / 273; //Coeficiente dado por el filtro Gaussiano
			indice = ((y+1)*width + (x+1));
			imagenFiltro[indice] = conv;

		}
	}


	}
	pthread_exit(arg);




}

