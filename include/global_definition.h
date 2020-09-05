//
// Created by inderjeet on 3/9/20.
//

#ifndef GLOBAL_DEFINITION_H_
#define GLOBAL_DEFINITION_H_

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
//#include <interrupt.h>
//#include "consoleUtils.h"

#define PRIVATE    static

#define LOG_MSG    printf

#define CRITICAL_SECTION_START(void)                        IntMasterIRQDisable(void)
#define CRITICAL_SECTION_END(void)                          IntMasterIRQEnable(void)


typedef uint32_t timestamp_t;

typedef int32_t error_t;


typedef error_t (*func_handler_1_args_t) (void *);
typedef error_t (*func_handler_2_args_t) (void *, void *);
typedef error_t (*func_handler_3_args_t) (void *, void *, void *);
typedef error_t (*func_handler_4_args_t) (void *, void *, void *, void *);
typedef error_t (*func_handler_5_args_t) (void *, void *, void *, void *, void *);
typedef error_t (*func_handler_6_args_t) (void *, void *, void *, void *, void *, void *);


typedef union ifaces_data_reception_events_u_
{
    struct sub_events_ {
        uint32_t non_rt_interface: 1;
        uint32_t reserved: 31;
    }sub_events_s;

    uint32_t events;
}ifaces_data_reception_events_u_t;


extern volatile ifaces_data_reception_events_u_t ifaces_data_reception_events;


#endif //GLOBAL_DEFINITION_H_
