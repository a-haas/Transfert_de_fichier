#ifndef _STRUCTRURES_H_
#define _STRUCTURES_H_

#define ID 10

typedef struct data_struct{
	struct buffer buff;
	int seq_num;
}*Distant_data, local_data;

typedef struct ack_struct{
	int ack_num;
}*Ack, ack;

#endif
