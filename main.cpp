/* mbed Microcontroller Library
 * Copyright (c) 2019 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
 */

#include "mbed.h"


#define B0 D7
#define B1 D8
#define B2 D9
#define LED_1 D10


#define READING_RATE 200ms
#define DISPLAY_RATE 800ms
#define MAX_PARKING 10

Mutex mutex;
int parking = 0;

struct Buttons {
    DigitalIn entry;
    DigitalIn exit;
};

void Carpark(Buttons* buttons)
{
    ThisThread::flags_wait_any_for(0x1, 100s, false);
    while (true) {
        int is_entry = buttons->entry.read();
        int is_exit = buttons->exit.read();
        
        if (is_entry == 1 && is_exit == 1)
            continue;
        if (is_entry == 0) {
            mutex.lock();
            parking == 10 ? : parking++;
            mutex.unlock();
        }
        if (is_exit == 0) {
            mutex.lock();
            parking == 0 ? : parking--;
            mutex.unlock();
        }
        ThisThread::sleep_for(READING_RATE);
    }
}


void Display()
{
    ThisThread::flags_wait_any_for(0x2, 100s, false);
    while (true) {
        mutex.lock();
        printf("Currently parked car: %d/10\n", parking);
        mutex.unlock();
        ThisThread::sleep_for(DISPLAY_RATE);
    }
}

int main()
{
    // Initialise the digital pin LED1 as an output
    // DigitalOut led(LED1);
    DigitalIn b0(B0);
    DigitalIn b1(B1);
    DigitalIn b2(B2);
    DigitalOut led(LED_1);
    Buttons buttons = {b0, b1};

    Thread worker_carpark;
    Thread worker_display;
    worker_carpark.start(callback(Carpark, &buttons));
    worker_display.start(callback(Display));

    while (true) {
        // led = !led;
        int is_start = b2.read();
        if (is_start == 0) {
            printf("Parking is now started\n");
            worker_carpark.flags_set(0x1);
            worker_display.flags_set(0x2);
            worker_carpark.join();
            worker_display.join();
        }
        printf("The parking is closed now\n");
        ThisThread::sleep_for(DISPLAY_RATE);
    }
}
