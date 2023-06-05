#include <wiringPiI2C.h>
#include <wiringPi.h>
#include <unistd.h>
#include <cmath>

// Default address:
#define PCA9685_ADDRESS 0x40

// Registers/etc:
#define MODE1           0x00
#define MODE2           0x01
#define SUBADR1         0x02
#define SUBADR2         0x03
#define SUBADR3         0x04
#define PRESCALE        0xFE
#define LED0_ON_L       0x06
#define LED0_ON_H       0x07
#define LED0_OFF_L      0x08
#define LED0_OFF_H      0x09
#define ALL_LED_ON_L    0xFA
#define ALL_LED_ON_H    0xFB
#define ALL_LED_OFF_L   0xFC
#define ALL_LED_OFF_H   0xFD

// Bits:
#define RESTART         0x80
#define SLEEP           0x10
#define ALLCALL         0x01
#define INVRT           0x10
#define OUTDRV          0x04

// Channels
#define CHANNEL00       0x00
#define CHANNEL01       0x01
#define CHANNEL02       0x02
#define CHANNEL03       0x03
#define CHANNEL04       0x04
#define CHANNEL05       0x05
#define CHANNEL06       0x06
#define CHANNEL07       0x07
#define CHANNEL08       0x08
#define CHANNEL09       0x09
#define CHANNEL10       0x0A
#define CHANNEL11       0x0B
#define CHANNEL12       0x0C
#define CHANNEL13       0x0D
#define CHANNEL14       0x0E
#define CHANNEL15       0x0F

class PCA9685 {
public:
    PCA9685(int i2cBus, int address = PCA9685_ADDRESS) {
        this->i2cBus = i2cBus;
        this->address = address;
        this->begin();
    }

    void begin() {
        wiringPiI2CWriteReg8(this->i2cBus, MODE2, OUTDRV);
        wiringPiI2CWriteReg8(this->i2cBus, MODE1, ALLCALL);
        usleep(5000);                                    // wait for oscillator
        int mode1 = wiringPiI2CReadReg8(this->i2cBus, MODE1);
        mode1 = mode1 & ~SLEEP;                          // wake up (reset sleep)
        wiringPiI2CWriteReg8(this->i2cBus, MODE1, mode1);
        usleep(5000);                                    // wait for oscillator
    }

    void reset() {
        wiringPiI2CWriteReg8(this->i2cBus, MODE1, RESTART);
        usleep(10000);
    }

    void set_address(int address) {
        this->address = address;
    }

    void set_i2c_bus(int i2cBus) {
        this->i2cBus = i2cBus;
    }

    void set_pwm(int channel, int on, int off) {
        wiringPiI2CWriteReg8(this->i2cBus, LED0_ON_L + 4 * channel, on & 0xFF);
        wiringPiI2CWriteReg8(this->i2cBus, LED0_ON_H + 4 * channel, on >> 8);
        wiringPiI2CWriteReg8(this->i2cBus, LED0_OFF_L + 4 * channel, off & 0xFF);
        wiringPiI2CWriteReg8(this->i2cBus, LED0_OFF_H + 4 * channel, off >> 8);
    }

    void set_all_pwm(int on, int off) {
        wiringPiI2CWriteReg8(this->i2cBus, ALL_LED_ON_L, on & 0xFF);
        wiringPiI2CWriteReg8(this->i2cBus, ALL_LED_ON_H, on >> 8);
        wiringPiI2CWriteReg8(this->i2cBus, ALL_LED_OFF_L, off & 0xFF);
        wiringPiI2CWriteReg8(this->i2cBus, ALL_LED_OFF_H, off >> 8);
    }

    void set_pwm_freq(float freq_hz) {
        float prescaleval = 25000000.0;                    // 25MHz
        prescaleval /= 4096.0;                             // 12-bit
        prescaleval /= freq_hz;
        prescaleval -= 1.0;
        int prescale = static_cast<int>(floor(prescaleval + 0.5));
        int oldmode = wiringPiI2CReadReg8(this->i2cBus, MODE1);
        int newmode = (oldmode & 0x7F) | 0x10;             // sleep
        wiringPiI2CWriteReg8(this->i2cBus, MODE1, newmode); // go to sleep
        wiringPiI2CWriteReg8(this->i2cBus, PRESCALE, prescale);
        wiringPiI2CWriteReg8(this->i2cBus, MODE1, oldmode);
        usleep(5000);
        wiringPiI2CWriteReg8(this->i2cBus, MODE1, oldmode | 0x80);
    }

private:
    int i2cBus;
    int address;
};

int main() {
    int i2cBus = wiringPiI2CSetup(PCA9685_ADDRESS);
    PCA9685 pca9685(i2cBus);
    return 0;
}
