#include <semSO.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {

  semSO_init();

  semSO_P(10,2);
    sleep(5);
  semSO_V(10);

  semSO_destroy(10);

  semSO_close();

  return 0;
}

