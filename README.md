# OpenCV-Webcam
This is a simple program, which displays an easy to use GUI, with which you can change your webcam feed to use with whatever program you want.

# Installation
`git clone https://github.com/maede97/OpenCV-Webcam`\
`sudo apt install v4l2loopback-dkms`

Clone also the opencv-github-repository and build it using\
## openCV
To install openCV, use the following steps:\
`git clone https://github.com/opencv/opencv`\
`cd opencv`\
`mkdir build && cd build`\
`cmake -D CMAKE_BUILD_TYPE=RELEASE -D CMAKE_INSTALL_PREFIX=/usr/local  -D WITH_FFMPEG=ON -D WITH_TBB=ON -D WITH_GTK=ON -D WITH_V4L=ON -D WITH_OPENGL=ON -D WITH_CUBLAS=ON -DWITH_QT=OFF -DCUDA_NVCC_FLAGS="-D_FORCE_INLINES" ..`\
`make -j4`\
`make install`\
The special flags for the `cmake` command are needed for video-file-reading

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
