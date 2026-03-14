/*
 * application.c
 *
 *  Created on: Feb 22, 2026
 *      Author: Dell
 */

#include "application.h"
#include "sensor.h"
#include <stdio.h>

static app_state_t app_state = INIT;
static app_state_t previous_state = COM_ERROR;

static BKITSensorData my_sensor_data = BKITSensorData_init_default;
static BKITSensorData rcv_sensor_data = BKITSensorData_init_default;

static uint64_t last_slave_rcv_timestamp = 0;
static uint64_t snd_sequence = 0;
static uint8_t master_send_done = 0;
static uint8_t master_send_ok = 0;
static uint8_t slave_send_done = 0;
static uint8_t master_process_done = 0;
static uint8_t master_panel_drawn = 0;
static uint8_t slave_panel_drawn = 0;
static uint16_t slave_id_input = 2;

static void set_flow_timer(void)
{
    setTimer(FLOW_TIMER, FLOW_TIME);
}

void bkit_com_fsm(void)
{
    // Execute one-time entry action when state changes
    if (app_state != previous_state) {
        app_sub_fsm(app_state);
        previous_state = app_state;
    }

    switch (app_state) {
        case INIT:
            lcd_clear(BLACK);

            if (!bkit_app_init()) {
                lcd_show_string(0, 10, "BKIT COM: ERR", RED, BLACK, 16, 0);
                app_state = COM_ERROR;
                break;
            }

            lcd_show_string_center(0, 10, "BKIT COM", CYAN, BLACK, 24, 0);
            app_state = SELECT_ROLE;
            break;

        case SELECT_ROLE:
            select_role();
            // Select role
            // button_scan();

            // if (button_count[0] == 1) {
            //     lcd_clear(BLACK);
            //     my_sensor_data.sensor_id = 1;
            //     set_flow_timer();
            //     master_panel_drawn = 0;

            //     app_state = MASTER_IDLE;
            // } else if (button_count[1] == 1) {
            //     lcd_clear(BLACK);
            //     slave_id_input = 2;
            //     app_state = SLAVE_MENU;
            // }
            break;

        case SLAVE_MENU:
            slave_menu();
            // button_scan();

            // if (button_count[2] == 1) {
            //     if (slave_id_input > 1) {
            //         slave_id_input--;
            //     }
            //     lcd_show_int_num(130, 60, slave_id_input, 4, CYAN, BLACK, 16);
            // }

            // if (button_count[3] == 1) {
            //     if (slave_id_input < 9999) {
            //         slave_id_input++;
            //     }
            //     lcd_show_int_num(130, 60, slave_id_input, 4, CYAN, BLACK, 16);
            // }

            // if (button_count[1] == 1) {
            // 	lcd_clear(BLACK);
            //     my_sensor_data.sensor_id = slave_id_input;
            //     slave_panel_drawn = 0;
            //     update_slave_sensor();
            //     set_flow_timer();
            //     app_state = SLAVE_RECEIVE;
            // }

            // if (button_count[0] == 1) {
            // 	lcd_clear(BLACK);
            //     app_state = SELECT_ROLE;
            // }
            break;

        case MASTER_IDLE:
            master_idle();
            // button_scan();

            // if (button_count[4] == 1) {
            //     app_state = MASTER_SEND;
            // } else if (isTimerExpired(FLOW_TIMER)) {
            //     set_flow_timer();
            //     app_state = MASTER_RECEIVE;
            // }
            break;

        case MASTER_SEND:
            master_send();
            break;
            //     if (bkit_send_message(&my_sensor_data)) {
            //         master_send_ok = 1;
            //     } else {
            //         master_send_ok = 0;
            //     }

            //     master_send_done = 1;
            //     setTimer(STATUS_HOLD_TIMER, STATUS_HOLD_TIME);
            // } else if (isTimerExpired(STATUS_HOLD_TIMER)) {
            //     if (master_send_ok) {
            //         app_state = MASTER_RECEIVE;
            //     } else {
            //         app_state = MASTER_IDLE;
            //     }
            // }
            // break;

        case MASTER_RECEIVE:
            master_receive();
            // button_scan();
            // if (button_count[4] == 1) {
            //     app_state = MASTER_SEND;
            //     break;
            // }

            // if (bkit_receive_message(&rcv_sensor_data)) {
            //     app_state = MASTER_PROCESS;
            // } else if (isTimerExpired(MASTER_RECV_WAIT_TIMER)) {
            //     app_state = MASTER_IDLE;
            // }
            break;

        case MASTER_PROCESS:
            master_process();
            // Process received frame, show values
            // if (!master_process_done) {
            //     show_master_received_data();

            //     master_process_done = 1;
            //     setTimer(STATUS_HOLD_TIMER, STATUS_HOLD_TIME);
            // } else if (isTimerExpired(STATUS_HOLD_TIMER)) {
            //     app_state = MASTER_IDLE;
            // }
            break;

        case SLAVE_RECEIVE:
            slave_receive();
            // // Update local slave sensor values on screen every 5 seconds
            // if (isTimerExpired(FLOW_TIMER)) {
            //     update_slave_sensor();
            //     show_slave_sensor_data();
            //     set_flow_timer();
            // }

            // // Move to send only when a new request is received and data is ready
            // if (bkit_receive_message(&rcv_sensor_data)) {
            //     if (rcv_sensor_data.timestamp != last_slave_rcv_timestamp) {
            //         last_slave_rcv_timestamp = rcv_sensor_data.timestamp;
            //         app_state = SLAVE_SEND;
            //     }
            // }
            break;

        case SLAVE_SEND:
            slave_send();
            // if (!slave_send_done) {
            //     my_sensor_data.timestamp = rcv_sensor_data.timestamp;

            //     if (bkit_send_message(&my_sensor_data)) {
            //         lcd_show_string(0, 300, "SEND: SUCCESS       ", GREEN, BLACK, 10, 0);
            //     } else {
            //         lcd_show_string(0, 300, "SEND: FAILED        ", RED, BLACK, 10, 0);
            //     }

            //     slave_send_done = 1;
            //     setTimer(STATUS_HOLD_TIMER, STATUS_HOLD_TIME);
            // } else if (isTimerExpired(STATUS_HOLD_TIMER)) {
            //     app_state = SLAVE_RECEIVE;
            // }
            break;

        case COM_ERROR:

        	break;

        default:
            app_state = INIT;
            break;
    }
}

