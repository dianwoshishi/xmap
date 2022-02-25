/*
 * ZMap Copyright 2013 Regents of the University of Michigan
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License. You may obtain a copy
 * of the License at http://www.apache.org/licenses/LICENSE-2.0
 */

#ifndef XMAP_MODULE_MSGQ_H
#define XMAP_MODULE_MSGQ_H

#include "output_modules.h"

#include "../fieldset.h"

#include <pthread.h>
static pthread_mutex_t msg_out_mutex = PTHREAD_MUTEX_INITIALIZER;

#define buffer_size 256
static int output_msgq_id = 0;

struct output_mesg{
	long int mtype;
    int type;
	char buffer[buffer_size];
};
static struct output_mesg msg;

int msgq_init(struct state_conf *conf, char **fields, int fieldlens);

int msgq_process(fieldset_t *fs);

int msgq_close(struct state_conf *c, struct state_send *s, struct state_recv *r);

#endif // XMAP_MODULE_CSV_H
