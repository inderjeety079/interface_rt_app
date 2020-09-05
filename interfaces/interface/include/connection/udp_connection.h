//
// Created by inderjeet on 2/11/20.
//


#ifndef UDP_CONNECTION_H_
#define UDP_CONNECTION_H_

#include <inttypes.h>
#include <stdbool.h>
#include <containers/circular_buffer.h>
#include "lwiplib.h"
#include <global_definition.h>

#define UDP_RECEIVE_BUFF_SIZE 512

#define NON_RT_NODE_IP_ADDR "10.3.1.1"



typedef struct udp_connection_s_ {

  struct udp_pcb *pcb;
  uint32_t port;
  struct ip_addr ip;
  circular_buffer_s_ptr_t recv_buffer;
  bool connection_status;
  bool data_avail_to_read;
//  ifaces_data_reception_events_u_t ifaces_data_reception_events;


  func_handler_3_args_t init_handler_;
  func_handler_1_args_t connect_handler_;
  func_handler_3_args_t send_msg_handler_;
  func_handler_6_args_t receive_msg_handler_;

}udp_connection_s_t;

typedef struct udp_connection_s_ * udp_connection_s_ptr_t;


/*Constructor of udp connection struct*/
udp_connection_s_ptr_t udp_connection_s_init(uint32_t *port);




#endif //UDP_CONNECTION_H_
