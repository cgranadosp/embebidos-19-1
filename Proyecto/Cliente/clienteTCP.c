#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

#include "datos.h"
#include "hilo.h"
#include "defs.h"

#define DIR_IP "10.100.74.163"
#define PUERTO 5000
#define TAM_BUFFER 250

int enviar(int sockfd, void *buf, int len);

unsigned char *imagenRGB,*imagenGray,*imagenFiltro;
uint32_t width;
uint32_t height;

int main(int argc, char **argv)
{
	///INICIO IMAGEN
	int nhs[NUM_HILOS], *res;
	register int nh;
	pthread_t tids[NUM_HILOS];

	bmpInfoHeader info;

	imagenRGB = abrirBMP("calle1.bmp",&info);
	width=info.width;
	height=info.height;

	displayInfo( &info);

	imagenGray=reservarMemoria(info.width,info.height);

	for (nh = 0; nh < NUM_HILOS; nh++)
	{
		nhs[nh] = nh;
		pthread_create(&tids[nh], NULL, RGBtoGray, (void *)&nhs[nh]);
	}

	for (nh = 0; nh < NUM_HILOS; nh++)
	{
		pthread_join(tids[nh], (void **)&res);
		printf("Hilo %d terminado\n", *res);
	}
	///FIN IMAGEN

	//int tamano_direccion, sockfd;
	int sockfd;
	struct sockaddr_in direccion_servidor;
	char leer_mensaje[TAM_BUFFER];
/*	
 *	AF_INET - Protocolo de internet IPV4
 *  htons - El ordenamiento de bytes en la red es siempre big-endian, por lo que
 *  en arquitecturas little-endian se deben revertir los bytes
 */	
	memset (&direccion_servidor, 0, sizeof (direccion_servidor));
	direccion_servidor.sin_family = AF_INET;
	direccion_servidor.sin_port = htons(PUERTO);
/*	
 *	inet_pton - Convierte direcciones de texto IPv4 en forma binaria
 */	
	if( inet_pton(AF_INET, DIR_IP, &direccion_servidor.sin_addr) <= 0 )
	{
		perror("Ocurrio un error al momento de asignar la direccion IP");
		exit(1);
	}
/*
 *	Creacion de las estructuras necesarias para el manejo de un socket
 *  SOCK_STREAM - Protocolo orientado a conexión
 */
	printf("Creando Socket ....\n");
	if( (sockfd = socket (AF_INET, SOCK_STREAM, 0)) < 0 )
	{
		perror("Ocurrio un problema en la creacion del socket");
		exit(1);
	}
/*
 *	Inicia el establecimiento de una conexion mediante una apertura
 *	activa con el servidor
 *  connect - ES BLOQUEANTE
 */
	printf ("Estableciendo conexion ...\n");
	if( connect(sockfd, (struct sockaddr *)&direccion_servidor, sizeof(direccion_servidor) ) < 0) 
	{
		perror ("Ocurrio un problema al establecer la conexion");
		exit(1);
	}
/*
 *	Inicia la transferencia de datos entre cliente y servidor
 */


	///ENVIAR imagenGray

	int len = info.width*info.height*sizeof(unsigned char);
	printf ("Size: %d \n", len);
	printf ("imagen: %c \n", *imagenGray);


	printf ("Enviando el tamaño del mensaje al servidor ... %d \n", len);
	sleep(2);

	int i = 0;
	if( write(sockfd, &len, sizeof(int)) < 0 )
	{
			perror("Ocurrio un problema en el envio de un mensaje al cliente");
			exit(1);
	}

	while(len>0){
		printf ("Enviando mensaje al servidor ...\n");
		if( write(sockfd, &imagenGray[i], sizeof(unsigned char)) < 0 )
		{
			perror("Ocurrio un problema en el envio de un mensaje al cliente");
			exit(1);
		}
		i++;
		len--;
	}
	printf("---Se enviaron %d \n", i);

	printf ("Recibiendo contestacion del servidor ...\n");
	if (read (sockfd, &leer_mensaje, TAM_BUFFER) < 0)
	{	
		perror ("Ocurrio algun problema al recibir datos del servidor");
		exit(1);
	}
	
	printf ("El servidor envio el siguiente mensaje: \n%s\n", leer_mensaje);
	printf ("Cerrando la aplicacion cliente\n");
/*
 *	Cierre de la conexion
 */
	close(sockfd);

	return 0;
}



int enviar(int sockfd, void *buf, int len){
	int total = 0; //bytes enviados
	int bytesleft = len; //bytes por enviar
	int n;

	printf("-Tamaño: %d \n", len);

	while(total < len){
		//send(int sockfd, const void *buf, size_t len, int flags);
		//n =  send(sockfd, buf+total, bytesleft, 0);
		//n =  write(sockfd, "Soy el Cliente", 15);
		n =  write(sockfd, buf+total, bytesleft);
		if(n==-1){
			break;
		}
		total += n;
		bytesleft -=n;

		printf("-Enviando: %d \n", total);
	}

	//*len = total; //Se guarda el número de bytes enviados
	
	return n==-1?-1:0; // 0 si se envió correctamente
}
