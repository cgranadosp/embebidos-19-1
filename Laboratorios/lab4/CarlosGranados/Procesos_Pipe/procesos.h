#ifndef PROCESOS_H
#define PROCESOS_H

void procesoHijo(int np, int *datos, int pipefd[]);
void procesoPadre(int pipefd[]);

#endif