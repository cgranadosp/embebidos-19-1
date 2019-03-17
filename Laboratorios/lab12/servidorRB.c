#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>

#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

#include <sys/mman.h>

#define PUERTO 			5000	//Número de puerto asignado al servidor
#define COLA_CLIENTES 	5 		//Tamaño de la cola de espera para clientes
#define TAM_BUFFER 		100

void ISRsw(int sig);

void procesarGLL(char* str);
int config_serial ( char *, speed_t );

char* lat_deg;
char* lat_min;
char* lat_dir;
char* lon_deg;
char* lon_min;
char* lon_dir;
char* horas;
char* min;
char* seg;
int* valido;

int main(int argc, char **argv)
{
	lat_deg = mmap(NULL, sizeof(char)*3, PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, -1, 0);
	lat_min = mmap(NULL, sizeof(char)*9, PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, -1, 0);
	lat_dir = mmap(NULL, sizeof(char)*2, PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, -1, 0);
	lon_deg = mmap(NULL, sizeof(char)*4, PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, -1, 0);
	lon_min = mmap(NULL, sizeof(char)*9, PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, -1, 0);
	lon_dir = mmap(NULL, sizeof(char)*2, PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, -1, 0);
	horas = mmap(NULL, sizeof(char)*3, PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, -1, 0);
	min = mmap(NULL, sizeof(char)*3, PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, -1, 0);
	seg = mmap(NULL, sizeof(char)*3, PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, -1, 0);
	valido = mmap(NULL, sizeof(int), PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, -1, 0);

   	int sockfd, cliente_sockfd;
   	struct sockaddr_in direccion_servidor; //Estructura de la familia AF_INET, que almacena direccion
   	char leer_mensaje[TAM_BUFFER];

    socklen_t sin_size;

	pid_t pid;
	pid_t pidUART;

	register int i;
	int fd_serie;
	char dato[50];

    fd_serie = config_serial( "/dev/ttyACM0", B9600 );
	printf("serial abierto con descriptor: %d\n", fd_serie);

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
		perror ("Ocurrió un problema al configurar el socket");
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
		perror("Ocurrió un problema al crear la cola de aceptar peticiones de los clientes");
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
		pidUART = fork();
		if(!pidUART){

			while(1)
			{
				read ( fd_serie, &dato, 50 );
				//printf("%s\n", dato);

				procesarGLL(dato);

				if(*valido==1){
					printf("Latitud grados: %s \n", lat_deg);
					printf("Latitud minutos: %s \n", lat_min);
					printf("Latitud direccion: %s \n", lat_dir);

					printf("Longitud grados: %s \n", lon_deg);
					printf("Longitud minutos: %s \n", lon_min);
					printf("Longitud direccion: %s \n", lon_dir);

					printf("Horas: %s \n", horas);
					printf("Minutos: %s \n", min);
					printf("Segundos: %s \n", seg);
				}

				printf("--------------------------\n");

			}
		}


	while(1){	

		printf ("---En espera de peticiones de conexión ...\n");
		if( (cliente_sockfd = accept(sockfd, (struct sockaddr *)&direccion_servidor, &sin_size)) < 0 ) //Cada que termina un cliente vuelve a esperar en accept la petición de otro cliente
		{
			perror("Ocurrio algun problema al atender a un cliente");
			exit(1);
		}

		//Al aceptar la solicitud del cliente se crea un proceso para atender esa petición
		pid = fork();
		if(!pid){ //Si es el proceso hijo continúa...

		/*
		*	Inicia la transferencia de datos entre cliente y servidor
		*/
			printf("Se aceptó un cliente, atendiéndolo \n");

			if( read (cliente_sockfd, &leer_mensaje, TAM_BUFFER) < 0 )
			{
				perror ("Ocurrió algun problema al recibir datos del cliente");
				exit(1);
			}
			printf ("El cliente nos envió el siguiente mensaje: \n %s \n", leer_mensaje);

			//AQUÍ ENVIAR DATOS GPS

			//if(strstr(dato, "V")!=NULL){
			if(*valido==0){

				char msg[] = "No hay señal.";
				if( write (cliente_sockfd, msg, strlen(msg)) < 0 )
				{
					perror("Ocurrió un problema en el envío de un mensaje al cliente");
					exit(1);
				}
			}
			else{
				/* Define a temporary variable */
				char example[38] = "";

				example[0] = lat_deg[0];
				example[1] = lat_deg[1];
				example[2] = '\n';
				example[3] = lat_min[0];
				example[4] = lat_min[1];
				example[5] = lat_min[2];
				example[6] = lat_min[3];
				example[7] = lat_min[4];
				example[8] = lat_min[5];
				example[9] = lat_min[6];
				example[10] = lat_min[7];
				example[11] = '\n';
				example[12] = lat_dir[0];
				example[13] = '\n';
				example[14] = lon_deg[0];
				example[15] = lon_deg[1];
				example[16] = lon_deg[2];
				example[17] = '\n';
				example[18] = lon_min[0];
				example[19] = lon_min[1];
				example[20] = lon_min[2];
				example[21] = lon_min[3];
				example[22] = lon_min[4];
				example[23] = lon_min[5];
				example[24] = lon_min[6];
				example[25] = lon_min[7];
				example[26] = '\n';
				example[27] = lon_dir[0];
				example[28] = '\n';
				example[29] = horas[0];
				example[30] = horas[1];
				example[31] = ':';
				example[32] = min[0];
				example[33] = min[1];
				example[34] = ':';
				example[35] = seg[0];
				example[36] = seg[1];
				example[37] = '\0';


				//CONVERSIÓN PARA LAT y LON
				double lat = atof(lat_deg) + atof(lat_min)/60;
				if(lat_dir[0]=='S')
					lat = lat * -1;
				double lon = atof(lon_deg) + atof(lon_min)/60;
				if(lon_dir[0]=='W')
					lon = lon * -1;

				printf("----> %f , %f \n", lat, lon);

				/* Display the concatenated strings */
				//printf("%s \n", example);
				printf("/////////////// \n");
				
				if( write (cliente_sockfd, example, strlen(example)) < 0 )
				{
					perror("Ocurrió un problema en el envío de un mensaje al cliente");
					exit(1);
				}

				char lat_arr[21];
				sprintf(lat_arr, "%f" ,lat);
				char lon_arr[10];
				sprintf(lon_arr, "%f" ,lon);

				strcat(lat_arr, ",");
				strcat(lat_arr, lon_arr);

				printf("---->#### %s \n", lat_arr);

				if( write (cliente_sockfd, lat_arr, strlen(lat_arr)) < 0 )
				{
					perror("Ocurrió un problema en el envío de un mensaje al cliente");
					exit(1);
				}
			}
			
			printf("Concluimos la ejecución de la aplicación Servidor \n");
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

	close(sockfd);
	close(fd_serie);

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


void procesarGLL(char* str){

    if(strstr(str, "GLL") != NULL){
        printf("%s \n", str);

        printf("OK \n");

        if(strstr(str, "V")!=NULL){
            *valido = 0;
            printf("--- No hay señal. --- \n");            
        }
        else{
            
            lat_deg[0] = str[7];
            lat_deg[1] = str[8];
            lat_deg[2] = '\0';

            lat_min[0] = str[9];
            lat_min[1] = str[10];
            lat_min[2] = str[11];
            lat_min[3] = str[12];
            lat_min[4] = str[13];
            lat_min[5] = str[14];
            lat_min[6] = str[15];
            lat_min[7] = str[16];
            lat_min[8] = '\0';

            lat_dir[0] = str[18];
            lat_dir[1] = '\0';

            lon_deg[0] = str[20];
            lon_deg[1] = str[21];
            lon_deg[2] = str[22];
            lon_deg[3] = '\0';

            lon_min[0] = str[23];
            lon_min[1] = str[24];
            lon_min[2] = str[25];
            lon_min[3] = str[26];
            lon_min[4] = str[27];
            lon_min[5] = str[28];
            lon_min[6] = str[29];
            lon_min[7] = str[30];
            lon_min[8] = '\0';

            lon_dir[0] = str[32];
            lon_dir[1] = '\0';

            horas[0] = str[34];
            horas[1] = str[35];
            horas[2] = '\0';

            min[0] = str[36];
            min[1] = str[37];
            min[2] = '\0';

            seg[0] = str[38];
            seg[1] = str[39];
            seg[2] = '\0';

            *valido = 1;
        }

    }
    else{
        *valido = 0;
    }
}



/** @brief: Esta funcion Configura la interfaz serie
 *  @param: dispositivo_serial, Nombre el dispositivo serial a usar: ttyUSB0, ttyUSB1, etc
 *  @param: baudios, Velocidad de comunicacion. Se usa la constante Bxxxx, donde xxxx  es la
 *          velocidad. Estan definidas en termios.h. Ejemplo: B9600, B19200..
 *  @return: fd, Descriptor del serial
 *******************************************************************************************
 */
int config_serial( char *dispositivo_serial, speed_t baudios )
{
	struct termios newtermios;
  	int fd;
/*
 * Se abre un descriptor de archivo para manejar la interfaz serie
 * O_RDWR - Se abre el descriptor para lectura y escritura
 * O_NOCTTY - El dispositivo terminal no se convertira en el terminal del proceso
 * ~O_NONBLOCK - Se hace bloqueante la lectura de datos
 */
  	fd = open( dispositivo_serial, (O_RDWR | O_NOCTTY) & ~O_NONBLOCK );
	if( fd == -1 )
	{
		printf("Error al abrir el dispositivo tty \n");
		exit( EXIT_FAILURE );
  	}
/*
 * cflag - Proporciona los indicadores de modo de control
 *	CBAUD	- Velocidad de transmision en baudios.
 * 	CS8	- Especifica los bits por dato, en este caso 8
 * 	CLOCAL 	- Ignora las lineas de control del modem: CTS y RTS
 * 	CREAD  	- Habilita el receptor del UART
 * iflag - proporciona los indicadores de modo de entrada
 * 	IGNPAR 	- Ingnora errores de paridad, es decir, comunicación sin paridad
 * oflag - Proporciona los indicadores de modo de salida
 * lflag - Proporciona indicadores de modo local
 * 	TCIOFLUSH - Elimina datos recibidos pero no leidos, como los escritos pero no transmitidos
 * 	~ICANON - Establece modo no canonico, en este modo la entrada esta disponible inmediatamente
 * cc[]	 - Arreglo que define caracteres especiales de control
 *	VMIN > 0, VTIME = 0 - Bloquea la lectura hasta que el numero de bytes (1) esta disponible
 */
	newtermios.c_cflag 	= CBAUD | CS8 | CLOCAL | CREAD;
  	newtermios.c_iflag 	= IGNPAR;
  	newtermios.c_oflag	= 0;
  	newtermios.c_lflag 	= TCIOFLUSH | ~ICANON;
  	newtermios.c_cc[VMIN]	= 1;
  	newtermios.c_cc[VTIME]	= 0;

// Configura la velocidad de salida del UART
  	if( cfsetospeed( &newtermios, baudios ) == -1 )
	{
		printf("Error al establecer velocidad de salida \n");
		exit( EXIT_FAILURE );
  	}
// Configura la velocidad de entrada del UART
	if( cfsetispeed( &newtermios, baudios ) == -1 )
	{
		printf("Error al establecer velocidad de entrada \n" );
		exit( EXIT_FAILURE );
	}
// Limpia el buffer de entrada
	if( tcflush( fd, TCIFLUSH ) == -1 )
	{
		printf("Error al limpiar el buffer de entrada \n" );
		exit( EXIT_FAILURE );
	}
// Limpia el buffer de salida
	if( tcflush( fd, TCOFLUSH ) == -1 )
	{
		printf("Error al limpiar el buffer de salida \n" );
		exit( EXIT_FAILURE );
	}
/*
 * Se establece los parametros de terminal asociados con el
 * descriptor de archivo fd utilizando la estructura termios
 * TCSANOW - Cambia los valores inmediatamente
 */
	if( tcsetattr( fd, TCSANOW, &newtermios ) == -1 )
	{
		printf("Error al establecer los parametros de la terminal \n" );
		exit( EXIT_FAILURE );
	}
//Retorna el descriptor de archivo
	return fd;
}
