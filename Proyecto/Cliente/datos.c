#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <math.h>
#include <unistd.h>
#include <sys/wait.h>

#include "imagen.h"

unsigned char * reservarMemoria(uint32_t width,uint32_t height)
{
	unsigned char *imagenGray;
	imagenGray=(unsigned char*) malloc(width*height*sizeof(unsigned char));
	if(imagenGray == NULL)
	{
		perror("Error al asignar memoria");
		exit(EXIT_FAILURE);
	}
	return imagenGray;

}
