#include <ros/ros.h>
#include <sensor_msgs/Image.h>
#include <sensor_msgs/CameraInfo.h>
#include <sensor_msgs/distortion_models.h>
#include <stdio.h>
#include <iostream>
#include <OpenNI.h>
#include <stdlib.h> 
#include <string>

#include "OniSampleUtilities.h"

#define SAMPLE_READ_WAIT_TIMEOUT 2000 //2000ms

using namespace openni;
using namespace std;

//IMAGE
ros::Publisher pub_depth;// = n.advertise<sensor_msgs::Image>("/"+topic+"/depth/image_raw", 1);

int main(int argc, char **argv)
{
  Status rc = OpenNI::initialize();
  if (rc != STATUS_OK)
  {
    printf("Initialize failed\n%s\n", OpenNI::getExtendedError());
    return 1;
  }

  Device device;
  rc = device.open(ANY_DEVICE);
  if (rc != STATUS_OK)
  {
    printf("Couldn't open device\n%s\n", OpenNI::getExtendedError());
    return 2;
  }

  VideoStream depth;

  if (device.getSensorInfo(SENSOR_DEPTH) != NULL)
  {
    rc = depth.create(device, SENSOR_DEPTH);
    if (rc != STATUS_OK)
    {
      printf("Couldn't create depth stream\n%s\n", OpenNI::getExtendedError());
      return 3;
    }
  }

  rc = depth.start();
  if (rc != STATUS_OK)
  {
    printf("Couldn't start the depth stream\n%s\n", OpenNI::getExtendedError());
    return 4;
  }

  VideoFrameRef frame;

  ros::init(argc, argv, "depther");
  ros::NodeHandle n;

  ros::Publisher pub_depth = n.advertise<sensor_msgs::Image>("/camera/depth/image_raw", 1);
   

  while (!wasKeyboardHit())
  {
    int changedStreamDummy;
    VideoStream* pStream = &depth;
    rc = OpenNI::waitForAnyStream(&pStream, 1, &changedStreamDummy, SAMPLE_READ_WAIT_TIMEOUT);
    if (rc != STATUS_OK)
    {
      printf("Wait failed! (timeout is %d ms)\n%s\n", SAMPLE_READ_WAIT_TIMEOUT, OpenNI::getExtendedError());
      continue;
    }

    rc = depth.readFrame(&frame);
    if (rc != STATUS_OK)
    {
      printf("Read failed!\n%s\n", OpenNI::getExtendedError());
      continue;
    }

    if (frame.getVideoMode().getPixelFormat() != PIXEL_FORMAT_DEPTH_1_MM && frame.getVideoMode().getPixelFormat() != PIXEL_FORMAT_DEPTH_100_UM)
    {
      printf("Unexpected frame format\n");
      continue;
    }

    DepthPixel* pDepth = (DepthPixel*)frame.getData();

    int middleIndex = (frame.getHeight()+1)*frame.getWidth()/2;

    printf("[%08llu] %8d\n", (long long)frame.getTimestamp(), pDepth[middleIndex]);
    pub_depth.publish(depth);

  }

  depth.stop();
  depth.destroy();
  device.close();
  OpenNI::shutdown();
  depth.

  return 0;
}
