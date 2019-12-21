# ball_chaser

This ROS package contains to nodes that act as server and client.

The communication is done between the camera and the drive_bot node.

The process_image node will determine in what direction should the robot drive, depending on the messages sent by the camera.

* drive_bot: server
* process_image: client

## Compute graph

Diagram of nodes, topics and their connections in this package.

![](ref_images/rqt_graph.png)
