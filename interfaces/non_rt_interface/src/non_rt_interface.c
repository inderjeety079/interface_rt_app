//
// Created by inderjeet on 2/10/20.
//
#include <non_rt_interface/non_rt_interface.h>
#include <delay.h>
#include <global.h>
#include <nav_params.h>
#include "butler_navigation.h"
//#include <udp_connection.h>

PRIVATE error_t init(non_rt_interface_s_ptr_t self);
PRIVATE error_t get_bezier_control_points(non_rt_interface_s_ptr_t self);
PRIVATE error_t send_bezier_path(non_rt_interface_s_ptr_t self);
PRIVATE error_t sample_odometry_data(non_rt_interface_s_ptr_t self);
PRIVATE error_t send_odometry_data(non_rt_interface_s_ptr_t self);
PRIVATE error_t parse_non_rt_data(non_rt_interface_s_ptr_t self);
PRIVATE error_t handle_emergency(non_rt_interface_s_ptr_t self);
PRIVATE error_t sample_imu_data(non_rt_interface_s_ptr_t self);
PRIVATE error_t send_imu_data(non_rt_interface_s_ptr_t self);
PRIVATE error_t send_destination_data(non_rt_interface_s_ptr_t self);
PRIVATE error_t sample_destination_data(non_rt_interface_s_ptr_t self);


udp_connection_s_ptr_t connection_handle;

volatile non_rt_events_u non_rt_events = {0};

non_rt_interface_s_ptr_t non_rt_interface_s_init() {

    non_rt_interface_s_ptr_t __object = NULL;
    uint32_t port = 5102;
    udp_connection_s_ptr_t non_rt_udp_conn = udp_connection_s_init(&port);

    if (NULL != non_rt_udp_conn)
    {
        protocol_s_ptr_t protocol_ptr = rt_non_rt_protocol_s_init();

        if (NULL != protocol_ptr)
        {
            interface_s_ptr_t interface =  udp_interface_s_init(non_rt_udp_conn, protocol_ptr);
            if (NULL != interface)
            {
                robot_kinematics_s_ptr_t robot_kinematics = robot_kinematics_s_init();
                if(NULL != robot_kinematics) {
                    LOG_MSG("DEBUG: Allocating Memory for NonRT Interface\n");

                    CRITICAL_SECTION_START();
                    __object = (non_rt_interface_s_ptr_t) malloc(sizeof(non_rt_interface_s_t));
                    CRITICAL_SECTION_END();
                    LOG_MSG("DEBUG: Allocated Memory for NonRT Interface\n");
                    if(__object != NULL)
                    {
                        __object->robot_state.odometry.pose.x = 0.0f;
                        __object->robot_state.odometry.pose.y = 0.0f;
                        __object->robot_state.odometry.pose.theta = 0.0f;
                        __object->robot_state.odometry.twist.linear = 0.0f;
                        __object->robot_state.odometry.twist.angular = 0.0f;
                        __object->robot_state.odometry_feedback_rate_ = 0.025;
                        __object->robot_state.reset_odometry = false;
                        __object->non_rt_events.event = 0;

                        __object->interface = interface;
                        __object->robot_kinematics = robot_kinematics;
                        __object->init_handler_ = (func_handler_1_args_t)&init;
                        __object->send_bezier_path_handler_ = (func_handler_1_args_t)&send_bezier_path;
                        __object->send_odometry_data_handler_ = (func_handler_1_args_t)&send_odometry_data;
                        __object->parse_non_rt_data_handler_ = (func_handler_1_args_t)&parse_non_rt_data;
                        __object->handle_emergency_handler_ = (func_handler_1_args_t)&handle_emergency;
                        __object->send_imu_data_handler_ = (func_handler_1_args_t)&send_imu_data;
                        __object->send_destination_handler_ = (func_handler_1_args_t)&send_destination_data;

                        connection_handle = __object->interface->connection_handle;
                        LOG_MSG("NonRT Interface constructed\n");
                    }

                    else
                    {
                        LOG_MSG("Malloc returned NULL\n");
                    }
                }

                else
                {
                    LOG_MSG("ERROR: Robot Kinematics constructor returned NULL\n");
                }
            }

            else
            {
                LOG_MSG("ERROR: Null ptr returned from UDP interface constructor\n");
            }
        }

        else
        {

            LOG_MSG("ERROR: Null ptr returned from rt non_rt protocol constructor\n");
        }
    }

    else


    {
        LOG_MSG("ERROR: Null ptr returned from udp connection constructor\n");

    }

    return __object;
}


