#ifndef __BUFFER_H
#define __BUFFER_H

#define BUFFER_LENGTH 1000

typedef struct buffer{
    char msg[BUFFER_LENGTH];
    int taille;
}*Buffer;

#endif
