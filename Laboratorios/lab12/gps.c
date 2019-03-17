//GPGLL,ddmm.mmmm,n,dddmm.mmmm,e,hhmmss.ss,a*CC
//https://www.gpsinformation.org/dale/nmea.htm#GLL

//$GPGLL,1921.49133,N,09904.99370,W,070253.00,A,A*7F

#include <stdio.h>
#include <string.h>

#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>

char str[] = "GPGLL,4916.45,N,12311.12,W,225444,A,*1D";

char lat_deg[3] = "";
char lat_min[9] = "";
char lat_dir[2] = "";
char lon_deg[4] = "";
char lon_min[9] = "";
char lon_dir[2] = "";
char horas[3] = "";
char min[3] = "";
char seg[3] = "";
int valido = 0;

void procesarGLL(char* str);

int config_serial ( char *, speed_t );

int main(){

    register int i;
	int fd_serie;
	char dato[50];

    fd_serie = config_serial( "/dev/ttyACM0", B9600 );
	printf("serial abierto con descriptor: %d\n", fd_serie);

	while(1)
	{
		read ( fd_serie, &dato, 50 );
		//printf("%s\n", dato);

        procesarGLL(dato);

        if(valido==1){
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

	close( fd_serie );

    return 0;

}

void procesarGLL(char* str){

    if(strstr(str, "GLL") != NULL){
        printf("%s \n", str);

        printf("OK \n");

        if(strstr(str, "V")!=NULL){
            valido = 0;
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

            valido = 1;
        }

    }
    else{
        valido = 0;
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

/*

//contador de ,
            int count = 0;
            int i = 6;

            for(i; i<strlen(str); i++){
                //printf("%lu \n", strlen(lat_deg));
                
                if(str[i]==',')
                    count++;
                
                //printf("Comas: %d \n", count);
                //printf("i: %d \n", i);

                //LATITUDE
                //degrees
                if(i==6){
                    //printf("Lat deg \n");
                    lat_deg[strlen(lat_deg)] = str[i];
                    i++;
                    lat_deg[strlen(lat_deg)] = str[i];
                    lat_deg[strlen(lat_deg)] = '\0';
                }

                //minutes
                if(i>7 && count==0){
                    //printf("Lat min \n");
                    lat_min[strlen(lat_min)] = str[i];
                }

                //direction
                if(count==1 && strlen(lat_dir)!=1){
                    lat_min[strlen(lat_min)] = '\0';

                    //printf("Lat dir \n");
                    i++;
                    lat_dir[strlen(lat_dir)] = str[i];

                    lat_dir[strlen(lat_dir)] = '\0';
                }

                //LONGITUDE
                //degrees
                if(count==2 && strlen(lon_deg)!=3){
                    //printf("Lon deg \n");
                    i++;
                    lon_deg[strlen(lon_deg)] = str[i];
                    i++;
                    lon_deg[strlen(lon_deg)] = str[i];
                    i++;
                    lon_deg[strlen(lon_deg)] = str[i];
                    i++;
                    lon_deg[strlen(lon_deg)] = '\0';
                }

                //minutes
                if(count==2 && strlen(lon_deg)==3 && strlen(lon_min)!=5){
                    //printf("Lon min \n");
                    lon_min[strlen(lon_min)] = str[i];
                }

                //direction
                if(count==3 && strlen(lon_dir)!=1){
                    //printf("Lon dir \n");
                    i++;
                    lon_dir[0] = str[i];
                }

                //UTC
                if(count==4){
                    i++;
                    horas[strlen(horas)] = str[i];
                    i++;
                    horas[strlen(horas)] = str[i];

                    i++;
                    min[strlen(min)] = str[i];
                    i++;
                    min[strlen(min)] = str[i];

                    i++;
                    seg[strlen(seg)] = str[i];
                    i++;
                    seg[strlen(seg)] = str[i];

                    break;
                }
                
            }


*/