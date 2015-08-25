#ifndef __GOBACKN_H
#define __GOBACKN_H

#include "type_buffer.h"

#define N 10
#define nFE 8 // largeur max de la fenetre d'emission
#define ID 101010

//Encapsulation du message pour go back n
typedef struct go_back_n{
    struct buffer buf;
    int seq;
}*Distant_data, local_data;

//Structure comprenant l'acquittement
typedef struct ack{
	int num_ack;
}*Ack, ack;

#endif