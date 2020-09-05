//
// Created by inderjeet on 3/17/20.
//
#include <containers/deque.h>
#include <global.h>

PRIVATE error_t init_list(list_s_ptr_t self);
PRIVATE error_t pop(list_s_ptr_t self, list_dtype_u_ptr_t data);
PRIVATE error_t push(list_s_ptr_t self , list_dtype_u_ptr_t data);
PRIVATE error_t flush(list_s_ptr_t self);
PRIVATE error_t delete_elements(list_s_ptr_t self , element_s_ptr_t begin_ptr, element_s_ptr_t end_ptr);
PRIVATE error_t insert(list_s_ptr_t self, element_s_ptr_t insert_after, list_dtype_u_ptr_t data);


list_s_ptr_t list_s_init() {

    list_s_ptr_t __object = NULL;
    CRITICAL_SECTION_START();
    __object = (list_s_ptr_t) malloc(sizeof(list_s_t));
    CRITICAL_SECTION_END();

    if (__object != NULL)
    {
        __object->init_handler_ = (func_handler_1_args_t)&init_list;
        __object->pop_handler_ = (func_handler_2_args_t)&pop;
        __object->push_handler_ = (func_handler_2_args_t)&push;
        __object->insert_handler_ = (func_handler_3_args_t)&insert;
        __object->flush_handler_ = (func_handler_1_args_t)&flush;
        __object->delete_elements_handler_ = (func_handler_3_args_t)&delete_elements;

        LOG_MSG("List object constructed\n");
    }

    else
    {
        LOG_MSG("Malloc returned NULL\n");

    }

    return __object;

}


PRIVATE error_t init_list(list_s_ptr_t self)
{
    self->begin = NULL;
    self->rbegin = NULL;
    self->end = NULL;
    self->rend = NULL;

    return 0;
}

PRIVATE error_t pop(list_s_ptr_t self, list_dtype_u_ptr_t data)
{
    error_t error_no = 0;

    if( self->begin == NULL)
    {
        memset((list_dtype_u_t *) data, 0, sizeof(list_dtype_u_t));
        error_no = -1;

    }

    else
    {
        /*copy the next */
        element_s_ptr_t temp = self->begin;
        element_s_ptr_t next_begin = self->begin->next;
        next_begin->prev = NULL;
        // printf("pop begin data : %f , %fs \n", temp->data.v_s_pair.distance, temp->data.v_s_pair.velocity);
        // memcpy((list_dtype_u_ptr_t) data, (list_dtype_u_ptr_t) &(temp->data), sizeof(list_dtype_u_t));
        // temp->prev = NULL;
        //    free the allocated space
        free(temp);
        self->begin = next_begin;
        self->rend = self->begin;


        if(NULL == self->begin)
        {
            self->end = NULL;
            self->rbegin = NULL;
            self->rend = NULL;
        }
    }

    return error_no;
}

PRIVATE error_t push(list_s_ptr_t self , list_dtype_u_ptr_t data)
{
    error_t error_no = 0;
    static int element_count = 0;

    CRITICAL_SECTION_START();
    // allocates memory for a new list element
    element_s_ptr_t new_element = (element_s_ptr_t) malloc(sizeof(element_s_t));
    CRITICAL_SECTION_END();
    element_count++;
    //    LOG_MSG("Allocating %d bytes , element_count: %d\n", sizeof(element_s_t), element_count);

    if (NULL != new_element)
    {
        memcpy((list_dtype_u_ptr_t) &(new_element->data), (list_dtype_u_ptr_t) data, sizeof(list_dtype_u_t));
        new_element->next = NULL;

        if (self->end == NULL)
        {
            LOG_MSG("First element getting pushed\n");
            new_element->prev = NULL;
            self->end = new_element;

            /* Copy the new element address in head i.e begin */
            self->begin = new_element;

            self->rend = self->begin;
            self->rbegin = self->end;
        }

        else
        {
            //LOG_MSG("new_element distance is: %f \n",new_element->data.v_s_pair.distance);
            //LOG_MSG("new element velocity is: %f \n",new_element->data.v_s_pair.velocity);
            self->end->next = new_element;
            new_element->prev = self->end;

            /* update the new end of list*/
            self->end = new_element;
            self->rbegin = self->end;

        }

    }

    else
    {
        error_no = 1;
        LOG_MSG("malloc returned null ptr\n");
    }


    return error_no;

}


//static error_t rpop(element_s_ptr_t rbegin_ptr)
//{
//  element_s_ptr_t temp = rbegin_ptr->prev;
//  temp->next = NULL;
//  rbegin_ptr = temp;
//  // free the mem allocated to old last element
//}
//

//static error_t rpush(element_s_ptr_t  rend_ptr)
//{
//  error_t error_no = 0;

//
//  element_s_ptr_t temp = rend_ptr->next;
//
//  temp->prev = NULL;
//  rend_ptr = temp;
//
//  return error_no;
//}
//

