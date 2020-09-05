//
// Created by inderjeet on 3/8/20.
//

#ifndef PROTOCOL_H_
#define PROTOCOL_H_

#include <global_definition.h>


typedef struct protocol_s_ {


//  func_handler_1_args_t init_handler_;
  func_handler_3_args_t parse_data_handler_;
  func_handler_4_args_t pack_data_handler_;



}protocol_s_t;

typedef struct protocol_s_ * protocol_s_ptr_t;


#endif //PROTOCOL_H_
