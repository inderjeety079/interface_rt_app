//
// Created by inderjeet on 3/17/20.
//

#ifndef INTERFACE_H_
#define INTERFACE_H_

#include <protocol/protocol.h>
#include <connection/udp_connection.h>
#include <global.h>


typedef struct interface_s_ {
  void * connection_handle;
  protocol_s_ptr_t protocol;
//  func_handler_1_args_t
}interface_s_t;

typedef struct interface_s_ * interface_s_ptr_t;

interface_s_ptr_t udp_interface_s_init(udp_connection_s_ptr_t connection_handle, protocol_s_ptr_t protocol);

#endif //INTERFACE_H_
