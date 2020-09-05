//
// Created by inderjeet on 3/17/20.
//

#include <global_definition.h>
#include <robot/robot_kinematics.h>
#include <global.h>

void set_wheels_speed(robot_kinematics_s_ptr_t self);

robot_kinematics_s_ptr_t robot_kinematics_s_init() {

    robot_kinematics_s_ptr_t __object = NULL;

//    circular_buffer_s_ptr_t circular_buffer = ;


    CRITICAL_SECTION_START();
    __object = (robot_kinematics_s_ptr_t) malloc(sizeof(robot_kinematics_s_t));
    CRITICAL_SECTION_END();

    if (__object != NULL)
    {
        __object->diff_drive.wheel_separation_ = 0.576;
        __object->diff_drive.wheel_radius_ = 0.1016;
        __object->diff_drive.wheel_separation_multiplier_ = 1.0;
        __object->diff_drive.left_wheel_radius_multiplier_ = 1.0;
        __object->diff_drive.right_wheel_radius_multiplier_ = 1.0;
        __object->set_wheels_speed_handler_ = (func_handler_1_args_t)&set_wheels_speed;
        memset(__object->diff_drive.left_wheel_speed, 0, (NUM_WHEELS/2) * sizeof(float));
        memset(__object->diff_drive.right_wheel_speed, 0, (NUM_WHEELS/2) * sizeof(float));
        LOG_MSG("robot kinematics object constructed\n");

    }

    else
    {
        LOG_MSG("Malloc returned NULL\n");

    }

    return __object;

}

void set_wheels_speed(robot_kinematics_s_ptr_t self) {

    if (self->cmd_vel.angular.z < -1.5 || self->cmd_vel.angular.z > 1.5) {
        self->cmd_vel.angular.z = 0.0;
        LOG_MSG("ERROR: Angular speed limits breach: %6.2f rad/s\n", (float)self->cmd_vel.angular.z);
    }

    if (self->cmd_vel.linear.x < -2.5 || self->cmd_vel.linear.x > 2.5) {
        self->cmd_vel.linear.x = 0.0;
        LOG_MSG("ERROR: Linear speed limits breach: %6.2f m/s\n", (float)self->cmd_vel.angular.z);
    }

    const float vel_left = (self->cmd_vel.linear.x + ((self->cmd_vel.angular.z * self->diff_drive.wheel_separation_) / 2.0));
    const float vel_right = (self->cmd_vel.linear.x - ((self->cmd_vel.angular.z * self->diff_drive.wheel_separation_) / 2.0));
//    LOG_MSG("DEBUG: vel_left : %6.2f m/s vel_right: %6.2f m/s \n", (float) (vel_left), (
//    float) (vel_right));

    self->diff_drive.left_wheel_speed[0] = vel_left;
    self->diff_drive.right_wheel_speed[0] = vel_right;

}
