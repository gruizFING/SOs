#include <semSO.h>

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
int semSO_init()
{
    return 0;
}

/**
 *  semSO_create
 *
 *  Crea/inicializa un semaforo binario.
 *  Parámetros:
 *  int – Identificacion del semaforo 0..31.
 *  int – Valor para inicialización (0 - bloqueado, 1 - Libre).
 *  Retorna:
 *  0 – La operacion culmino con exito.
 * -1 – Hubo un error.
*/
int semSO_create(int s,int i)
{
    if (s >= 0 && s < 32 && (i == 0 || i == 1))
    {
        int fd;
        char buffer[2];
        char* nodo = malloc(sizeof(char)*15);
        sprintf(nodo,"/dev/so2011_%d",s);
        sprintf(buffer,"i%d",i);
        fd = open(nodo,O_RDWR);
        free(nodo);
        if (write(fd,&buffer,sizeof(char)*2) <= 0)
        {
            close(fd);
            return -1;
        }
        else
        {
            close(fd);
            return 0;
        }
    }
    return -1;
}

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
int semSO_destroy(int s)
{
    if (s >= 0 && s < 32)
    {
        int fd;
		char buffer = 'd';
        char* nodo = malloc(sizeof(char)*15);
        sprintf(nodo,"/dev/so2011_%d",s);
        fd = open(nodo,O_RDWR);
        free(nodo);
        if (write(fd,&buffer,sizeof(char)) <= 0)
        {
            close(fd);
            return -1;
        }
        else
        {
            close(fd);
            return 0;
        }
    }
    return -1;
}

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
int semSO_P(int s,int p)
{
    if (s >= 0 && s < 32 && p >= 1 && p < 6)
    {
        int fd;
        char buffer[2];
        char* nodo = malloc(sizeof(char)*15);
        sprintf(nodo,"/dev/so2011_%d",s);
        sprintf(buffer,"p%d",p);
        fd = open(nodo,O_RDWR);
        free(nodo);
        if (write(fd,&buffer,sizeof(char)*2) <= 0)
        {
			close(fd);
            return -1;
        }
        else
        {
            close(fd);
            return 0;
        }
    }
    return -1;
}
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
int semSO_V(int s)
{
    if (s >= 0 && s < 32)
    {
        int fd;
		char buffer = 'v';
        char* nodo = malloc(sizeof(char)*15);
        sprintf(nodo,"/dev/so2011_%d",s);
        fd = open(nodo,O_RDWR);
        free(nodo);
        if (write(fd,&buffer,sizeof(char)) <= 0)
        {
            close(fd);
            return -1;
        }
        else
        {
            close(fd);
            return 0;
        }
    }
    return -1;
}

/**
 *  semSO_close
 *
 *  Finaliza el uso de la biblioteca.
*/
void semSO_close()
{
}

