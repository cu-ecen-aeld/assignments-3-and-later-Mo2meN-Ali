// Author is Mo'men
// Nov 28, 21:12

#include <syslog.h>
#include <stdio.h>
#include <ctype.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

#define     NUM_OF_ELEMENTS     (1U)

int main(int argc, char *argv[])
{
    FILE *pFile = NULL;

    if (argc < 2) {
        printf("Error arguments less than 2!\n");
        syslog(LOG_DEBUG, "Error arguments less than 2!");
        return 1;   // Return error!
    }

// This is optional but to add PID to the syslog output
    openlog(NULL, LOG_PID, LOG_SYSLOG);

// Open the file in write mode in order to reset its contents    
    pFile = fopen(argv[1], "w"); 
    if (NULL != pFile) {    // Opened successfully?
        fprintf(pFile, "%s", argv[2]);
        fclose(pFile);
    } else {
        printf("Error file does not exist!\n");
        syslog(LOG_DEBUG, "Error file does not exist!");
        return 1;  
    }  
    return 0;  
}
