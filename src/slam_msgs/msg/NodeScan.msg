# One graph node's lidar scan, in the robot BASE frame at the node's time
# (lidar mounting offset applied, deskewed). Published once per node.
std_msgs/Header header
int32 id
float32[] x
float32[] y
