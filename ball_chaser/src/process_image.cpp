#include "ros/ros.h"
#include "ball_chaser/DriveToTarget.h"
#include <sensor_msgs/Image.h>

// Define a global client that can request sevices
ros::ServiceClient client;

// This function calls the command_robot service to drive the robot in the specifict direction
void drive_robot(float lin_x, float ang_z){
	// TODO: Request a service and pass the velocities to it to drive the robot
	ROS_INFO_STREAM("Driving the robot!");
	ball_chaser::DriveToTarget srv;
	srv.request.linear_x = lin_x;
	srv.request.angular_z = ang_z;

	// Call the drive_bot service and pass the requestet linear and angular velocities
	if (!client.call(srv))
		ROS_ERROR("Failed to call service command_robot");
}
// This callback function continuously executes and reads the image data
void process_image_callback(const sensor_msgs::Image img)
{
	bool white_pixel_found = false;
	int white_pixel_location;
	int white_pixel = 255;
	int offset;

	for (int i = 0; i < img.height; i++)
	{
		offset = i * img.step;

		for (int j = 0; j < img.step; j += 3)
		{
			
			if ((img.data[j+offset] == 255) && (img.data[j+1+offset] == 255) && (img.data[j+2+offset] == 255))
			{
				white_pixel_found = true;
				ROS_INFO_STREAM("FOUND\n");
				white_pixel_location = j/3;
				
			}			

		}

		if (white_pixel_found == true) 
			break;
	}
	
	if (white_pixel_found == true) 
	{
		if (white_pixel_location <= img.width/3)
		{
			ROS_INFO_STREAM("LEFT\n");
	    	drive_robot(0, 0.3);
		}


	    else if ((img.width/3 < white_pixel_location) && (white_pixel_location <= (2/3) * img.width))
	    {
	    	ROS_INFO_STREAM("MIDDLE\n");
	        drive_robot(0.5, 0);
	    }

	    else //((2/3) * img.width < white_pixel_location <= img.width)
	    {
	    	ROS_INFO_STREAM("RIGHT\n");
	        drive_robot(0, -0.3);
	    }
	}

	if (white_pixel_found == false)
	{
		ROS_INFO_STREAM("NO\n");
		drive_robot(0,0);
	}

}

int main(int argc, char** argv)
{
	// Initizlize the process_image node and create a handle to it
	ros::init(argc, argv, "process_image");
	ros::NodeHandle n;

	// Define a client service capable of requesting service from command_robot
	client = n.serviceClient<ball_chaser::DriveToTarget> ("/ball_chaser/command_robot");

	// Subscribe to /camera/rgb/image_raw topic to read the image data inside 
	// the process_image_callback function
	ros::Subscriber sub1 = n.subscribe("/camera/rgb/image_raw", 10, process_image_callback);


	// Handle ROS communication events
	ros::spin();

	return 0;

}
