//
// Created by inderjeet on 3/17/20.
//

#ifndef ROBOT_KINEMATICS_H_
#define ROBOT_KINEMATICS_H_

#include <global_definition.h>
#include <containers/circular_buffer.h>

#define NUM_WHEELS 2

typedef struct vector_3_
{
  float x;
  float y;
  float z;
} vector_3_s_t;

typedef struct header_s_ {
  uint32_t seq_id;
  uint32_t timestamp;
}header_s_t;


typedef struct velocity_s_
{
  vector_3_s_t linear;
  vector_3_s_t angular;
} velocity_s_t;

typedef struct velocity_stamped_s_ {
  header_s_t header;
  velocity_s_t velocity;
} velocity_stamped_s_t;

typedef struct diff_drive_s_ {

  /// Wheel separation, wrt the midpoint of the wheel width:
  float wheel_separation_;
  /// Wheel radius (assuming it's the same for the left and right wheels):
  float wheel_radius_;
  /// Wheel separation and radius calibration multipliers:
  float wheel_separation_multiplier_;
  float left_wheel_radius_multiplier_;
  float right_wheel_radius_multiplier_;

  float left_wheel_speed[NUM_WHEELS / 2];
  float right_wheel_speed[NUM_WHEELS / 2];


}diff_drive_s_t;

typedef struct diff_drive_s_t * diff_drive_s_ptr_t;



typedef struct robot_kinematics_s_ {

  velocity_s_t cmd_vel;
  circular_buffer_s_ptr_t cmd_vel_buffer;
  diff_drive_s_t diff_drive;
  func_handler_1_args_t set_wheels_speed_handler_;
} robot_kinematics_s_t;

typedef struct robot_kinematics_s_ * robot_kinematics_s_ptr_t;

robot_kinematics_s_ptr_t robot_kinematics_s_init();

#endif //ROBOT_KINEMATICS_H_
