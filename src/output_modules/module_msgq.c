/*
 * ZMap Copyright 2013 Regents of the University of Michigan
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License. You may obtain a copy
 * of the License at http://www.apache.org/licenses/LICENSE-2.0
 */

#include "module_msgq.h"

#include <assert.h>
#include <errno.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../lib/logger.h"
#include "output_modules.h"

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/msg.h>
#include<sys/ipc.h>
#include <unistd.h> 

static char buff[buffer_size];
//sudo ./src/xmap -6 -Q 13 -M icmp6_echo_full -O  msgq-csv -f outersaddr -B 1M -P 17  -i en0
int msgq_init(struct state_conf *conf, char **fields, int fieldlens) {
    
	key_t key = ftok("/tmp",conf->output_msgq_id);
	output_msgq_id = msgget(key,IPC_CREAT | 0666);
	if(output_msgq_id == -1)
	{
		printf("create msg error \n");
		return 0;
	}

    return EXIT_SUCCESS;
}

int msgq_close(__attribute__((unused)) struct state_conf *c,
              __attribute__((unused)) struct state_send *s,
              __attribute__((unused)) struct state_recv *r) {


    return EXIT_SUCCESS;
}

static void hex_encode(char *buff, unsigned char *readbuf, size_t len) {
    for (size_t i = 0; i < len; i++) {
        sprintf(buff, "%02x", readbuf[i]);
    }
}

int msgq_process(fieldset_t *fs) {
    memset(buff, 0, buffer_size);
    char tmp[64] = {0};
    int offset = 0;
    for (int i = 0; i < fs->len; i++) {
        field_t *f = &(fs->fields[i]);
        if (i) {
            sprintf(buff + offset, ",");
            offset += 1;
        }

        if (f->type == FS_STRING) {
            if (strchr((char *) f->value.ptr, ',')) {
                sprintf(tmp, "\"%s\"", (char *) f->value.ptr);
            } else {
                sprintf(tmp, "%s", (char *) f->value.ptr);
            }
        } else if (f->type == FS_UINT64) {
            sprintf(tmp, "%" PRIu64, (uint64_t) f->value.num);
        } else if (f->type == FS_BOOL) {
            sprintf(tmp, "%" PRIi32, (int) f->value.num);
        } else if (f->type == FS_BINARY) {
            hex_encode(tmp, (unsigned char *) f->value.ptr, f->len);
        } else if (f->type == FS_NULL) {
            // do nothing
        } else {
            log_fatal("csv", "received unknown output type: %d (%s)", f->type,
                      f->name);
        }
        memcpy(buff + offset, tmp, strlen(tmp));
        offset += strlen(tmp);
        // FIXME may be buffer overflow
    }
    sprintf(buff + offset, "\n");

    pthread_mutex_lock(&msg_out_mutex);
    msg.mtype = 1;
    
    memcpy(msg.buffer, buff, buffer_size);

    // FIXME IPC_NOWAIT will drop data; 0 don't ; but 0 will block this thread
    int ret = msgsnd(output_msgq_id,(void *)&msg,sizeof(struct output_mesg),0);
    if(ret < 0) 
    {
        printf("failed with %d\n", errno);
    }
    pthread_mutex_unlock(&msg_out_mutex);

    return EXIT_SUCCESS;
}

output_module_t module_msgq = {
    .name                = "msgq-csv",
    .filter_duplicates   = 0, // framework should not filter out duplicates
    .filter_unsuccessful = 0, // framework should not filter out unsuccessful
    .init                = &msgq_init,
    .start               = NULL,
    .update              = NULL,
    .update_interval     = 0,
    .close               = &msgq_close,
    .process_ip          = &msgq_process,
    .supports_dynamic_output = NO_DYNAMIC_SUPPORT,
    .helptext = "Outputs one or more output fields as a comma-delimited file. "
                "By default, the \n"
                "probe module does not filter out duplicates or limit to "
                "successful fields, \n"
                "but rather includes all received packets. Fields can be "
                "controlled by \n"
                "setting --output-fields. Filtering out failures and duplicate "
                "packets can \n"
                "be achieved by setting an --output-filter."};
