#!/usr/bin/env python3
"""Drive the TurtleBot3 around the pillar in pgs_world (several laps) and publish the Gazebo path.

    ros2 run my_pose_graph_slam drive_loop.py --ros-args -p use_sim_time:=true -p laps:=3

Route (world frame = odom frame, robot spawned at the origin facing +x): along the entry
corridor to (5, 0), then around the pillar block (x 6.3-7.7, y -7.0 to -1.8) through
(5, -7.9), (9.3, -7.9), (9.3, 0) - about 24 m per lap, every corner >= 0.9 m from a wall.
Control uses Gazebo's /odom (near-perfect). /gt_path = that /odom, i.e. the TRUE path for
comparison with /optimized_path and /odom_path (the drifting odometry when noise is on).
"""
import math
import rclpy
from rclpy.node import Node
from geometry_msgs.msg import Twist, PoseStamped
from nav_msgs.msg import Odometry, Path

LOOP = [(5.0, -7.9), (9.3, -7.9), (9.3, 0.0), (5.0, 0.0)]
V, W = 0.2, 0.5            # m/s, rad/s


def wrap(a):
    return math.atan2(math.sin(a), math.cos(a))


class Driver(Node):
    def __init__(self):
        super().__init__("drive_loop")
        laps = self.declare_parameter("laps", 3).value
        self.plan = [(5.0, 0.0)] + LOOP * laps
        self.pose, self.k, self.done = None, 0, False
        self.path = Path(); self.path.header.frame_id = "map"
        self.create_subscription(Odometry, "/odom", self.odom, 20)
        self.cmd = self.create_publisher(Twist, "/cmd_vel", 10)
        self.gt = self.create_publisher(Path, "/gt_path", 10)
        self.create_timer(0.05, self.control)
        self.create_timer(1.0, lambda: self.gt.publish(self.path))
        self.get_logger().info(f"{laps} laps around the pillar, {len(self.plan)} waypoints")

    def odom(self, m):
        q = m.pose.pose.orientation
        self.pose = (m.pose.pose.position.x, m.pose.pose.position.y, 2 * math.atan2(q.z, q.w))
        if not self.path.poses or math.hypot(self.pose[0] - self.path.poses[-1].pose.position.x,
                                             self.pose[1] - self.path.poses[-1].pose.position.y) > 0.05:
            ps = PoseStamped(); ps.header = m.header; ps.header.frame_id = "map"; ps.pose = m.pose.pose
            self.path.poses.append(ps); self.path.header.stamp = m.header.stamp

    def control(self):
        if self.pose is None or self.done:
            return
        t = Twist()
        if self.k >= len(self.plan):
            self.cmd.publish(t); self.done = True
            self.get_logger().info("route finished"); return
        gx, gy = self.plan[self.k]; x, y, th = self.pose
        d, e = math.hypot(gx - x, gy - y), wrap(math.atan2(gy - y, gx - x) - th)
        if d < 0.05:
            self.k += 1
        elif abs(e) > 0.15:
            t.angular.z = max(-W, min(W, 2.0 * e))                    # turn in place
        else:
            t.linear.x = min(V, 0.5 * d + 0.03); t.angular.z = max(-W, min(W, 1.5 * e))
        self.cmd.publish(t)


def main():
    rclpy.init(); n = Driver()
    while rclpy.ok() and not n.done:
        rclpy.spin_once(n, timeout_sec=0.1)
    n.cmd.publish(Twist()); n.destroy_node(); rclpy.shutdown()


if __name__ == "__main__":
    main()
