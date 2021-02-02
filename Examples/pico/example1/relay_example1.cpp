/**
 * Copyright (c) 2020 SparkFun Electronics
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include <string.h>

#include "pico/stdlib.h"
#include "qwiic_relay.h"

Qwiic_Relay myRelay;


int main() {

     // setup stdio. Output device is controlled in CMakeLists.txt file
    stdio_init_all();

	printf("Starting relay Test\n\n");

    // Init the system 

    if(!myRelay.begin()){
    	printf("Error starting the qwiic relay device. Aboarting\n");
  		return 0;
    }

    printf("Firmware Version: %f\n", myRelay.singleRelayVersion());

    while(1){

        printf("Starting Test Interation...\n");

        myRelay.turnRelayOn();
        printf("Relay On\n");
        sleep_ms(500);

        myRelay.turnRelayOff();
        printf("Relay Off\n");
        sleep_ms(500);

        myRelay.toggleRelay();        
        printf("Toggle 1\n");
        sleep_ms(500);     

        myRelay.toggleRelay();        
        printf("Toggle 2\n");
        sleep_ms(500);    

        printf("Relay State: %s\n", myRelay.getState() ? "On" : "Off");

        sleep_ms(1000);
    } 
    return 0;
}
