#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <pthread.h>

#define PUERTO 			5000	//Número de puerto asignado al servidor
#define COLA_CLIENTES 	5 		//Tamaño de la cola de espera para clientes
#define TAM_BUFFER 		1000000
#define NUM_HILOS 4
#define DIMASK 3

void ISRsw(int sig);
int recibir(int cliente_sockfd, void *buf, int len);

uint32_t width;
uint32_t height;


int main(int argc, char **argv)
{
	int nhs[NUM_HILOS], *res;
	register int nh;
	pthread_t tids[NUM_HILOS];


   	int sockfd, cliente_sockfd;
	//socklen_t sin_size;
   	struct sockaddr_in direccion_servidor; //Estructura de la familia AF_INET, que almacena direccion

	pid_t pid;

/*	
 *	AF_INET - Protocolo de internet IPV4
 *  htons - El ordenamiento de bytes en la red es siempre big-endian, por lo que
 *  en arquitecturas little-endian se deben revertir los bytes
 *  INADDR_ANY - Se elige cualquier interfaz de entrada
 */	
   	memset (&direccion_servidor, 0, sizeof (direccion_servidor));	//se limpia la estructura con ceros
   	direccion_servidor.sin_family 		= AF_INET;
   	direccion_servidor.sin_port 		= htons(PUERTO);
   	direccion_servidor.sin_addr.s_addr 	= INADDR_ANY;

	if(signal(SIGUSR1, ISRsw) == SIG_ERR){
		perror("Error en la ISR\n");
		exit(EXIT_FAILURE);
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
 *  bind - Se utiliza para unir un socket con una dirección de red determinada
 */
   	printf("Configurando socket ...\n");
   	if( bind(sockfd, (struct sockaddr *) &direccion_servidor, sizeof(direccion_servidor)) < 0 )
	{
		perror ("Ocurrio un problema al configurar el socket");
		exit(1);
   	}
/*
 *  listen - Marca al socket indicado por sockfd como un socket pasivo, esto es, como un socket
 *  que será usado para aceptar solicitudes de conexiones de entrada usando "accept"
 *  Habilita una cola asociada la socket para alojar peticiones de conector procedentes
 *  de los procesos clientes
 */
   	printf ("Estableciendo la aceptacion de clientes...\n");
	if( listen(sockfd, COLA_CLIENTES) < 0 )
	{
		perror("Ocurrio un problema al crear la cola de aceptar peticiones de los clientes");
		exit(1);
   	}
/*
 *  accept - Aceptación de una conexión
 *  Selecciona un cliente de la cola de conexiones establecidas
 *  se crea un nuevo descriptor de socket para el manejo
 *  de la nueva conexion. Apartir de este punto se debe
 *  utilizar el nuevo descriptor de socket
 *  accept - ES BLOQUEANTE 
 */
	while(1){
		

		printf ("---En espera de peticiones de conexión ...\n");
		//if( (cliente_sockfd = accept(sockfd, (struct sockaddr *)&direccion_servidor, &sin_size)) < 0 )
		if( (cliente_sockfd = accept(sockfd, NULL, NULL)) < 0 ) //Cada que termina un cliente vuelve a esperar en accept la petición de otro cliente
		{
			perror("Ocurrio algun problema al atender a un cliente");
			exit(1);
		}

		//Al aceptar la solicitud del cliente se crea un proceso para atender esa petición
		pid = fork();
		if(!pid){ //Si es el proceso hijo continúa...

		int nhs[NUM_HILOS], *res;
		register int nh;
		pthread_t tids[NUM_HILOS];

		/*
		*	Inicia la transferencia de datos entre cliente y servidor
		*/

		//RECIBIR

			printf("Se aceptó un cliente \n");

			int i=0;
			unsigned char my_char;

			int tam = 0;

			if( read (cliente_sockfd, &tam, sizeof(int)) < 0 )
			{
				perror ("Ocurrio algun problema al recibir datos del cliente");
				exit(1);
			}
			printf("TAMAÑO ------------> %d", tam);
			sleep(2);

			unsigned char imagenGray[tam];

			while(i<tam){
				if( read (cliente_sockfd, &my_char, sizeof(unsigned char)) < 0 )
				{
					perror ("Ocurrio algun problema al recibir datos del cliente");
					exit(1);
				}
				
				imagenGray[i] = my_char;
				i++;
			}
			
		
				for(int i=0; i<n; i++){
					// if(size_of_image[i]<64)
					// 	printf ("Imagen a recibir: \n \%d \n", (unsigned) size_of_image[i]);
					// else
						printf ("Imagen a recibir: \n %d \n", (unsigned) imagenGray[i]);
				}

			unsigned char *imagenFiltro;

			for (nh = 0; nh < NUM_HILOS; nh++)
			{
				nhs[nh] = nh;
				//pthread_create(&tids[nh], NULL, funHilo, (void *)&nhs[nh]);
				pthread_create(&tids[nh], NULL, filtroImagen, (void *)&nhs[nh]);
			}

			if( write (cliente_sockfd, "Bienvenido cliente", 19) < 0 )
			{
				perror("Ocurrio un problema en el envio de un mensaje al cliente");
				exit(1);
			}
			
			printf("Concluimos la ejecución de la aplicacion Servidor \n");
			/*
			*	Cierre de las conexiones
			*/

			close (cliente_sockfd);
			kill(getppid(), SIGUSR1);
			exit(0);
			//Fin del proceso hijo
	
		}
			
		//Se requiere un wait en el proceso padre para que libere al proceso hijo y no quede en estado zombie
		//pero el wait es bloqueante y necesitamos que el proceso padre siga en espera de más peticiones
		//una alternativa es utilzar una señal para notificar al proceso padre que el hijo ha terminado
	
	}	

	close (sockfd);

	return 0;
}


void ISRsw(int sig){
	int estado;
	pid_t pid;

	if(sig == SIGUSR1){ //Si la señal que se recibe es SIGUSR1 entonces se trata del proceso hijo
		pid = wait(&estado);
		printf("USR1 recibida por el proceso %d \n", pid);
	}
}


int recibir(int cliente_sockfd, void *buf, int len){
	
	char workbuffer[len]; //Para almacenar lo que se acaba de recibir
	
	int total = 0; //Total de bytes recibidos
	int n;
	int bytesleft = len; //Bytes por recibir

	while(total < len){
		//recibir y agregar a workbuffer
		n = read (cliente_sockfd, workbuffer+total, bytesleft);
		if(n <= 0){
			break;
		}

		total += n;
		bytesleft -= n;

		printf("-Recibido: %d \n", total);
	}

	printf("Total recibido: %d", total);

	memcpy(buf,workbuffer,sizeof(workbuffer));
	switch(n){
		case -1:
			return -1;
		break;
		
		case 0:
			return 0;
		break;
		
		default:
			return total;
	}
}


void * filtroImagen(void *arg)
{
	int nh = *(int*) arg;
	register int y,x,ym,xm;

	int indiceGray,conv1,conv2,magnitud,indicem;
/*	unsigned char mascara[]={
							-1,-1 ,-1,
							-1 ,8 ,-1,
						-1 ,-1 ,-1};
*/
	char Gradf[]={
							1, 0 ,-1,
							2 ,0 ,-2,
							1 ,0 ,-1};
	char Gradc[]={
							-1,-2 ,-1,
							 0 ,0 , 0,
							 1 ,2 , 1};

	int tamBloque =height/NUM_HILOS;
	int iniBloque = nh * tamBloque; 
	int finBloque = iniBloque + tamBloque;
	if(nh<=2)
	{
	for (y = iniBloque; y < finBloque; y++)
	{
		for (x = 0; x < width-DIMASK ; x++)
		{
			conv1=0;
			conv2=0;
			indicem=0;
			for (ym= 0; ym < DIMASK; ym++)
			{

				for (xm = 0; xm < DIMASK; xm++)
				{
					indiceGray = ((y + ym) * width + (x + xm ));
					conv1+=imagenGray[indiceGray]*Gradf[indicem];
					conv2+=imagenGray[indiceGray]*Gradc[indicem++];
					
				}
			}
				conv1=conv1/4;
				conv2=conv2/4;
				
				magnitud=sqrt(pow(conv1,2)+pow(conv2,2));
				indiceGray=((y+1)*width + (x+1));
				imagenFiltro[indiceGray]=magnitud;
				// printf(">%d\n",conv1);
				// printf(">>%d\n",conv2);
				// printf(">>>%d\n",magnitud);
			
			
		}
	}
	}
	else
	{
	for (y = iniBloque; y < finBloque-DIMASK; y++)
		{
		for (x = 0; x < width-DIMASK ; x++)
		{
			conv1=0;
			conv2=0;
			indicem=0;
			for (ym= 0; ym < DIMASK; ym++)
			{

				for (xm = 0; xm < DIMASK; xm++)
				{
					indiceGray = ((y + ym) * width + (x + xm ));
					conv1+=imagenGray[indiceGray]*Gradf[indicem];
					conv2+=imagenGray[indiceGray]*Gradc[indicem++];
					
				}
			}
				conv1=conv1/4;
				conv2=conv2/4;
				
				magnitud=sqrt(pow(conv1,2)+pow(conv2,2));
				indiceGray=((y+1)*width + (x+1));
				imagenFiltro[indiceGray]=magnitud;
				
			
			
		}
	}


	}
	pthread_exit(arg);
}
