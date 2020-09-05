//
// Created by inderjeet on 3/9/20.
//

#ifndef ROBOT_STATE_H_
#define ROBOT_STATE_H_

#include <global_definition.h>
#include "global.h"
#include <robot/robot_kinematics.h>

typedef enum emergency_states_e_ {
    EMERGENCY_ENGAGED = 1,
    EMERGENCY_RELEASED = 2,
    MAX_EMERGENCY_STATES
}emergency_states_e_t;

typedef struct euler_
{
  float roll;
  float pitch;
  float yaw;
} euler_s_t;


typedef struct imu_
{
  timestamp_t timestamp;
  euler_s_t orientation;
  vector_3_s_t angular_velocity;
  vector_3_s_t linear_acceleration;
}imu_s_t;

typedef struct two_d_pose_s_
{
  float x;
  float y;
  float theta;
} two_d_pose_s_t;

typedef struct twist_s_
{
  float linear;
  float angular;
} twist_s_t;

typedef struct odometry_
{
  timestamp_t timestamp;
  two_d_pose_s_t pose;
  twist_s_t twist;
}odometry_s_t;

typedef struct bezier_path_s_
{
  control_pts_s_t control_pts[4];
}bezier_path_s_t;

typedef struct destination_data_s_ {
  uint32_t timestamp;
  char dm_str[8];
  float orientation;
}destination_data_s_t;


typedef struct robot_state_ {

  odometry_s_t odometry;
  bezier_path_s_t bezier_path_;
  destination_data_s_t destination_data_;
  imu_s_t imu;
  velocity_s_t cmd_vel;
  velocity_s_t meas_vel;
  float odometry_feedback_rate_; // in secs
  emergency_states_e_t emergency_state;
  bool cmd_vel_available;
  bool reset_odometry;
} robot_state_t;

typedef struct robot_state_ * robot_state_ptr_t;

#endif //ROBOT_STATE_H_
