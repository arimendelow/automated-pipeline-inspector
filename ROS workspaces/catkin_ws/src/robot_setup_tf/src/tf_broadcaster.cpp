#include <ros/ros.h>
#include <tf/transform_broadcaster.h>
//provides an implementation of a tf::TransformBroadcaster to help make the task of publishing transforms easier


//this node will broadcast the base_laser -> base_link transform over ROS


int main(int argc, char** argv){
  ros::init(argc, argv, "robot_tf_publisher");
  ros::NodeHandle n;

  ros::Rate r(100);

  tf::TransformBroadcaster broadcaster;
  //we will use this to send the base_link -> base_laser transform over the wire

  while(n.ok()){
    broadcaster.sendTransform( 
      tf::StampedTransform(
        tf::Transform(tf::Quaternion(0, 0, 0, 1), tf::Vector3(0.0, 0.18, 1.05)),
        ros::Time::now(),"base_link", "base_laser"));

	//requires 5 args:
		//pass in rotation transform, specified by a btQuaternion for any rotation that
 		  //needs to occur between the two coordinate frames
		  //we want to apply no rotation, so pitch/roll/yaw = 0
		//btVector3 for any translation that we'd like to apply.
		  //we create a btVector3 cooresponding to the laser's x offset of ____, y offset
 		  //of 10cm, and z offset of 105cm (in meters).
		//give the transform a timestamp - we just stamp it with ros::Time::now()
		//pass name of child node of the link we're crerating - in this case, "base_laser"

    r.sleep();
  }
}
