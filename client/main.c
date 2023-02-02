#include "queue.h"
#include <stdio.h>
#include <stdlib.h>

int main()
{
  char *str1 = "guilherme";
  char *str2 = "joão";
  char *str3 = "vai todo mundo passar";

  Queue *queue = create_queue();

  push(queue, str1);
  push(queue, str2);
  push(queue, str3);

  int total = queue_size(queue);

  printf("cheguei\n");

  int size = queue_size(queue);

  printf("tururu");

  while (size > 0)
  {
    printf("cheguei 2");
    char *strAux;

    get_queue(queue, strAux);

    printf("%s\n", strAux);

    pop(queue);

    printf("%d\n", queue_size(queue));

    size = queue_size(queue);
  }

  return 0;
}