#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>

#define I2C_DEVICE "/dev/i2c-1"
#define MPU6050_ADDRESS 0x68

#define ACCEL_XOUT_H_ADDR 0x3B
#define ACCEL_XOUT_L_ADDR 0x3C
#define ACCEL_YOUT_H_ADDR 0x3D
#define ACCEL_YOUT_L_ADDR 0x3E
#define ACCEL_ZOUT_H_ADDR 0x3F
#define ACCEL_ZOUT_L_ADDR 0x40

#define TEMP_OUT_H_ADDR 0x41
#define TEMP_OUT_L_ADDR 0x42

#define GYRO_XOUT_H_ADDR 0x43
#define GYRO_XOUT_L_ADDR 0x44
#define GYRO_YOUT_H_ADDR 0x45
#define GYRO_YOUT_L_ADDR 0x46
#define GYRO_ZOUT_H_ADDR 0x47
#define GYRO_ZOUT_L_ADDR 0x48

int main() {
    int file;
    char filename[20];
    int16_t data;

    // Open the I2C bus
    sprintf(filename, "%s", I2C_DEVICE);
    if ((file = open(filename, O_RDWR)) < 0) {
        perror("Failed to open the i2c bus");
        return 1;
    }

    // Set the I2C slave address
    if (ioctl(file, I2C_SLAVE, MPU6050_ADDRESS) < 0) {
        perror("Failed to acquire bus access and/or talk to slave");
        return 1;
    }

    // Read accelerometer data
    data = i2c_smbus_read_word_data(file, ACCEL_XOUT_H_ADDR);
    printf("Accel X: %d\n", data);
    data = i2c_smbus_read_word_data(file, ACCEL_YOUT_H_ADDR);
    printf("Accel Y: %d\n", data);
    data = i2c_smbus_read_word_data(file, ACCEL_ZOUT_H_ADDR);
    printf("Accel Z: %d\n", data);

    // Read temperature data
    data = i2c_smbus_read_word_data(file, TEMP_OUT_H_ADDR);
    printf("Temperature: %d\n", data);

    // Read gyroscope data
    data = i2c_smbus_read_word_data(file, GYRO_XOUT_H_ADDR);
    printf("Gyro X: %d\n", data);
    data = i2c_smbus_read_word_data(file, GYRO_YOUT_H_ADDR);
    printf("Gyro Y: %d\n", data);
    data = i2c_smbus_read_word_data(file, GYRO_ZOUT_H_ADDR);
    printf("Gyro Z: %d\n", data);

    // Close the I2C bus
    close(file);

    return 0;
}
