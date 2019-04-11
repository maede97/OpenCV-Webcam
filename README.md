# OpenCV-Webcam
This is a simple program, which displays an easy to use GUI, with which you can change your webcam feed to use with whatever program you want.

# Installation
`git clone https://github.com/maede97/OpenCV-Webcam`\
`sudo apt install v4l2loopback-dkms`

Then create a new video-device:\
`sudo modprobe v4l2loopback video_nr=23`

Check whether this video device now exists:\
`ls /dev | grep video23`\
If this returns nothing, try to remove the module:\
`sudo modprobe -r v4l2loopback`\
Then, try to create the `/dev/video23` again

# Building
Move into your cloned folder, then simply call\
`./build.sh`\
This will automatically create a build folder for you and create the binary.

# Usage
Run `./run.sh` from within the root folder, use `/dev/video23` as your source for your new video feed. Change behaviour of your feed using the GUI.
