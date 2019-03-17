#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PUERTO 5000
#define TAM_BUFFER 100
#define DIR_IP "192.168.43.95"

int main(int argc, char **argv)
{
	int tamano_direccion, sockfd;
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
		perror("Ocurrió un error al momento de asignar la direccion IP");
		exit(1);
	}
/*
 *	Creacion de las estructuras necesarias para el manejo de un socket
 *  SOCK_STREAM - Protocolo orientado a conexión
 */
	printf("Creando Socket ....\n");
	if( (sockfd = socket (AF_INET, SOCK_STREAM, 0)) < 0 )
	{
		perror("Ocurrió un problema en la creación del socket");
		exit(1);
	}
/*
 *	Inicia el establecimiento de una conexion mediante una apertura
 *	activa con el servidor
 *  connect - ES BLOQUEANTE
 */
	printf ("Estableciendo conexión ...\n");
	if( connect(sockfd, (struct sockaddr *)&direccion_servidor, sizeof(direccion_servidor) ) < 0) 
	{
		perror ("Ocurrió un problema al establecer la conexión");
		exit(1);
	}
/*
 *	Inicia la transferencia de datos entre cliente y servidor
 */
	printf ("Enviando mensaje al servidor ...\n");
	char msg[] = "Solicitando posición";
	if( write(sockfd, msg, strlen(msg)) < 0 )
	{
		perror("Ocurrió un problema en el envío de un mensaje al cliente");
		exit(1);
	}

	printf ("Recibiendo contestacion del servidor ...\n");
	if (read (sockfd, &leer_mensaje, 38) < 0)
	{	
		perror ("Ocurrió algún problema al recibir datos del cliente");
		exit(1);
	}

	char param[21];

	if(strstr(leer_mensaje, "señal")==NULL){
	
		if (read (sockfd, &param, 21) < 0)
		{	
			perror ("Ocurrió algún problema al recibir datos del cliente2");
			exit(1);
		}
		
		printf ("-> POSICIÓN: \n%s\n----------------------------------------\n%s\n", leer_mensaje, param);
	}else{
		printf ("-> MENSAJE: \n%s\n----------------------------------------\n", leer_mensaje);
	}
	printf ("Cerrando la aplicación cliente\n");
/*
 *	Cierre de la conexion
 */
	close(sockfd);

	if(strstr(param, ",")){
		char URL[] = "google-chrome https://www.google.com/maps/search/";
		strcat(URL, param);
		
		//system("google-chrome https://www.google.com/maps/search/19.35829,-99.08329");
		system(URL);
	}

	return 0;
}
	
