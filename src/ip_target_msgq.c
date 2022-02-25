#include "ip_target_msgq.h"
#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "state.h"

#include "../lib/blocklist.h"
#include "../lib/logger.h"
#include "../lib/util.h"
static int ipv46_flag;

#define LOGGER_NAME "ip_target_msgq"

int ip_target_msgq_init(int id){
    ipv46_flag = xconf.ipv46_flag;

    // create msg queue
    key_t key = ftok("/tmp", id);
    msgq_id = msgget(key,IPC_CREAT | 0666);
	if(msgq_id == -1)
	{
		printf("create msg error \n");
		return -1;
	}
    return msgq_id;
}
int ip_target_msgq_get_ip(void *ip) {
    
    char  line[mesg_ip_len];
    char *ret, *pos;
    pthread_mutex_lock(&msg_in_mutex);
    if(msgrcv(msgq_id,(void *)&msg,sizeof(struct ip_mesg),1,0) < 0)
    {
        log_fatal(LOGGER_NAME,
                          "receive msg error \n");
        pthread_mutex_destroy(&msg_in_mutex) ;/* 销毁互斥锁 */
        return EXIT_FAILURE;
    }
    
    memcpy(line, msg.ip, mesg_ip_len);
    pthread_mutex_unlock(&msg_in_mutex);

    pos = strchr(line, '\n');
    if (pos != NULL) *pos = '\0';
    pos = strchr(line, '/');
    if (pos != NULL) *pos = '\0';
    if (!inet_str2in(line, ip, ipv46_flag)) {
        if (!xconf.ignore_filelist_error)
            log_fatal(LOGGER_NAME,
                      "could not parse IPv%d address from line: %s: %s",
                      ipv46_flag, line, strerror(errno));
        goto goon;
    }

    while (!blocklist_is_allowed_ip(ip)) {
    goon:
        pos = strchr(line, '\n');
        if (pos != NULL) *pos = '\0';
        pos = strchr(line, '/');
        if (pos != NULL) *pos = '\0';
        if (!inet_str2in(line, ip, ipv46_flag)) {
            if (!xconf.ignore_filelist_error)
                log_fatal(LOGGER_NAME,
                          "could not parse IPv%d address from line: %s: %s",
                          ipv46_flag, line, strerror(errno));
        }
    }
    // printf("%s\n",msg.ip);
    return EXIT_SUCCESS;
}