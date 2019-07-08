#ifndef SEMSO_H
#define SEMSO_H

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

/**
 *  Sistemas Operativos
 *  Curso 2011.
 *
 *  Biblioteca de semaforos binarios con prioridades.
 *
 **/

/**
 *  semSO_init
 *
 *  Incializa las estructuras internas de la biblioteca para el proceso.
 *  Parámetros:
 *     No tiene.
 *  Retorna:
 *   0 - Se inicializó la librería en forma correcta.
 *  -1 - Hubo un error.
 */
int semSO_init();

/**
 *  semSO_create
 *
 *  Crea/inicializa un semaforo binario.
 *  Parámetros:
 *  int – Identificacion del semaforo 0..31.
 *  int – Valor para inicialización (0 - bloqueado, 1 -Libre).
 *  Retorna:
 *  0 – La operacion culmino con exito.
 * -1 – Hubo un error.
*/
int semSO_create(int,int);

/**
 *  semSO_destroy
 *
 *  Destruye un semaforo binario.
 *  Parámetros:
 *  int – Identificacion del semaforo 0..31.
 *  Retorna:
 *  0 – La operacion culmino con exito.
 * -1 – Hubo un error.
*/
int semSO_destroy(int);

/**
 *  semSO_P
 *
 *  Operación P.
 *  Parámetros:
 *  int – Identificacion del semaforo 0..31.
 *  int – Prioridad 1..5.
 *  Retorna:
 *  0 – La operacion culmino con exito.
 * -1 – Hubo un error.
*/
int semSO_P(int,int);
/**
 *  semSO_V
 *
 *  Operación V.
 *  Parámetros:
 *  int – Identificacion del semaforo 0..31.
 *  Retorna:
 *  0 – La operacion culmino con exito.
 * -1 – Hubo un error.
*/
int semSO_V(int);

/**
 *  semSO_close
 *
 *  Finaliza el uso de la biblioteca.
*/
void semSO_close();


#endif

