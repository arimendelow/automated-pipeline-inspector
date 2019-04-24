#!/usr/bin/env python
import rospy
from std_msgs.msg import String
from geometry_msgs.msg import Twist
from sensor_msgs.msg import Joy
import serial

def callback(data):
    twist.linear.x = 4*data.axes[3]
    twist.angular.z = 4*data.axes[2]
    pub.publish(twist)

class driver:
    def __init__(self):
        # init ros
        rospy.init_node('car_driver', anonymous=True)

	global pub
        pub = rospy.Publisher('/cmd_vel', Twist)

	global twist
  	twist = Twist()

	rospy.Subscriber("joy", Joy, callback)
        rospy.Subscriber('/cmd_vel', Twist, self.get_cmd_vel)

        self.ser = serial.Serial('/dev/ttyACM5', 115200)
        self.get_arduino_message()

    # get cmd_vel message, and get linear velocity and angular velocity
    def get_cmd_vel(self, data):
        x = twist.linear.x
        angular = twist.angular.z
        self.send_cmd_to_arduino(x, angular)

    # translate x, and angular velocity to PWM signal of each wheels, and send to arduino
    def send_cmd_to_arduino(self, x, angular):
        # calculate right and left wheels' signal
        right = int((x + angular) * 50)
        left = int((x - angular) * 50)
        # format for arduino
        message = "{},{}*".format(right, left)
        print message
        # send by serial 
        self.ser.write(message)

    # receive serial text from arduino and publish it to '/arduino' message
    def get_arduino_message(self):
        pub = rospy.Publisher('arduino', String, queue_size=10)
        r = rospy.Rate(10)
        while not rospy.is_shutdown():
            message = self.ser.readline()
            pub.publish(message)
            r.sleep()

if __name__ == '__main__':
    try:
        d = driver()
    except rospy.ROSInterruptException: 
        pass