void app_sub_fsm(app_state_t state)
{
    switch (state) {
        case SELECT_ROLE:
            lcd_clear(BLACK);
            lcd_show_string(70, 10, "BKIT COM", CYAN, BLACK, 24, 0);
            lcd_show_string(20, 70, "[0] MASTER", YELLOW, BLACK, 16, 0);
            lcd_show_string(20, 95, "[1] SLAVE ", YELLOW, BLACK, 16, 0);
            break;

        case SLAVE_MENU:
            lcd_clear(BLACK);
            lcd_show_string(10, 20, "SLAVE MENU", MAGENTA, BLACK, 24, 0);
            lcd_show_string(10, 60, "Set Slave ID:", WHITE, BLACK, 16, 0);
            lcd_show_int_num(130, 60, slave_id_input, 4, CYAN, BLACK, 16);
            lcd_show_string(10, 90, "[0] Back", YELLOW, BLACK, 16, 0);
            lcd_show_string(10, 110, "[1] OK", YELLOW, BLACK, 16, 0);
            lcd_show_string(10, 130, "[2] Decrease ID", YELLOW, BLACK, 16, 0);
            lcd_show_string(10, 150, "[3] Increase ID", YELLOW, BLACK, 16, 0);
            break;

        case MASTER_IDLE:
            lcd_show_string(10, 10, "ROLE: MASTER", CYAN, BLACK, 16, 0);
            lcd_show_string(60, 300, "[4] REQUEST", WHITE, BLACK, 16, 0);
            if (!master_panel_drawn) {
                draw_master_data_panel();
                master_panel_drawn = 1;
            }
            set_flow_timer();
            break;

        case MASTER_SEND:
            master_send_done = 0;
            master_send_ok = 0;
            break;

        case MASTER_RECEIVE:
            setTimer(MASTER_RECV_WAIT_TIMER, MASTER_RECV_WAIT_TIME);
            break;

        case MASTER_PROCESS:
            master_process_done = 0;
            break;

        case SLAVE_RECEIVE:
            lcd_show_string(10, 10, "ROLE: SLAVE", MAGENTA, BLACK, 16, 0);
            if (!slave_panel_drawn) {
                show_slave_sensor_data();
                slave_panel_drawn = 1;
            }
            set_flow_timer();
            break;

        case SLAVE_SEND:
            slave_send_done = 0;
            break;

        default:
            break;
    }
}

