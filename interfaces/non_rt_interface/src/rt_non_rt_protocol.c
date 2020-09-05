//
// Created by inderjeet on 3/8/20.
//

#include <protocol/protocol.h>
#include <robot/robot_state.h>
#include <containers/circular_buffer.h>
#include <non_rt_interface/rt_non_rt_protocol.h>
#include <global.h>

static non_rt_parser_state_e_t parser_state = HEADER;

PRIVATE error_t add_header(char *msg);

PRIVATE error_t parse_data(circular_buffer_s_ptr_t msg_buffer, robot_state_ptr_t robot_state, non_rt_events_u_ptr_t non_rt_events);

PRIVATE error_t pack_data(robot_state_ptr_t robot_state, char *msg, int32_t *msg_len, int32_t *packet_id);
PRIVATE error_t calculate_data_length(circular_buffer_s_ptr_t msg_buffer, int32_t *buffer_length);


protocol_s_ptr_t rt_non_rt_protocol_s_init() {

    protocol_s_ptr_t __object = NULL;

    CRITICAL_SECTION_START();
    __object = (protocol_s_ptr_t) malloc(sizeof(protocol_s_t));
    CRITICAL_SECTION_END();


    if(__object != NULL) {

//        __object->init_handler_ = (func_handler_1_args_t)&init;
        __object->parse_data_handler_ = (func_handler_3_args_t)&parse_data;
        __object->pack_data_handler_ = (func_handler_4_args_t)&pack_data;
        LOG_MSG("RT --> NonRT Protocol constructed\n");

    }

    else {
        LOG_MSG("Malloc returned NULL\n");
    }

    return __object;
}


PRIVATE error_t add_header(char *msg) {

    error_t error_no = 0;

    uint8_t header = 0xFD;

    memcpy((uint8_t *)msg, (uint8_t *)&header, sizeof(uint8_t));
    memcpy((uint8_t *)(msg + 1), (uint8_t *)&header, sizeof(uint8_t));

    return error_no;
}


PRIVATE error_t pack_data(robot_state_ptr_t robot_state, char *msg, int32_t *msg_len, int32_t *packet_id) {
    error_t error_no = 0;
    uint32_t index = 2;
    int32_t checksum = 0;

    add_header(msg);

    /*Add Length */
    memcpy((msg+index), msg_len, sizeof(int32_t));
    index += sizeof(int32_t);

    /*Add Packet ID */
    memcpy((msg+index), packet_id, sizeof(int32_t));
    index += sizeof(int32_t);


    switch(*packet_id) {

        case ODOMETRY_DATA:
//            LOG_MSG("DEBUG: Copying odometry data to the msg buffer\n");
            memcpy((msg + index), (odometry_s_t *)&robot_state->odometry, sizeof(odometry_s_t));
            index += sizeof(odometry_s_t);
//            LOG_MSG("DEBUG: Copy Complete\n");
            break;

        case IMU_DATA:
//            LOG_MSG("DEBUG: Copying IMU data to the msg buffer\n");
            memcpy((msg + index), &robot_state->imu, sizeof(imu_s_t));
            index += sizeof(imu_s_t);
//            LOG_MSG("DEBUG: Copy Complete\n");
            break;

        case BEZIER_DATA:
//            LOG_MSG("DEBUG: Copying Bezier data to the msg buffer\n");
            memcpy((msg + index), (void *)&robot_state->bezier_path_, sizeof(bezier_path_s_t));
            index += sizeof(bezier_path_s_t);
//            LOG_MSG("DEBUG: Copy Complete\n");
            break;
        
        case DESTINATION_DATA:
            memcpy((msg + index), (void *)&robot_state->destination_data_, sizeof(destination_data_s_t));
            index += sizeof(destination_data_s_t);
            break;

        default:
            error_no = -1;
            LOG_MSG("Wrong packed id");
    }

    memcpy((msg+index), &checksum, sizeof(int32_t));
    index += sizeof(int32_t);

    return error_no;
}

