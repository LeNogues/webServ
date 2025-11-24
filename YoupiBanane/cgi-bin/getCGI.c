#include <unistd.h>
#include <stdio.h>

int main()
{
    printf("content-type: text/html\n\n");
    printf("<html><body><h1>one cgi response </h1></body></html>\n");
    return (0);
}
