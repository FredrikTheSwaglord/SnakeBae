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

volatile bool run;

VideoStream depth;

void* camera_thread(void*){

  VideoFrameRef frame;
  int changedStreamDummy;
  VideoStream* pStream = &depth;
  sensor_msgs::Image image;
  image.is_bigendian=0;

  int count = 0;
  struct timeval previous_time;
  gettimeofday(&previous_time, 0);
  uint64_t last_stamp = 0;

  while(run){

  	bool new_frame = false;
  	uint64_t current_stamp = 0;
    openni::OpenNI::waitForAnyStream(&pStream, 1, &changedStreamDummy);
    depth.readFrame(&frame);
    image.header.stamp=ros::Time::now();
    if(!frame.isValid()) break;
    current_stamp=frame.getTimestamp();

    if (current_stamp-last_stamp>5000){
        count++;
        new_frame = true;
    }
    last_stamp = current_stamp;

  
    image.header.seq = count;
    
    image.header.frame_id="depth";
    image.height=frame.getHeight();
    image.width=frame.getWidth();
    image.encoding="mono16";
    image.step=frame.getWidth()*2;
    image.data.resize(image.step*image.height);
    memcpy((char*)(&image.data[0]),frame.getData(),image.height*image.width*2);
    pub_depth.publish(image);

  }

}





int main(int argc, char **argv){

	ros::init(argc, argv, "depthpub");
	ros::NodeHandle n;

    Status rc = OpenNI::initialize();
    if (rc != STATUS_OK){

      printf("Initialize failed\n%s\n", OpenNI::getExtendedError());
      return 1;
    }

    Device device;

    rc = device.open(ANY_DEVICE);

    if (rc != STATUS_OK){

      printf("Couldn't open device\n%s\n", OpenNI::getExtendedError());
      return 2;

    }

    if (device.getSensorInfo(SENSOR_DEPTH) != NULL){

      rc = depth.create(device, SENSOR_DEPTH);
      
      if (rc != STATUS_OK){

        printf("Couldn't create depth stream\n%s\n", OpenNI::getExtendedError());
        fflush(stdout);
        return 3;
      }
    }

    pub_depth = n.advertise<sensor_msgs::Image>("camera/depth/image_raw", 1);
    rc = depth.setVideoMode(device.getSensorInfo(SENSOR_DEPTH)->getSupportedVideoModes()[0]);
    depth.setMirroringEnabled(false);
    rc = depth.start();  

  	run = true;
  	pthread_t runner;
  	pthread_create(&runner, 0, camera_thread, 0);
  	ros::spin();
  	void* result;
  	run =false;

  	pthread_join(runner, &result);

  	depth.stop();
  	depth.destroy();
 	device.close();
  	OpenNI::shutdown(); 
  	return 0;


}