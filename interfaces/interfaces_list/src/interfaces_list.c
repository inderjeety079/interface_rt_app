//
// Created by inderjeet on 22/06/20.
//

#include <interfaces_list/interfaces_list.h>
#include "butler_navigation.h"
#include <timer_init.h>



/*----------------------------------------------------------------------------*/
/* Function : initialize_non_rt_interface */
/*----------------------------------------------------------------------------*/
/*!
  @brief This function initialized the non rt interface.

  @details

  @param void

  @param

  @return void

*/


void initialize_non_rt_interface() {


    LOG_MSG("INFO: Initializing NonRT Interface\n");

    non_rt_interface = non_rt_interface_s_init();
    udp_connection_s_ptr_t non_rt_conn_handle = non_rt_interface->interface->connection_handle;
    int non_rt_interface_conn_status = -1;
    int conn_attempt_count = 0;


    if (non_rt_interface != NULL) {
        LOG_MSG("INFO: NRT Interface constructed");

        non_rt_interface->init_handler_(non_rt_interface);

        while (non_rt_interface_conn_status && conn_attempt_count < 20) {
            LOG_MSG("INFO: Trying to Connect to NonRT Node... \n");


            non_rt_interface_conn_status = non_rt_conn_handle->connect_handler_(non_rt_conn_handle);

            if(0 != non_rt_interface_conn_status) {
            delay(1000);
            }
            conn_attempt_count++;

        }

        if (0 == non_rt_interface_conn_status) {

            non_rt_interface_initialized = true;
            LOG_MSG(" NonRT Node Available. Continue with app ....\n");
        }

    } else {
        non_rt_interface_initialized = false;
        LOG_MSG("ERROR :NonRT Interface init returned NUll. Will Not share data with NRT\n");
    }

}

/*----------------------------------------------------------------------------*/
/* Function : process_non_rt_data */
/*----------------------------------------------------------------------------*/
/*!
  @brief This function processes the non rt events and executes the task
  associated with the event. Execute this function within a synchronous/asynch-
  ronous spinner.

  @details

  @param void

  @param

  @return void

*/
void process_non_rt_data() {

    if (true == non_rt_interface_initialized) {

        if(1 == ifaces_data_reception_events.sub_events_s.non_rt_interface) {
//            LOG_MSG("Non RT Message Processing Started \n");
            non_rt_interface->parse_non_rt_data_handler_(non_rt_interface);
            ifaces_data_reception_events.sub_events_s.non_rt_interface = 0;
//            LOG_MSG("Non RT Message Processing Complete \n");
        }


        /* Check for Non RT Events */
        if (non_rt_events.event) {

            if (1 == non_rt_events.sub_event_s.engage_emergency) {

                non_rt_events.sub_event_s.engage_emergency = 0;
//                LOG_MSG("DEBUG: Event: NonRT CMD, SubEvent: engage_emergency\n");
            }

            if (1 == non_rt_events.sub_event_s.cmd_vel) {

//                LOG_MSG("DEBUG: Event: NonRT CMD, SubEvent: cmd_vel\n");
                memcpy((struct velocity_s_t *)&non_rt_interface->robot_kinematics->cmd_vel, (struct velocity_s_t *)&non_rt_interface->robot_state.cmd_vel, sizeof(velocity_s_t));

                non_rt_interface->robot_kinematics->set_wheels_speed_handler_(non_rt_interface->robot_kinematics);
//                LOG_MSG("DEBUG: Command : left_wheel: %d cm/s , right_wheel: %d cm/s \n",
//                        (int)(non_rt_interface->robot_kinematics->diff_drive.left_wheel_speed[0] * 100),
//                        (int)(non_rt_interface->robot_kinematics->diff_drive.right_wheel_speed[0]) * 100);


                set_wheel_speed_cmps(
                        (double []){non_rt_interface->robot_kinematics->diff_drive.left_wheel_speed[0] * 100,
                                    non_rt_interface->robot_kinematics->diff_drive.right_wheel_speed[0] * 100},
                        BUTLER_WHEEL_MOTION_NRT_MOVE);
//                LOG_MSG("Speed sent to MD\n");


                non_rt_events.sub_event_s.cmd_vel = 0;


            }

            if (1 == non_rt_events.sub_event_s.release_emergency) {

                non_rt_events.sub_event_s.release_emergency = 0;
                LOG_MSG("DEBUG: Event: NonRT CMD, SubEvent: release_emergency\n");
            }

            if (1 == non_rt_events.sub_event_s.reset_odom) {

                rtpc.world.x = 0.0f;
                rtpc.world.y = 0.0;
                rtpc.world.theta = 0.0;
                // rtpc.bot_act.x = 0.0;
                // rtpc.bot_act.y = 0.0;
                // rtpc.bot_act.theta = 0.0;
                rtpc.time_stamp = 0;
                non_rt_events.sub_event_s.reset_odom = 0;
                LOG_MSG("DEBUG:  Event: NonRT CMD, SubEvent: Reset Odometry\n");

            }

        }

    }
} /* process_non_rt_data */


