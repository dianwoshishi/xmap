/*
 * ZMap Copyright 2013 Regents of the University of Michigan
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License. You may obtain a copy
 * of the License at http://www.apache.org/licenses/LICENSE-2.0
 */

#include "output_modules.h"

#include <stdio.h>
#include <string.h>

extern output_module_t module_csv_file;
extern output_module_t module_msgq;

#ifdef JSON
extern output_module_t module_json_file;
#endif

#ifdef REDIS
extern output_module_t module_redis;
extern output_module_t module_redis_csv;
extern output_module_t module_csv_redis;
#endif

#ifdef MONGODB
extern output_module_t module_mongodb;
#endif

output_module_t *output_modules[] = {
    &module_csv_file,
#ifdef JSON
    &module_json_file,
#endif
#ifdef REDIS
    &module_redis,    &module_redis_csv,
#endif
#ifdef MONGODB
    &module_mongodb,
#endif
    // ADD YOUR MODULE HERE
    &module_msgq,
};

output_module_t *get_output_module_by_name(const char *name) {
    int num_modules =
        (int) (sizeof(output_modules) / sizeof(output_modules[0]));
    for (int i = 0; i < num_modules; i++) {
        if (!strcmp(output_modules[i]->name, name)) {
            return output_modules[i];
        }
    }

    return NULL;
}

void print_output_modules(void) {
    int num_modules =
        (int) (sizeof(output_modules) / sizeof(output_modules[0]));
    for (int i = 0; i < num_modules; i++) {
        printf("%s\n", output_modules[i]->name);
    }
}
