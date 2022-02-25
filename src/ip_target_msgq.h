
#ifndef XMAP_IP_TARGET_MSGQ_H
#define XMAP_IP_TARGET_MSGQ_H

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/msg.h>
#include<sys/ipc.h>
#include <pthread.h>

#define mesg_ip_len 64
struct ip_mesg{
	long int mtype;
    int type;
	char ip[mesg_ip_len];
};
static int msgq_id = 0;
static struct ip_mesg msg;
static pthread_mutex_t msg_in_mutex = PTHREAD_MUTEX_INITIALIZER;

int ip_target_msgq_init(int id);

// int ip_target_set_thread_pos(iterator_t *it);

int ip_target_msgq_get_ip(void *ip);

// port_h_t ip_target_file_get_port(shard_t *shard);

#endif // XMAP_IP_TARGET_FILE_H
