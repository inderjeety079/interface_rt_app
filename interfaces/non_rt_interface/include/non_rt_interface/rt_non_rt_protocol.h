//
// Created by inderjeet on 3/9/20.
//

#ifndef RT_NON_RT_PROTOCOL_H_
#define RT_NON_RT_PROTOCOL_H_



typedef enum non_rt_tx_packed_ids_ {

  ODOMETRY_DATA = 1,
  IMU_DATA = 2,
  BEZIER_DATA = 3,
  IPU_DATA = 4,
  DESTINATION_DATA = 5,
  NUM_TX_PACKED_IDS
}non_rt_tx_packed_ids_e_t;

typedef enum non_rt_rx_packed_ids_e_ {
  ENGAGE_EMERGENCY = 1,
  RELEASE_EMERGENCY = 2,
  CMD_VEL = 3,
  RESET_ODOMETRY = 4,
  NUM_RX_PACKED_IDS
}non_rt_rx_packed_ids_e_t;

typedef enum non_rt_parser_state_e_ {

  HEADER = 1,
  LENGTH = 2,
  PACKET_ID = 3,
  PAYLOAD = 4,
  CHECKSUM = 5,
  NUM_RX_PARSER_STATES
}non_rt_parser_state_e_t;


typedef union non_rt_events_ {

  struct sub_event_ {
    uint32_t engage_emergency : 1;
    uint32_t release_emergency : 1;
    uint32_t cmd_vel : 1;
    uint32_t reset_odom : 1;
    uint32_t reserved : 28;
  } sub_event_s;

  uint32_t event;

}non_rt_events_u;

typedef union non_rt_events_ * non_rt_events_u_ptr_t;

protocol_s_ptr_t rt_non_rt_protocol_s_init();


#endif //RT_NON_RT_PROTOCOL_H_
