#include <ros/ros.h>
#include <geometry_msgs/PointStamped.h>
#include <tf/transform_listener.h> //needed for creating a TransformListener object
//A TransformListener object automatically subscribes to the transform message topic over ROS and manages all transform data coming in over the wire


//this node will use the transform broadcasted by tf_broadcaster.cpp to take a point in the "base_laser" frame and transform it to a point in the "base_link" frame


void transformPoint(const tf::TransformListener& listener){
//We'll create a function that, given a TransformListener, takes a point in the "base_laser" frame and transforms it to the "base_link" frame. This function will serve as a callback for the ros::Timer created in the main() of our program and will fire every second.

  //we'll create a point in the base_laser frame that we'd like to transform to the base_link frame
  geometry_msgs::PointStamped laser_point; //the "Stamped" just means that it includes a header, allowing us to associate both a timestamp and frame_id with the message
  laser_point.header.frame_id = "base_laser"; //for the frame_id field of the header, we'll set it to be "base_laser" because we're creating a point in the "base_laser" frame

  //we'll just use the most recent transform available for our simple example
  laser_point.header.stamp = ros::Time();

  //just an arbitrary point in space
  laser_point.point.x = 0.0;
  laser_point.point.y = 0.18;
  laser_point.point.z = 1.05;

  try{
    geometry_msgs::PointStamped base_point;
    listener.transformPoint("base_link", laser_point, base_point);

    ROS_INFO("base_laser: (%.2f, %.2f. %.2f) -----> base_link: (%.2f, %.2f, %.2f) at time %.2f",
        laser_point.point.x, laser_point.point.y, laser_point.point.z,
        base_point.point.x, base_point.point.y, base_point.point.z, base_point.header.stamp.toSec());
  }
  catch(tf::TransformException& ex){
    ROS_ERROR("Received an exception trying to transform a point from \"base_laser\" to \"base_link\": %s", ex.what());
  }
}

int main(int argc, char** argv){
  ros::init(argc, argv, "robot_tf_listener");
  ros::NodeHandle n;

  tf::TransformListener listener(ros::Duration(10));

  //we'll transform a point once every second
  ros::Timer timer = n.createTimer(ros::Duration(1.0), boost::bind(&transformPoint, boost::ref(listener)));

  ros::spin();

}
