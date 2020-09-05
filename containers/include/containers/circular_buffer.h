//
// Created by inderjeet on 2/11/20.
//

#ifndef CIRCULAR_BUFFER_H_
#define CIRCULAR_BUFFER_H_

#include <global_definition.h>
#include <stdlib.h>

#define PRIVATE static

#define error_t int32_t

#define MAX_CIRC_BUFF_SIZE 512

typedef struct float_element_ {
  float element;

}float_element_s_t;

typedef struct int32_element_ {
  int32_t element;

}int32_element_s_t;

typedef struct int8_element_ {
  int8_t element;

}int8_element_s_t;

typedef union circular_buffer_element_u_ {
  float_element_s_t float_element;
  int32_element_s_t int32_element;
  int8_element_s_t int8_element;

}circular_buffer_element_u_t;

typedef struct circular_buffer_s_
{
  int8_t data[MAX_CIRC_BUFF_SIZE];
//  circular_buffer_element_u_t data[MAX_CIRC_BUFF_SIZE];
  uint32_t rd_idx;
  uint32_t wr_idx;
  uint32_t max_size;

  func_handler_2_args_t read_byte_handler_;
  func_handler_2_args_t write_byte_handler_;
  func_handler_1_args_t flush_handler_;
  func_handler_2_args_t increment_buffer_handler_;
  func_handler_2_args_t decrement_buffer_handler_;

} circular_buffer_s_t;

typedef struct circular_buffer_s_ * circular_buffer_s_ptr_t;


circular_buffer_s_ptr_t circular_buffer_s_init(uint32_t buffer_size);



#endif //CIRCULAR_BUFFER_H_