PRIVATE error_t init(non_rt_interface_s_ptr_t self)
{
  error_t error_no = 0;

  int status = connection_handle->init_handler_(connection_handle, &connection_handle->ip, &connection_handle->port);
  error_no = status;
  return error_no;
}

PRIVATE error_t get_bezier_control_points(non_rt_interface_s_ptr_t self)
{
  CRITICAL_SECTION_START();

//  self->bezier_path_.control_pts[0] = ;
//  memcpy(&self->bezier_path_, current_bezier_ctrl_point, sizeof(bezier_path_s_t));

  CRITICAL_SECTION_END();

  return 0;

}


PRIVATE error_t send_bezier_path(non_rt_interface_s_ptr_t self) {

    error_t status;
    uint32_t packet_length = 0;

    /* fill the packet and send it to obstacle node for collision checking*/

    const int32_t min_packet_size = 14;

    int32_t payload_size = sizeof(bezier_path_s_t);

    packet_length = (min_packet_size + payload_size);
    int32_t packet_id =  (non_rt_tx_packed_ids_e_t)BEZIER_DATA;

    char packet[packet_length]; // extra 2 bytes are for packet id and checksum
    memset((char *) packet, 0, (packet_length));

    get_bezier_control_points(self);
    self->interface->protocol->pack_data_handler_(&self->robot_state, packet, &packet_length, &packet_id);

    LOG_MSG("Sending %d bytes to NonRT node\n", packet_length);

    status = connection_handle->send_msg_handler_(connection_handle, packet, &packet_length);

    return status;

}


PRIVATE error_t sample_odometry_data(non_rt_interface_s_ptr_t self)
{
    CRITICAL_SECTION_START();

    self->robot_state.odometry.timestamp = milliseconds;
    self->robot_state.odometry.pose.x = rtpc.world.y;
    self->robot_state.odometry.pose.y = -rtpc.world.x;
    self->robot_state.odometry.pose.theta = -rtpc.world.theta;
    self->robot_state.odometry.twist.linear = ((signed_current_speed.left_wheel + signed_current_speed.right_wheel) / 2.0f);
    self->robot_state.odometry.twist.angular = atan(-(signed_current_speed.right_wheel - signed_current_speed.left_wheel)  / (butler_info.hardware_params.wheel_params.wheel_base));

    CRITICAL_SECTION_END();

    return 0;

}

PRIVATE error_t send_odometry_data(non_rt_interface_s_ptr_t self) {

    error_t status;
    uint32_t index;
//    LOG_MSG("DEBUG: Sending odometry data to obstacle node\n");


    // 2 bytes header + 4 bytes packet_len + 4 bytes packed_id + 4 bytes checksum
    const int32_t min_packet_size = 14;
    int32_t payload_size = sizeof(odometry_s_t);
    int32_t packet_length = (min_packet_size + payload_size);
    int32_t packet_id =  (non_rt_tx_packed_ids_e_t)ODOMETRY_DATA;

    char packet[packet_length + 2];
    memset((char *) packet, 0, (packet_length + 2));

    sample_odometry_data(self);

    self->interface->protocol->pack_data_handler_(&self->robot_state, packet, &packet_length, &packet_id);

//    LOG_MSG("DEBUG: Sending %d bytes to obstacle node\n", packet_length);
    status = connection_handle->send_msg_handler_(connection_handle, packet, &packet_length);

    if(0 != status) {
        LOG_MSG("ERROR: Sending odometry data failed. Error reason: {%d}\n",status);
    }

    return status;
}



