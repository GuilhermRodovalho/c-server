#include <stdio.h>
#include <stdlib.h>
#include "queue.h"

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

  int size = queue_size(queue);

  while (size > 0)
  {
    char *strAux = get_queue(queue);

    printf("%s\n", strAux);

    pop(queue);

    printf("%d\n", queue_size(queue));

    size = queue_size(queue);
  }

  return 0;
}