//
// Created by inderjeet on 2/12/20.
//

#include <containers/circular_buffer.h>
#include <global.h>

PRIVATE error_t read_byte(circular_buffer_s_ptr_t self, int8_t * byte);
PRIVATE error_t write_byte(circular_buffer_s_ptr_t self, int8_t * byte);
PRIVATE error_t flush_buffer(circular_buffer_s_ptr_t self);
PRIVATE error_t increment_buffer(circular_buffer_s_ptr_t self, uint32_t * n);
PRIVATE error_t decrement_buffer(circular_buffer_s_ptr_t self, uint32_t * n);


/******* Constructor of circular buffer struct **********/

circular_buffer_s_ptr_t circular_buffer_s_init(uint32_t buffer_size) {

    circular_buffer_s_ptr_t __object = NULL;

    CRITICAL_SECTION_START();
     __object = (circular_buffer_s_ptr_t) malloc(sizeof(circular_buffer_s_t));
    CRITICAL_SECTION_END();

     if (__object != NULL) {
         __object->rd_idx = 0;
         __object->wr_idx = 0;

         if(buffer_size <= MAX_CIRC_BUFF_SIZE) {
             __object->max_size = buffer_size;
         }
         else {
             __object->max_size = MAX_CIRC_BUFF_SIZE;
             LOG_MSG("WARN: Request for circular buffer of size more than max buff size\n");
         }

         __object->read_byte_handler_ = (func_handler_2_args_t)&read_byte;
         __object->write_byte_handler_ = (func_handler_2_args_t)&write_byte;
         __object->flush_handler_ = (func_handler_1_args_t)&flush_buffer;
         __object->increment_buffer_handler_ = (func_handler_2_args_t)&increment_buffer;
         __object->decrement_buffer_handler_ = (func_handler_2_args_t)&decrement_buffer;
         LOG_MSG("Circular buffer object constructed\n");
     }

     else
     {
         LOG_MSG("Malloc returned Null\n");

     }

     return __object;
}


PRIVATE error_t read_byte(circular_buffer_s_ptr_t self, int8_t * byte) {
    error_t error_no = 0;

    if (self->wr_idx != self->rd_idx) {
        *byte = self->data[self->rd_idx];
        self->rd_idx = (self->rd_idx + 1) % self->max_size;
    }

    else {
        error_no = -1;
        LOG_MSG("WARN: Circular Buffer Underflow \n");
    }


    return error_no;
}

PRIVATE error_t write_byte(circular_buffer_s_ptr_t self, int8_t * byte) {
    error_t error_no = 0;

    if (self->wr_idx == self->rd_idx) {

        LOG_MSG("WARN: Circular Buffer Overflow: wr_idx: %d, rd_idx: %d \n", self->wr_idx, self->rd_idx);
    }

    self->data[self->wr_idx] = *byte;

    self->wr_idx = (self->wr_idx + 1) % self->max_size;

    return error_no;
}

PRIVATE error_t flush_buffer(circular_buffer_s_ptr_t self) {

    error_t error_no = 0;

    self->rd_idx = 0;
    self->wr_idx = 0;

    return error_no;
}

PRIVATE error_t increment_buffer(circular_buffer_s_ptr_t self, uint32_t * n) {

    int count = 0;
    error_t error_no = 0;

    while(count < *n) {
        self->rd_idx = (self->rd_idx + 1) % self->max_size;
        count++;
    }

    return error_no;
}

PRIVATE error_t decrement_buffer(circular_buffer_s_ptr_t self, uint32_t * n) {

    error_t error_no = 0;

//self->rd_idx = (self->rd_idx - n) % self->max_size;

    return error_no;
}