PRIVATE error_t handle_emergency(non_rt_interface_s_ptr_t self) {

    error_t error_no = 0;


    return error_no;

}

PRIVATE error_t send_imu_data(non_rt_interface_s_ptr_t self) {

    error_t status;
    uint32_t index;

    const int32_t min_packet_size = 14;
    int32_t payload_size =  sizeof(imu_s_t);
    int32_t packet_length = (min_packet_size + payload_size);
    int32_t packet_id = (non_rt_tx_packed_ids_e_t)IMU_DATA;

    char packet[packet_length + 2];
    memset((char *) packet, 0, (packet_length + 2));

    sample_imu_data(self);

    self->interface->protocol->pack_data_handler_(&self->robot_state, packet, &packet_length, &packet_id);

    //    LOG_MSG("Sending %d bytes to obstacle node\n", packet_length);
    status = connection_handle->send_msg_handler_(connection_handle, packet, &packet_length);

    if(0 != status) {
        LOG_MSG("Sending imu data failed. Error reason: {%d}\n",status);
    }

    return status;

}

PRIVATE error_t sample_imu_data(non_rt_interface_s_ptr_t self) {

    error_t error_no = 0;

    CRITICAL_SECTION_START();
    self->robot_state.imu.timestamp = milliseconds;
    memcpy((float *) (&self->robot_state.imu.orientation), (float *)(imu.euler.array), sizeof(euler_s_t));
    memcpy((float *)&self->robot_state.imu.angular_velocity, (float *)(imu.ang_speed.array), sizeof(vector_3_s_t));
    memcpy((float *) (&self->robot_state.imu.linear_acceleration), (float *)(imu.acc.array), sizeof(vector_3_s_t));

    CRITICAL_SECTION_END();

    return error_no;

}

PRIVATE error_t sample_destination_data(non_rt_interface_s_ptr_t self) {
        
        
    error_t error_no = 0;
    float destination_orientation = (float)(move_params.move_cmd.final_orientation_bot);
    uint8_t temp_idx = move_data_wr_idx;


    CRITICAL_SECTION_START();

    self->robot_state.destination_data_.timestamp = milliseconds;
    // memcpy((char *) (self->robot_state.destination_data_.dm_str), (char *)(move_data[move_data_wr_idx].dm_string), 8);
    memcpy((float *)(&self->robot_state.destination_data_.orientation), (float *)(&destination_orientation), sizeof(float));

    CRITICAL_SECTION_END();

    return error_no;
}

PRIVATE error_t send_destination_data(non_rt_interface_s_ptr_t self) {
    
    error_t status;
    uint32_t index;

    const int32_t min_packet_size = 14;
    int32_t payload_size =  sizeof(destination_data_s_t);
    int32_t packet_length = (min_packet_size + payload_size);
    int32_t packet_id = (non_rt_tx_packed_ids_e_t)DESTINATION_DATA;

    char packet[packet_length + 2];
    memset((char *) packet, 0, (packet_length + 2));

    sample_destination_data(self);
    

    self->interface->protocol->pack_data_handler_(&self->robot_state, packet, &packet_length, &packet_id);

    //    LOG_MSG("Sending %d bytes to obstacle node\n", packet_length);
    status = connection_handle->send_msg_handler_(connection_handle, packet, &packet_length);

    if(0 != status) {
        LOG_MSG("Sending destination data failed. Error reason: {%d}\n",status);
    }

    else {
        LOG_MSG("Send Destination DM %s to NonRT\n",self->robot_state.destination_data_.dm_str);
    }

    return status;


}

PRIVATE error_t parse_non_rt_data(non_rt_interface_s_ptr_t self) {
    error_t error_no = 0;

//    self->interface->protocol->parse_data_handler_(connection_handle->recv_buffer, &self->robot_state, &self->non_rt_events);
    self->interface->protocol->parse_data_handler_(connection_handle->recv_buffer, &self->robot_state, (non_rt_events_u_ptr_t)&non_rt_events);

    return error_no;
}



