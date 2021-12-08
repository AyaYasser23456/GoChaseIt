#include "ros/ros.h"
#include "ball_chaser/DriveToTarget.h"
#include <sensor_msgs/Image.h>

// Define a global client that can request services
ros::ServiceClient client;

// This function calls the command_robot service to drive the robot in the specified direction
void drive_robot(float lin_x, float ang_z)
{
    // Request a service and pass the velocities to it to drive the robot
    ball_chaser::DriveToTarget srv;
    srv.request.linear_x= lin_x;
    srv.request.angular_z= ang_z;
    if(!client.call(srv)){
	ROS_ERROR("Failed to call service command_robot");
    }
}


// This callback function continuously executes and reads the image data
void process_image_callback(const sensor_msgs::Image img)
{

    int white_pixel = 255;
    float pixel_step;
    float pixel_height;
    bool find_pixel= false;

    // Loop through each pixel in the image and check if there's a bright white one
    // Then, identify if this pixel falls in the left, mid, or right side of the image
    // Depending on the white ball position, call the drive_bot function and pass velocities to it
    // Request a stop when there's no white ball seen by the camera
    for (int i=0;i<img.height*img.step;i+=3) {
        if ((img.data[i]==white_pixel)&&(img.data[i+1]==white_pixel)&&(img.data[i+2]==white_pixel)){
            find_pixel=true;
            pixel_step=(i/img.step);
            pixel_height=(i/img.height);
   // use pexel height and width to navigate the robot towards white ball
   ROS_INFO("image height:%1.2f,image width:%1.2f, pixel height:%1.2f, pixel step:%1.2f", (float)img.height,(float)img.width, (float)pixel_height, (float)pixel_step);
	    break;    
        }
    }


    // If the ball isn't appear at image stop or the robot reach to the ball (don't move)
      if (find_pixel==false){  
      	ROS_INFO_STREAM("Stop");
	drive_robot(0.0,0.0);
     }

      else{

	    // If the ball in the left side of image move to left
	    if(pixel_height==1200){   

		ROS_INFO_STREAM("Moving to left");
		drive_robot(0.0,0.08);
	    }

	    // If the ball in the mid side of image move forward
	    else if(pixel_height==1201){   
	 
		ROS_INFO_STREAM("Moving forward");
		drive_robot(0.1,0.0);
	    }

	    else if(pixel_height==1202){
		ROS_INFO_STREAM("Moving to right");
		drive_robot(0.0,-0.1);
		}
	}	
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