void init();
void select_role() {
    button_scan();

    if (button_count[0] == 1) {
        lcd_clear(BLACK);
        my_sensor_data.sensor_id = 1;
        set_flow_timer();
        master_panel_drawn = 0;

        app_state = MASTER_IDLE;
    } else if (button_count[1] == 1) {
        lcd_clear(BLACK);
        slave_id_input = 2;
        app_state = SLAVE_MENU;
    }
}

void slave_menu() {
    button_scan();

    if (button_count[2] == 1) {
        if (slave_id_input > 1) {
            slave_id_input--;
        }
        lcd_show_int_num(130, 60, slave_id_input, 4, CYAN, BLACK, 16);
    }

    if (button_count[3] == 1) {
        if (slave_id_input < 9999) {
            slave_id_input++;
        }
        lcd_show_int_num(130, 60, slave_id_input, 4, CYAN, BLACK, 16);
    }

    if (button_count[1] == 1) {
        lcd_clear(BLACK);
        my_sensor_data.sensor_id = slave_id_input;
        last_slave_rcv_timestamp = 0;
        slave_panel_drawn = 0;
        update_slave_sensor();
        set_flow_timer();
        app_state = SLAVE_RECEIVE;
    }

    if (button_count[0] == 1) {
        app_state = SELECT_ROLE;
    }
}
void master_idle() {
    lcd_show_string(10, 10, "ROLE: MASTER", CYAN, BLACK, 16, 0);
    lcd_show_string(60, 300, "[4] REQUEST", WHITE, BLACK, 16, 0);
    if (!master_panel_drawn) {
        draw_master_data_panel();
        master_panel_drawn = 1;
    }

    button_scan();

    if (button_count[4] == 1) {
        app_state = MASTER_SEND;
    } else if (isTimerExpired(FLOW_TIMER)) {
        set_flow_timer();
        app_state = MASTER_RECEIVE;
    }
}
void master_send() {
    if (!master_send_done) {
        snd_sequence++;
        my_sensor_data.timestamp = snd_sequence;

        if (bkit_send_message(&my_sensor_data)) {
            master_send_ok = 1;
        } else {
            master_send_ok = 0;
        }

        master_send_done = 1;
        setTimer(STATUS_HOLD_TIMER, STATUS_HOLD_TIME);
    } else if (isTimerExpired(STATUS_HOLD_TIMER)) {
        if (master_send_ok) {
            app_state = MASTER_RECEIVE;
        } else {
            app_state = MASTER_IDLE;
        }
    }
}

void master_receive() {
    button_scan();
    if (button_count[4] == 1) {
        app_state = MASTER_SEND;
    }

    if (bkit_receive_message(&rcv_sensor_data)) {
        app_state = MASTER_PROCESS;
    } else if (isTimerExpired(MASTER_RECV_WAIT_TIMER)) {
        app_state = MASTER_IDLE;
    }
}
void master_process() {
    if (!master_process_done) {
        show_master_received_data();

        master_process_done = 1;
        setTimer(STATUS_HOLD_TIMER, STATUS_HOLD_TIME);
    } else if (isTimerExpired(STATUS_HOLD_TIMER)) {
        app_state = MASTER_IDLE;
    }
}

void slave_receive() {

    lcd_show_string(10, 10, "ROLE: SLAVE", MAGENTA, BLACK, 16, 0);
    if (!slave_panel_drawn) {
        show_slave_sensor_data();
        slave_panel_drawn = 1;
    }

    // Update local slave sensor values on screen every 5 seconds
    if (isTimerExpired(FLOW_TIMER)) {
        update_slave_sensor();
        show_slave_sensor_data();
        set_flow_timer();
    }

    // Move to send only when a new request is received and data is ready
    if (bkit_receive_message(&rcv_sensor_data)) {
        if (rcv_sensor_data.timestamp != last_slave_rcv_timestamp) {
            last_slave_rcv_timestamp = rcv_sensor_data.timestamp;
            app_state = SLAVE_SEND;
        }
    }
}
void slave_send() {
    if (!slave_send_done) {
        my_sensor_data.timestamp = rcv_sensor_data.timestamp;

        if (bkit_send_message(&my_sensor_data)) {
            lcd_show_string(0, 300, "SEND: SUCCESS       ", GREEN, BLACK, 10, 0);
        } else {
            lcd_show_string(0, 300, "SEND: FAILED        ", RED, BLACK, 10, 0);
        }

        slave_send_done = 1;
        setTimer(STATUS_HOLD_TIMER, STATUS_HOLD_TIME);
    } else if (isTimerExpired(STATUS_HOLD_TIMER)) {
        app_state = SLAVE_RECEIVE;
    }
}
void com_error() {}

