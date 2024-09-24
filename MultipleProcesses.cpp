#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <unistd.h> 

int main(int argc, char *argv[])
{
    char *str = argv[1]; //string we passed

    while (1) {
      printf("%s\n", str);
      sleep(5);
    }
    return 0;
}