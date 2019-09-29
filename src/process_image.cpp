#include "ros/ros.h"
#include "ball_chaser/DriveToTarget.h"
#include <sensor_msgs/Image.h>
#include <math.h>

// Define a global client that can request services
ros::ServiceClient client;

enum class DBGDirection { left, forward, right, stop };

// This function calls the command_robot service to drive the robot in the specified direction
void drive_robot(float lin_x, float ang_z)
{
    ROS_INFO_STREAM("Moving the robot");
    ball_chaser::DriveToTarget service;
    service.request.linear_x = lin_x;
    service.request.angular_z = ang_z;
    if (!client.call(service)) {
      ROS_ERROR("Failed to call service command_robot");
    }
}

void move_towards(DBGDirection d)
{
    //ROS_INFO_STREAM("Direction chosen: " + std::to_string(d));
    if (DBGDirection::left == d){
      drive_robot(0.0, 0.5);
    } else if (DBGDirection::forward == d){
      drive_robot(0.5, 0.0);
    } else if (DBGDirection::right == d){
      drive_robot(0.0, -0.5);
    } else if (DBGDirection::stop == d){
      drive_robot(0.0, 0.0);
    }
}


// This callback function continuously executes and reads the image data
void process_image_callback(const sensor_msgs::Image img)
{
    int white_pixel = 255;
    //bool white_seen = false;
    DBGDirection d = DBGDirection::stop;
    float third_width = img.step / 3;
    int third_limit = floor(third_width);
    for (int i = 0; i < img.height * img.step; i++) {
      if (img.data[i] == white_pixel) {
        int xpos = i % img.step;
        if (xpos < third_limit) {
          d = DBGDirection::left;
        } else if (xpos >= third_limit && xpos <= third_limit*2) {
          d = DBGDirection::forward;
        } else {
          d = DBGDirection::right;
          // it should be the same to avoid the middle condition and write
          // explicitly the right condition. as the initial value is forward.
        }
        break;
      }
    }


    move_towards(d);
    // Loop through each pixel in the image and check if there's a bright white one
    // Then, identify if this pixel falls in the left, mid, or right side of the image
    // Depending on the white ball position, call the drive_bot function and pass velocities to it
    // Request a stop when there's no white ball seen by the camera
}

int main(int argc, char** argv)
{
    // Initialize the process_image node and create a handle to it
    ros::init(argc, argv, "process_image");
    ros::NodeHandle n;

    // Define a client service capable of requesting services from command_robot
    client = n.serviceClient<ball_chaser::DriveToTarget>("/ball_chaser/command_robot");

    // Subscribe to /camera/rgb/image_raw topic to read the image data inside the process_image_callback function
    ros::Subscriber sub1 = n.subscribe("/camera/rgb/image_raw", 10, process_image_callback);

    // Handle ROS communication events
    ros::spin();

    return 0;
}