void draw_master_data_panel(void)
{
    lcd_show_string(10,  80, "Slave ID     :", GREEN, BLACK, 16, 0);
    lcd_show_string(130,  80, "----", GREEN, BLACK, 16, 0);
    lcd_show_string(10, 100, "Voltage      :", GREEN, BLACK, 16, 0);
    lcd_show_string(130, 100, "----", GREEN, BLACK, 16, 0);
    lcd_show_string(10, 120, "Current      :", GREEN, BLACK, 16, 0);
    lcd_show_string(130, 120, "----", GREEN, BLACK, 16, 0);
    lcd_show_string(10, 140, "Light        :", GREEN, BLACK, 16, 0);
    lcd_show_string(130, 140, "----", GREEN, BLACK, 16, 0);
    lcd_show_string(10, 160, "Potentiometer:", GREEN, BLACK, 16, 0);
    lcd_show_string(130, 160, "----", GREEN, BLACK, 16, 0);
    lcd_show_string(10, 180, "Temperature  :", GREEN, BLACK, 16, 0);
    lcd_show_string(130, 180, "----", GREEN, BLACK, 16, 0);
}

void update_slave_sensor(void)
{
    sensor_read();
    my_sensor_data.light = sensor_get_light();
    my_sensor_data.potentiometer = sensor_get_potentiometer();
    my_sensor_data.voltage = sensor_get_voltage();
    my_sensor_data.current = sensor_get_current();
    my_sensor_data.temperature = sensor_get_temperature();
}

void show_slave_sensor_data() {
	// Show Voltage
	lcd_show_string(10, 100, "Voltage      :", RED, BLACK, 16, 0);
    lcd_show_float_num(130, 100, my_sensor_data.voltage, 4, RED, BLACK, 16);
	// Show Current
	lcd_show_string(10, 120, "Current      :", RED, BLACK, 16, 0);
    lcd_show_float_num(130, 120, my_sensor_data.current, 4, RED, BLACK, 16);
	// Show Light
	lcd_show_string(10, 140, "Light        :", RED, BLACK, 16, 0);
    lcd_show_int_num(130, 140, my_sensor_data.light, 5, RED, BLACK, 16);
	// Show Potentiometer
	lcd_show_string(10, 160, "Potentiometer:", RED, BLACK, 16, 0);
    lcd_show_int_num(130, 160, my_sensor_data.potentiometer, 5, RED, BLACK, 16);
	// Show Temperature
	lcd_show_string(10, 180, "Temperature  :", RED, BLACK, 16, 0);
    lcd_show_float_num(130, 180, my_sensor_data.temperature, 4, RED, BLACK, 16);
}

void show_master_received_data(void)
{
    // Keep the same data area coordinates as show_slave_sensor_data()
    lcd_show_string(10,  80, "Slave ID     :", GREEN, BLACK, 16, 0);
    lcd_show_int_num(130,  80, rcv_sensor_data.sensor_id, 5, GREEN, BLACK, 16);
    // Show Voltage
    lcd_show_string(10, 100, "Voltage      :", GREEN, BLACK, 16, 0);
    lcd_show_float_num(130, 100, rcv_sensor_data.voltage, 4, GREEN, BLACK, 16);
    // Show Current
    lcd_show_string(10, 120, "Current      :", GREEN, BLACK, 16, 0);
    lcd_show_float_num(130, 120, rcv_sensor_data.current, 4, GREEN, BLACK, 16);
    // Show Light
    lcd_show_string(10, 140, "Light        :", GREEN, BLACK, 16, 0);
    lcd_show_int_num(130, 140, rcv_sensor_data.light, 5, GREEN, BLACK, 16);
    // Show Potentiometer
    lcd_show_string(10, 160, "Potentiometer:", GREEN, BLACK, 16, 0);
    lcd_show_int_num(130, 160, rcv_sensor_data.potentiometer, 5, GREEN, BLACK, 16);
    // Show Temperature
    lcd_show_string(10, 180, "Temperature  :", GREEN, BLACK, 16, 0);
    lcd_show_float_num(130, 180, rcv_sensor_data.temperature, 4, GREEN, BLACK, 16);
}


