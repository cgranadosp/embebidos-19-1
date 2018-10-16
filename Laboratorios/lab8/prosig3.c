#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<signal.h>
#include<unistd.h>
#include<sys/wait.h>

#define NUM_PROC 4

void proceso_hijo(int np);
void proceso_padre(pid_t pid[]);
void ISRsw(int sig);

int main()
{
	pid_t pid[NUM_PROC];
	register int np;

	printf("Probando procesos...\n");

	//Colgando la función de la señal SIGUSR1 del proceso padre. Los hijos heredan los manejadores de las señales.
	if(signal(SIGUSR1, ISRsw) == SIG_ERR){
		perror("Error al asignar el manejador...\n");
		exit(EXIT_FAILURE);
	}

	for( np = 0; np < NUM_PROC; np++ )
	{
		pid[np] = fork();
		if( pid[np] == -1 )
		{
			perror("Error al crear el proceso");
			exit(EXIT_FAILURE);
		}
		if(!pid[np])
		{
			proceso_hijo(np);
		}
	}
	proceso_padre(pid); //Se envía el arreglo de pids al proceso padre

	return 0;
}

void proceso_hijo( int np )
{
	printf("Proceso hijo %d con pid %d\n", np, getpid());

	pause(); //Queda en espera de una señal

	printf("Señal recibida en el proceso hijo %d \n", np);

	exit(np);
}

void proceso_padre(pid_t pid[])
{
	register int np;
	int status;
	pid_t pid1;

	printf("Proceso padre con pid %d\n", getpid());

	for( np = 0; np < NUM_PROC; np++ )
	{
		kill(pid[np], SIGUSR1);
		sleep(1);
	}

	for( np = 0; np < NUM_PROC; np++ )
	{
		//Esperar a que los hijos terminen para que no queden en estado zombie
		pid1 = wait( &status );
		printf("Proceso hijo %d terminado con PID %d\n", status>>8, pid1);
	}
}


//Definiendo el manejador de la señal
void ISRsw(int sig){
	if(sig == SIGUSR1){
		printf("Señal SIGUSR1 detectada \n");
	}
}