# Hello World Example
#
# Welcome to the OpenMV IDE! Click on the green run arrow button below to run the script!
#import helloword,time,my_sensor,motor
import time,my_sensor,motor

my_sensor.qmi8658_init()
my_sensor.qmc6310_init()
#my_sensor.compass_calibrate()
#my_sensor.send_data()
#motor.run("500\n")
#motor.recevied_cmd()
while(True):
    
    #time.sleep_ms(2000)

    #helloword.print_hello_word()
    #print(my_sensor.get_roll())
    #print(my_sensor.get_acceleration(9))
    #print(my_sensor.get_gyroscope('y'))
    #print(my_sensor.get_acc_gyro_angle())
    #print(my_sensor.get_mag_data())
    #print("hello")
    #print(my_sensor.get_rotation())
    
    #print(my_sensor.get_roll())
    #print(my_sensor.get_pitch())
    #print(my_sensor.get_yaw())
    #print(my_sensor.get_acceleration("x"))
    #print(my_sensor.get_gyroscope('y'))
    #my_sensor.get_gyroscope('y')
    #my_sensor.get_gyroscope(9)
    #print(my_sensor.get_gyroscope("t"))
    #print(my_sensor.get_rotation())
    #print(my_sensor.compass_heading())
    #my_sensor.get_mag_data()
    #my_sensor.gyro_calibrate()
    #if (my_sensor.button() ==1):
    #    print("button is press")
    #my_sensor.button()
    #my_sensor.send_data()
    #print("G2 M1 E0")
    #motor.motor_cmd("G17 M1 P100")
    #motor.left_motor("100")
    #motor.right_motor("100")
    #motor.change_device_id(12)
    #motor.motor_cmd("G128 M1 R200 G100 B200 L60")
    motor.forward("700")
    time.sleep_ms(5000)
    motor.backward("700")
    time.sleep_ms(5000)
    #motor.recevied_cmd()
