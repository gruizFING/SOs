#include <semSO.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
  semSO_init();

  semSO_create(10,1);

  semSO_P(10,1);
    sleep(30);
  semSO_V(10);

  semSO_close();

  return 0;
}

