#include "memoria.h"
#include <errno.h>
#include <stdlib.h>
#include <sys/stat.h>
#define PERMISOS S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH

int abrir_segmento(char clave, int segsize)
{
    key_t llave;
    llave = ftok(".",clave);
    if(llave == (key_t)-1)
        return -1;
    return shmget(llave, segsize, IPC_CREAT | PERMISOS); // si existe no se devuelve error
}

bool existe_segmento(char clave, int segsize)
{
    int shmid = shmget(ftok(".",clave), segsize, PERMISOS);
    return !(shmid == -1 && errno == ENOENT);
}

void * mapear_segmento(char clave, int segsize)
{
    key_t llave;
    llave = ftok(".",clave);
    if(llave == (key_t)-1)
        return NULL;
    int shmid = shmget(llave, segsize,PERMISOS);
    if(shmid != -1)
        return shmat(shmid, 0, 0); // mismos permisos
    else
        return NULL;
}

int liberar_segmento(void* addr)
{
    return shmdt(addr);
}

int destruir_segmento(char clave, int segsize)
{
    key_t llave;
    llave = ftok(".",clave);
    if(llave == (key_t)-1)
        return -1;
    int shmid = shmget(llave, segsize, PERMISOS);
    if(shmid != -1)
        return(shmctl(shmid, IPC_RMID, NULL));
    else
        return -1;
}

void* mapear_segmento_id(int shmid)
{
    return shmat(shmid, 0, 0);
}

int destruir_segmento_id(int shmid)
{
    return (shmctl(shmid, IPC_RMID, NULL));
}
