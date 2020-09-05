//
// Created by inderjeet on 2/11/20.
//

#include <connection/udp_connection.h>
#include <global.h>

PRIVATE int init_comm(udp_connection_s_ptr_t self, struct ip_addr * ip, uint32_t *port);
PRIVATE int send_msg(udp_connection_s_ptr_t self, const unsigned char *msg, uint32_t *size);
PRIVATE void receive_msg(void *arg, struct udp_pcb *pcb, struct pbuf *p, struct ip_addr *addr, uint16_t port);
PRIVATE int32_t connect(udp_connection_s_ptr_t self);


udp_connection_s_ptr_t udp_connection_s_init(uint32_t *port)
{
    udp_connection_s_ptr_t __object = NULL;
    uint32_t buffer_size = 512;
    circular_buffer_s_ptr_t buffer = circular_buffer_s_init(buffer_size);

    if(NULL != buffer)
    {
        CRITICAL_SECTION_START();
        __object = (udp_connection_s_ptr_t) malloc(sizeof(udp_connection_s_t));
        CRITICAL_SECTION_END();

        if (__object != NULL)
        {
            __object->port = *port;
            __object->connection_status = false;
            __object->recv_buffer = buffer;
            __object->data_avail_to_read = 0;
            __object->init_handler_ = (func_handler_3_args_t)&init_comm;
            __object->connect_handler_ = (func_handler_1_args_t)&connect;
            __object->send_msg_handler_ = (func_handler_3_args_t)&send_msg;
            __object->receive_msg_handler_ = (func_handler_6_args_t)&receive_msg;
            LOG_MSG("udp_connection object constructed\n");
        }

        else
        {
            LOG_MSG("Malloc returned NULL\n");

        }

    }

    else
    {
        LOG_MSG("ERROR: Null ptr returned from circular buffer constructor\n");


    }

    return __object;

}

/*----------------------------------------------------------------------------*/
/* Function : init_comm */
/*----------------------------------------------------------------------------*/
/*!
  @brief

  @details

  @pre

  @param void

  @param

  @return int

*/
PRIVATE int init_comm(udp_connection_s_ptr_t self, struct ip_addr * ip, uint32_t *port)
{

    /* Local Variables */
    int iret = 1;

    /* Validation */

    /* Algorithm */
    self->pcb = (struct udp_pcb *) udp_new();
    self->port = *port;

    if (self->pcb != NULL)
    {
        iret = udp_bind(self->pcb, IP_ADDR_ANY, *port);
        LOG_MSG("ret of udp_bind: %d for port: %d\n", iret, *port);
        udp_recv(self->pcb, (udp_recv_fn)self->receive_msg_handler_, self);
    }

    else
    {
        iret = -1;
    }

    return iret;

}/* init_comm */

/*----------------------------------------------------------------------------*/
/* Function : send_msg */
/*----------------------------------------------------------------------------*/
/*!
  @brief

  @details

  @pre

  @param unsigned char *msg

  @param int size

  @return int

*/
PRIVATE int send_msg(udp_connection_s_ptr_t self, const unsigned char *msg, uint32_t *size)
{

    /* Local Variables */
    int iret = -1;
    struct pbuf *p;
    struct ip_addr ipgroup;
    /* Validation */

    /* Algorithm */
//    LOG_MSG("Allocating lwip pbuff of size: %d\n",*size);
    p = pbuf_alloc(PBUF_TRANSPORT, *size, PBUF_RAM);

    if (p == NULL)
    {
        LOG_MSG("pbuf_alloc failed\n");
    }
    else
    {
        uint32_t ip[4] = {0};

        sscanf(NON_RT_NODE_IP_ADDR, "%u.%u.%u.%u", &ip[3], &ip[2], &ip[1], &ip[0]);

        IP4_ADDR(&ipgroup, ip[3], ip[2], ip[1], ip[0]);

        memcpy(p->payload, msg, *size);
        iret = udp_sendto(self->pcb, p, &ipgroup, self->port);

        if (iret != 0)
        {
            LOG_MSG("Udp send to error : %d", iret);
        }

        pbuf_free(p);
        p = NULL;

//        LOG_MSG("UDP send success. Sent %d bytes\n", *size);
    }

    return iret;

}/* send_msg */


/*----------------------------------------------------------------------------*/
/* Function : receive_msg */
/*----------------------------------------------------------------------------*/
/*!
  @brief

  @details

  @pre

  @param void *arg

  @param struct udp_pcb *pcb

  @param struct pbuf *p

  @param struct ip_addr *addr

  @param u16_t port

  @return void

*/
PRIVATE void receive_msg(void *arg, struct udp_pcb *pcb, struct pbuf *p, struct ip_addr *addr, uint16_t port)
{

    /* Local Variables */
    int i = 0;
    uint16_t pkt_length;
    char *packet = NULL;
    udp_connection_s_t udp_conn = *((udp_connection_s_ptr_t)(arg));
    udp_connection_s_ptr_t self = &udp_conn;

    /* Validation */

    /* Algorithm */
    if (p != NULL)
    {

        packet = (p->payload);
        pkt_length = (p->len);

        LOG_MSG("Received msg on UDP port:%d, pkt len: %d  self: %x\n", self->port, pkt_length, self);

        for (i = 0; i <  pkt_length; i++)
        {
            self->recv_buffer->write_byte_handler_(self->recv_buffer, ((char *) (packet + i)));
        }

        LOG_MSG("Received data written to buffer\n");

        self->data_avail_to_read = 1;

        /*Set the events of data receptions based on the interfaces*/
        if (self->port == 5102) {
            LOG_MSG("Debug: Setting flag for data reception\n");
            ifaces_data_reception_events.sub_events_s.non_rt_interface = 1;
            LOG_MSG("Debug: Set flag for data reception\n");
        }


        pbuf_free(p);
    }

    else
    {
        LOG_MSG("Null Packet\n");
    }

}/* receive_msg */


/*----------------------------------------------------------------------------*/
/* Function : connect */
/*----------------------------------------------------------------------------*/
/*!
  @brief

  @details

  @pre

  @param void

  @param

  @return int32_t

*/
PRIVATE int32_t connect(udp_connection_s_ptr_t self) {

    /* Local Variables */
    int32_t err_t;

    /* Validation */
    /* Algorithm */

    err_t = udp_connect(self->pcb, IP_ADDR_ANY, self->port);

    if(0 == err_t)
    {
        self->connection_status = true;
        LOG_MSG("UDP connection established on port: %d", self->port);

    }

    else
    {
        self->connection_status = false;
        LOG_MSG("UDP Connection error :{%d}\n",err_t);
    }
    return err_t;
} /* connect */
