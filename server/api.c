#include <stdio.h>

int get_the_function_from_buffer(char *buf);

char *handle_request(char *buf)
{
    int function = get_the_function_from_buffer(buf);
}

int get_the_function_from_buffer(char *buf)
{
}
