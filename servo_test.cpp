/*
Author: Milán Szalai

This program initializes the PCA9685 controller, 
then moves the servo motors on the 4 channels in sequence 
by 90 degrees, then stops them. Each channel has a 1 second 
wait between movements.*/

#include "PCA9685.cpp"

int main() {
    int i2cBus = wiringPiI2CSetup(PCA9685_ADDRESS);
    PCA9685 pca9685(i2cBus);

    // Move on all 4 channels
    int onStep = 150;  // Active time of the servo after switching off
    int offStep = 600; // Waiting time after switch-off

    for (int channel = 0; channel < 4; ++channel) {
        // Moving the servo motor 90 degrees
        pca9685.set_pwm(channel, 0, onStep);
        usleep(1000000);   // Wait 1 second
        pca9685.set_pwm(channel, 0, offStep);
        usleep(1000000);   // Wait 1 second
    }

    return 0;
}
