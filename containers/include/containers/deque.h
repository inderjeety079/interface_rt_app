//
// Created by inderjeet on 12/10/19.
//

#ifndef GOR_FW_NAVIGATION_INC_DEQUE_H_
#define GOR_FW_NAVIGATION_INC_DEQUE_H_

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <global_definition.h>



typedef struct v_s_pair_s_
{
  float velocity;
  float distance;
}v_s_pair_s_t;

typedef struct v_t_pair_s_
{
  float velocity;
  float time;
}v_t_pair_s_t;

typedef struct a_t_pair_s_
{
  float accel;
  float time;
}a_t_pair_s_t;

typedef struct j_t_pair_s_
{
  float jerk;
  float time;
}j_t_pair_s_t;

typedef struct float_pair_s
{
  float first;
  float second;
}float_pair_s_t;


typedef union list_dtype_u_
{
  v_s_pair_s_t v_s_pair;
  v_t_pair_s_t v_t_pair;
  a_t_pair_s_t a_t_pair;
  j_t_pair_s_t j_t_pair;
  float_pair_s_t float_pair;

}list_dtype_u_t;

typedef union list_dtype_u_ *list_dtype_u_ptr_t;

typedef struct element_s_
{
  int32_t fwd_idx;
  int32_t bkwd_idx;
  list_dtype_u_t data;
  struct element_s_ * prev;
  struct element_s_ * next;
}element_s_t;

typedef struct element_s_ *element_s_ptr_t;


typedef struct list_s_
{
  element_s_ptr_t begin;
  element_s_ptr_t rbegin;
  element_s_ptr_t end;
  element_s_ptr_t rend;


  func_handler_1_args_t init_handler_;
  func_handler_2_args_t pop_handler_;
  func_handler_2_args_t push_handler_;
  func_handler_3_args_t insert_handler_;
//  func_handler_t rpop_handler_;
//  func_handler_t rpush_handler_;
//  func_handler_t peek_handler_;
//  func_handler_t rpeek_handler_;
  func_handler_1_args_t flush_handler_;
  func_handler_3_args_t delete_elements_handler_;

}list_s_t;

typedef struct list_s_ * list_s_ptr_t;


list_s_ptr_t list_s_init();


#endif //GOR_FW_NAVIGATION_INC_DEQUE_H_
