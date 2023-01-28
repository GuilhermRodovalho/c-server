

char *get_first_word(char *str)
{
    char *first_word = str;
    int i = 0;
    while (str[i] != ' ' && str[i] != '\0')
    {
        i++;
    }
    str[i] = '\0';
    return first_word;
}

char *get_next_word(char *str)
{
    int i = 0;
    while (str[i] != ' ' && str[i] != '\0')
    {
        i++;
    }
    while (str[i] == ' ' || str[i] == '\0')
    {
        i++;
    }
    return &str[i];
}