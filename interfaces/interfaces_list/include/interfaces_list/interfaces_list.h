//
// Created by inderjeet on 22/06/20.
//

#ifndef GOR_FW_NAVIGATION_INTERFACES_LIST_H
#define GOR_FW_NAVIGATION_INTERFACES_LIST_H


#include <non_rt_interface/non_rt_interface.h>





/*
 *
 *
 * */
            /*Objects for NonRT Interface*/
/*----------------------------------------------------------------------------*/
extern non_rt_interface_s_ptr_t non_rt_interface;
/*----------------------------------------------------------------------------*/
extern bool non_rt_interface_initialized;

/*----------------------------------------------------------------------------*/
extern void initialize_non_rt_interface();

/*----------------------------------------------------------------------------*/
/* Reactor function for NonRT Interface*/
extern void process_non_rt_data();


#endif //GOR_FW_NAVIGATION_INTERFACES_LIST_H
