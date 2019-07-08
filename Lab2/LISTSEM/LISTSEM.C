#include <so2011.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

int main() {
  int                     fd;
  struct so2011_msg_read  buf;

  fd = open("/dev/so2011_ctrl",O_RDWR);

  while (read(fd,&buf,sizeof(struct so2011_msg_read)))
    fprintf(stdout,"SEM: %d, VAL: %d, WAIT: %d\n",buf.semid,buf.value,buf.waitcount);

  close(fd);

  return 0;
}

