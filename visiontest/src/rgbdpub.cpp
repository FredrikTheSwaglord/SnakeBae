#include <ros/ros.h>
#include <sensor_msgs/Image.h>
#include <sensor_msgs/CameraInfo.h>
#include <sensor_msgs/distortion_models.h>
#include <stdio.h>
#include <iostream>
#include <OpenNI.h>
#include <stdlib.h> 
#include <string>

using namespace openni;
using namespace std;

//IMAGE
ros::Publisher pub_depth;// = n.advertise<sensor_msgs::Image>("/"+topic+"/depth/image_raw", 1);
ros::Publisher pub_rgb;// = n.advertise<sensor_msgs::Image>("/"+topic+"/rgb/image_raw", 1);
//CAMERA INFO	