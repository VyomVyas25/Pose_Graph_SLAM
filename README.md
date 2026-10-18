# Pose Graph SLAM — Built from Scratch in ROS 2

A 2D LiDAR Pose Graph SLAM system built from scratch in C++/ROS 2, implemented on TurtleBot3 Waffle in Gazebo simulation. The back-end optimisation approach is based on [Cyrill Stachniss's SLAM lectures](https://www.youtube.com/watch?v=uHbRKvD8TWg).

> **v1 demo:** chi² reduced 99.94% (11,404 → 6.79) in 7 Gauss-Newton iterations across 668 nodes with 24 loop closures.
> v1's `noise:=true` only perturbed the initial guesses (edges stayed clean), so that demo did not exercise loop closure - see [Front end v2](#front-end-v2-validated-2026-10-18).

---

<p align="center">
  <img src="src/media/PGS_MAP.png"  width="46%" alt="Optimized point-cloud map after Gauss-Newton back-end"/>
  &nbsp;
  <img src="src/media/PGS_MAP2.png" width="46%" alt="Pose graph: nodes, odometry edges (blue) and loop-closure edges (red)"/>
</p>

▶️ **[Watch the full SLAM run (video)](src/media/PGS_rec.mp4)**

---

## What it does

Pose Graph SLAM builds a map by maintaining a graph of robot poses connected by constraints. Every time the robot moves, an odometry edge is added. When the robot revisits a place it has been before, a loop closure edge is added using ICP scan matching. A Gauss-Newton optimiser then corrects all poses simultaneously to satisfy all constraints as well as possible — snapping the map into alignment.

The system has three components:

- **GraphBuilderNode** — front-end: creates pose nodes, odometry edges, and loop closure edges from `/odom` and `/scan`
- **BackendOptimizer** — back-end: Gauss-Newton sparse optimiser, produces corrected `/optimized_path`
- **PointCloudMapNode** — map builder: reprojects all stored LiDAR scans using corrected poses, publishes `/point_cloud_map`

---

## Front end v2 (validated 2026-10-18)

Pipeline, same structure as slam_toolbox / Karto and Cartographer (scan matching refines odometry; loop closures are verified scan matches; the back end only optimises):

```
wheel (+IMU) odometry  ->  a scan becomes a NODE every 0.2 m / 0.3 rad
                           odometry pose interpolated at the scan time, points moved into the base frame
                           (lidar offset from TF), deskewed if the scan has time_increment
                       ->  sequential edges: odometry edge + point-to-line ICP edge vs the last 3 node scans
                       ->  loop closures: candidates within 1 m of the estimate -> correlative search
                           (+-0.6 m, +-0.4 rad) -> point-to-line ICP -> accept on overlap >= 0.7, rms <= 3 cm
back end               ->  Gauss-Newton step, Levenberg-Marquardt fallback (a step must lower the cost),
                           warm start from the current estimate, Cauchy kernel on loop closures
map                    ->  each node's scan at its optimised pose (redrawn after every optimisation);
                           map -> odom TF from the newest node
```

What was checked and changed (v1 code compiled into test programs; GTSAM / scipy as independent references):

| | v1 | v2 |
|---|---|---|
| Back-end Jacobians (vs finite differences, 2000 random edges) | correct (1.9e-8) | unchanged |
| Back-end minima on Intel / M3500 / MITb | true minima (scipy cannot improve them) | same |
| Robustness: M3500b, 15 iterations | chi2 4.1e7 -> 2.6e8 (worse than the start) | never increases; 2282 solved incrementally |
| MITb, incremental (solve every 50 nodes) | 770.7 (restart from raw odometry) | 41.2 (warm start) |
| `noise:=true` | perturbed initial guesses only; GN removed 2.24 m of drift with ZERO loop closures | perturbs odometry increments (edges drift too) |
| Sequential edges | raw odometry only | odometry + point-to-line ICP |
| ICP (synthetic scans, known transform, 10 mm noise) | point-to-point, median 6.9 mm; rms gate 0.03 rejected 73 % of correct matches | point-to-line, median 1.3 mm, 0.6 ms |
| Loop closure from a guess 0.5 m / 0.3 rad off | - | 98-100 % found, 0 wrong |
| Corner-feature initial guess | inverted (cand->cur) | replaced by correlative search |
| Lidar mounting offset (-6.4 cm) | ignored: walls doubled even with perfect poses | from TF |
| Map snapshots | re-posed to the nearest node by position | each node's own scan at its optimised pose |
| map -> odom | static identity | from the optimised graph |

End to end, recorded Gazebo runs (Kalparambh, LD06-like lidar), vs Gazebo ground truth:

| odometry | odometry ATE | v1 ATE | v2 ATE | v2 map: mean distance to true walls |
|---|---|---|---|---|
| rover-like (0.1 % scale) - 3 routes | 1.3-3.3 cm | 1.2-2.3 cm | 0.2 cm | 0.2 cm |
| bad (1.5 % scale, 1.1 deg/m) - 3 routes | 24-63 cm | 20-61 cm (1 LC per run) | 0.2 cm (27-33 LCs) | 0.2 cm |
| bad + LD06 sweep, end stamp | 66 cm | - | 1.5 cm | 1.0 cm |
| bad + 2 deg lidar angle offset | 66 cm | - | 6.1 cm | 3.8 cm |
| bad + 45 mm range bias | 66 cm | - | 1.5 cm | 3.0 cm |

Live in `pgs_world`, 3 laps, `noise:=true`: odometry alone 25 cm ATE, v2 2.4 cm (reference = Gazebo wheel odometry, itself a few cm off), 51 loop closures, map median 0.96 cm from the wall faces (sim lidar noise 1 cm).
On a real lidar, calibrate the mounting yaw first (2 deg costs most), then the range bias.

---

## Prerequisites

| Requirement | Version |
|---|---|
| Ubuntu | 22.04 LTS |
| ROS 2 | Humble Hawksbill — [install guide](https://docs.ros.org/en/humble/Installation.html) |
| TurtleBot3 | `sudo apt install ros-humble-turtlebot3*` |
| Eigen3 | `sudo apt install libeigen3-dev` |
| Gazebo Classic | Installed automatically with TurtleBot3 |

---

## Installation

### 1. Clone

```bash
mkdir -p ~/pgs_ws/src
cd ~/pgs_ws/src
git clone https://github.com/VyomVyas25/Pose_Graph_SLAM.git .
```

### 2. Set TurtleBot3 model

```bash
echo "export TURTLEBOT3_MODEL=waffle" >> ~/.bashrc
source ~/.bashrc
```

### 3. Build

```bash
cd ~/pgs_ws
colcon build --packages-select slam_msgs my_pose_graph_slam
source install/setup.bash
```

Add `source ~/pgs_ws/install/setup.bash` to your `~/.bashrc` to avoid sourcing every time.

---

## Running

### One command: Gazebo `pgs_world` + SLAM + RViz + scripted loop (3 laps around the pillar)

```bash
ros2 launch my_pose_graph_slam pgs_sim.launch.py              # noise:=false laps:=2 drive:=false gui:=false
```
Needs `turtlebot3_gazebo` with `worlds/pgs_world.world` sourced. RViz: blue = Gazebo path, red = odometry alone,
green = optimised path, red graph edges = loop closures.

### Manual (any world / real robot)

Three terminals are needed. Gazebo and teleop are kept separate so you can swap worlds or use a real robot.

### Terminal 1 — Gazebo

```bash
export TURTLEBOT3_MODEL=waffle
ros2 launch turtlebot3_gazebo turtlebot3_house.launch.py
```

### Terminal 2 — SLAM stack

```bash
ros2 launch my_pose_graph_slam slam.launch.py noise:=true use_sim_time:=true
```

This starts the graph builder, backend optimiser, point cloud map node, and RViz all at once.

### Terminal 3 — Drive the robot

```bash
export TURTLEBOT3_MODEL=waffle
ros2 run turtlebot3_teleop teleop_keyboard
```

Drive **3–5 complete laps** to generate enough loop closures for visible drift correction.

---

## Launch arguments

| Argument | Default | Description |
|---|---|---|
| `noise` | `false` | Perturb the odometry increments (edges and initial guesses drift) to test scan matching and loop closure |
| `use_sim_time` | `false` | `true` with Gazebo or a bag |
| `lc_cooldown` | `5` | Minimum nodes between accepted loop closures |
| `noise_alpha1` | `0.02` | Translation noise magnitude |
| `noise_alpha3` | `0.005` | Rotation noise magnitude |
| `scan_skip` | `3` | Publish every N-th scan in the point cloud (higher = faster, fewer points) |
| `rviz` | `true` | Launch RViz2 |

---

## RViz displays

Set **Fixed Frame** to `map`, then add:

| Display | Topic |
|---|---|
| MarkerArray | `/graph_markers` — nodes (white), odom edges (blue), loop closure edges (red) |
| Path | `/optimized_path` (green), `/odom_path` (odometry alone), `/gt_path` (sim only) |
| PointCloud2 | `/point_cloud_map` |

---

## Troubleshooting

**No loop closures after lap 2:**
```bash
ros2 launch my_pose_graph_slam slam.launch.py noise:=true lc_cooldown:=5
```

**Optimizer shows Δx ≈ 0 with `noise:=false`:**
Expected — Gazebo odometry is near-perfect with no drift to correct. Use `noise:=true`.

**Real lidar (e.g. LD06):** its driver stamps the END of the rotation - set `scan_stamp_at_end:=true` on
`graph_builder_node` so deskewing uses the right beam times. Odometry input: `odom_topic` (e.g. a wheel + IMU EKF).

**`slam_msgs` not found:**
```bash
colcon build --packages-select slam_msgs
source install/setup.bash
colcon build --packages-select my_pose_graph_slam
```

---

## Acknowledgements

- [Cyrill Stachniss](https://www.youtube.com/watch?v=uHbRKvD8TWg) — SLAM lecture series this back-end is based on; Grisetti et al., "A Tutorial on Graph-Based SLAM" (2010)
- Censi, "An ICP variant using a point-to-line metric" (ICRA 2008); Olson, "Real-time correlative scan matching" (ICRA 2009)
- [Eigen](https://eigen.tuxfamily.org/) — sparse linear algebra
- [ROS 2 Humble](https://docs.ros.org/en/humble/) — middleware
- [TurtleBot3](https://emanual.robotis.com/docs/en/platform/turtlebot3/overview/) — robot platform
