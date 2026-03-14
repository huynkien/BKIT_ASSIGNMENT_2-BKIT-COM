/*
 * application.h
 *
 *  Created on: Feb 22, 2026
 *      Author: Dell
 */

#ifndef INC_APPLICATION_H_
#define INC_APPLICATION_H_

#include "main.h"
#include "buzzer.h"
#include "lcd.h"
#include "button.h"
#include "led_7seg.h"
#include "bkit_app.h"
#include "software_timer.h"
#include "adc.h"

#define FLOW_TIMER      		0
#define FLOW_TIME  				1000
#define STATUS_HOLD_TIMER  		1
#define STATUS_HOLD_TIME     	1000
#define MASTER_RECV_WAIT_TIMER 	2
#define MASTER_RECV_WAIT_TIME  	1000

typedef enum {
    ROLE_UNASSIGNED = 0,
    ROLE_MASTER,
    ROLE_SLAVE
} app_role_t;

typedef enum {
    INIT = 0,
    SELECT_ROLE,
    SLAVE_MENU,
    MASTER_IDLE,
    MASTER_SEND,
    MASTER_RECEIVE,
    MASTER_PROCESS,
    SLAVE_RECEIVE,
    SLAVE_SEND,
    COM_ERROR
} app_state_t;

// main FSM
void bkit_com_fsm(void);
void app_sub_fsm(app_state_t state);

//State function
void init();
void select_role();
void slave_menu();
void master_idle();
void master_send();
void master_receive();
void master_process();
void slave_receive();
void slave_send();
void com_error();

void draw_master_data_panel(void);
void update_slave_sensor(void);
void show_slave_sensor_data(void);
void show_master_received_data(void);

#endif /* INC_APPLICATION_H_ */