//
//static error_t peek(element_s_ptr_t begin_ptr, const list_dtype_s_t &data)
//{
//  error_t error_no = 0;
//  memcpy((void *)&data,  (void *)&(begin_ptr->data), sizeof(list_dtype_s_t));
//  return error_no;
//}
//
//static error_t rpeek(element_s_ptr_t rbegin_ptr, const list_dtype_s_t &data)
//{
//  error_t error_no = 0;
//  memcpy((void *)&data, (void *) &(rbegin_ptr->data), sizeof(list_dtype_s_t));
//  return error_no;
//}

// Deletes the entire list
PRIVATE error_t flush(list_s_ptr_t self)
{
    error_t error_no = 0;
    element_s_ptr_t itr = NULL;

    int count = 0;

    if(self->begin != NULL)
    {

        for (itr = self->begin; itr != self->end; )
        {
            list_dtype_u_t temp_data = itr->data;
            element_s_ptr_t temp_itr = itr->next;
            free(itr);
            count++;
            itr = temp_itr;
        }

        if(itr != NULL)
        {
            free(itr);
            count++;
        }

        LOG_MSG("Flushed list containing %d elements\n", count);
        self->end = NULL;
        self->begin = NULL;
        self->rbegin = NULL;
        self->rend = NULL;

    }

    else
    {
        //      LOG_MSG("Null ptr passed to flush. Doing Nothing\n");
    }
    return error_no;
}

PRIVATE error_t delete_elements(list_s_ptr_t self , element_s_ptr_t begin_ptr, element_s_ptr_t end_ptr)
{
    //    LOG_MSG("Delete called \n");

    error_t error_no = 0;
    element_s_ptr_t itr = NULL;
    int count = 0;

    element_s_ptr_t temp_prev = begin_ptr->prev;
    element_s_ptr_t temp_next;

    if(begin_ptr != NULL && end_ptr != NULL)
    {

        for (itr = begin_ptr; itr != end_ptr; )
        {
            list_dtype_u_t temp_data = itr->data;
            element_s_ptr_t temp_itr = itr->next;
            free(itr);
            itr = temp_itr;
            count++;
        }

        if(NULL == itr->next)
        {
            free(itr);

            if (NULL == temp_prev)
            {
            self->begin = NULL;
            self->rend = NULL;
            self->end = NULL;
            self->rbegin = NULL;

            //                LOG_MSG("Deleted the complete list \n");
            // printf("deleted the complete list \n");
            }

            else
            {
                self->end = temp_prev;
                self->rbegin = temp_prev;
                //                LOG_MSG("Deleted till the end \n");
            }

            count++;
        }

        else
        {
            temp_next = itr->next;
            free(itr);

            if ( NULL == temp_prev)
            {
                self->begin = temp_next;
                self->rend = temp_next;
                //                LOG_MSG("Deleted from the beginning\n");
            }

            else
            {
                //                LOG_MSG("Deleted in between elements elements \n");
                temp_prev->next = temp_next;
                temp_next->prev = temp_prev;
            }

            count++;
        }
    }
    //    LOG_MSG("deleted %d elements \n", count);

    return 0;
}

// Inserts an element in the list
PRIVATE error_t insert(list_s_ptr_t self, element_s_ptr_t insert_after, list_dtype_u_ptr_t data)

{
    error_t error_no = 0;
    element_s_ptr_t new_element;
    element_s_ptr_t insert_after_next;

    if (NULL == self->begin || NULL == insert_after)
    {
        error_no = -1;
        LOG_MSG("Either Empty list or insert_after itself is NULL \n");
        return error_no;

    }

    // allocates memory for a new list element
    CRITICAL_SECTION_START();
    new_element = (element_s_ptr_t) malloc(sizeof(element_s_t));
    CRITICAL_SECTION_END();
    new_element->next = NULL;

    // LOG_MSG("Inserting after first: %d , second: %d\n", (int)(insert_after->data.float_pair.first * 1000), (int)(insert_after->data.float_pair.second * 1000));

    // if(insert_after->next != NULL)
    // {
    //     LOG_MSG("Inserting before first: %d , second: %d\n", (int)(insert_after->next->data.float_pair.first * 1000), (int)(insert_after->next->data.float_pair.second * 1000));
    // }

    if (NULL != new_element)
    {
        memcpy( (list_dtype_u_ptr_t) &(new_element->data), (list_dtype_u_ptr_t) data, sizeof(list_dtype_u_t));

        /* Insert the new element after the element insert_after */
        insert_after_next = insert_after->next;
        insert_after->next = new_element;
        new_element->next = insert_after_next;

        /* Update prev pointers*/
        new_element->prev = insert_after;
        insert_after_next->prev = new_element;
        //        LOG_MSG("Element inserted\n",(stop_time - start_time));
    }

    else
    {
        LOG_MSG("malloc returned NULL\n");
        error_no = -1;
    }

    return error_no;
}