PRIVATE error_t parse_data(circular_buffer_s_ptr_t msg_buffer, robot_state_ptr_t robot_state, non_rt_events_u_ptr_t non_rt_events) {

    error_t error_no = 0;
    int32_t packet_length = 0;
    static int32_t packet_id = 0;
    static int32_t buffer_length;
    int32_t length_size = 0;
    int32_t packet_id_size = 0;


    LOG_MSG("DEBUG: Parsing NonRT Data\n");


    while(msg_buffer->rd_idx != msg_buffer->wr_idx)
    {
        uint8_t byte;

        switch(parser_state) {

            case HEADER:
                calculate_data_length(msg_buffer, &buffer_length);

                msg_buffer->read_byte_handler_(msg_buffer, &byte);
                if (0xFD == byte) {
                    msg_buffer->read_byte_handler_(msg_buffer, &byte);
                    if (0xFD == byte) {
                        LOG_MSG("DEBUG: Found header. Switching to Length\n");
                        parser_state = LENGTH;
                    }
                }

                break;

            case LENGTH:
                calculate_data_length(msg_buffer, &buffer_length);
                length_size = sizeof(int32_t);
                LOG_MSG("DEBUG: length_size: %d\n", length_size);
                if (buffer_length >= sizeof(int32_t)) {
                    memcpy(&packet_length, &msg_buffer->data[msg_buffer->rd_idx], sizeof(int32_t));
                    msg_buffer->increment_buffer_handler_(msg_buffer, &length_size);
                    LOG_MSG("DEBUG: packet length: %d. Switching to packet id\n", packet_length);
                    parser_state = PACKET_ID;
                }
                break;

            case PACKET_ID:
                calculate_data_length(msg_buffer, &buffer_length);
                packet_id_size = sizeof(int32_t);
                LOG_MSG("DEBUG: packet_id_size: %d\n", packet_id_size);
                if (buffer_length >= sizeof(int32_t)) {

                    memcpy(&packet_id, &msg_buffer->data[msg_buffer->rd_idx], sizeof(int32_t));
                    msg_buffer->increment_buffer_handler_(msg_buffer, &packet_id_size);
                    LOG_MSG("DEBUG: packet id: %x . Switching to payload\n", packet_id);
                    parser_state = PAYLOAD;
                }
                break;

            case PAYLOAD:
                calculate_data_length(msg_buffer, &buffer_length);
                LOG_MSG("Debug: payload length: %d\n", buffer_length);


                switch ((non_rt_rx_packed_ids_e_t)packet_id) {

                    case ENGAGE_EMERGENCY:
                        non_rt_events->sub_event_s.engage_emergency = 1;
                        LOG_MSG("Command --> ENGAGE Emergency\n");
                        parser_state = CHECKSUM;
                        break;

                    case RELEASE_EMERGENCY:
                        robot_state->emergency_state = EMERGENCY_RELEASED;
                        non_rt_events->sub_event_s.release_emergency = 1;
                        LOG_MSG("Command --> DISENGAGE Emergency\n");
                           parser_state = CHECKSUM;
                        break;

                    case CMD_VEL:
                        if (buffer_length >= sizeof(velocity_s_t)) {
                            int32_t cmd_vel_size = sizeof(velocity_s_t);
                            memcpy(&robot_state->cmd_vel, &msg_buffer->data[msg_buffer->rd_idx], sizeof(velocity_s_t));
                            msg_buffer->increment_buffer_handler_(msg_buffer, &cmd_vel_size);
                            non_rt_events->sub_event_s.cmd_vel = 1;
                            LOG_MSG("Cmd Vel: Linear: %d cm/s, Angular: %d rad * 100/s\n", (int)(robot_state->cmd_vel.linear.x * 100), (int)(robot_state->cmd_vel.angular.z * 100));
                            parser_state = CHECKSUM;

                        }
                        break;

                    case RESET_ODOMETRY:
                        non_rt_events->sub_event_s.reset_odom = 1;
                        LOG_MSG("!!!!!  Resetting Odometry !!!!!!!\n");
                        parser_state = CHECKSUM;
                        break;

                    default:
                        LOG_MSG("Unknown packet id: {%d} received from Non-RT Node\n", packet_id);
                        parser_state = HEADER;
                }
                break;

            case CHECKSUM:
                LOG_MSG("Debug: Checksum parser state\n");
                calculate_data_length(msg_buffer, &buffer_length);

                if (buffer_length >= sizeof(int32_t)) {
                    parser_state = HEADER;
                }
                break;

            default:
                LOG_MSG("ERROR: Default Parser State\n");
        }

    }

    return error_no;

}


PRIVATE error_t calculate_data_length(circular_buffer_s_ptr_t msg_buffer, int32_t * buffer_length) {

    error_t error_no = 0;

    if (msg_buffer->rd_idx <= msg_buffer->wr_idx) {

        *buffer_length = msg_buffer->wr_idx - msg_buffer->rd_idx + 1;
    }

    else {
        *buffer_length = (msg_buffer->max_size - msg_buffer->rd_idx) + msg_buffer->wr_idx;
    }

    return error_no;
}
