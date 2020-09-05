#ifndef NON_RT_INTERFACE_H_
#define NON_RT_INTERFACE_H_

#include <butler_imu.h>
#include <interface.h>
#include <non_rt_interface/rt_non_rt_protocol.h>
#include <robot/robot_state.h>



typedef struct non_rt_interface_s_ {

  interface_s_ptr_t interface;
  robot_kinematics_s_ptr_t robot_kinematics;
  robot_state_t robot_state;
  non_rt_events_u non_rt_events;

  func_handler_1_args_t init_handler_;
  func_handler_1_args_t send_bezier_path_handler_;
  func_handler_1_args_t send_destination_handler_;
  func_handler_1_args_t send_odometry_data_handler_;
  func_handler_1_args_t parse_non_rt_data_handler_;
  func_handler_1_args_t handle_emergency_handler_;
  func_handler_1_args_t send_imu_data_handler_;

}non_rt_interface_s_t;

typedef struct non_rt_interface_s_ * non_rt_interface_s_ptr_t;


/* Constructor*/
non_rt_interface_s_ptr_t non_rt_interface_s_init();


extern volatile non_rt_events_u non_rt_events;


#endif /* NON_RT_INTERFACE_H_*/
