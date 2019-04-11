#include <iostream>
#include <opencv2/opencv.hpp>

int main(int argc, char const *argv[])
{
    // open camera
    cv::VideoCapture cap(0);
    if (!cap.isOpened())
    {
        std::cout << "Unable to open camera." << std::endl;
        std::exit(-1);
    }

    // get capture data
    const int width = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_WIDTH));
    const int height = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_HEIGHT));
    const double FPS = cap.get(cv::CAP_PROP_FPS);

    //std::cout << "WIDTH:  " << width << std::endl;
    //std::cout << "HEIGHT: " << height << std::endl;
    //std::cout << "FPS:    " << FPS << std::endl;

    // create images
    cv::Mat image;
    cv::Mat panel_middle, panel_right;

    // size of one panel
    const int delta = width / 3;

    while (true)
    {
        cap >> image; // read image
        if (image.empty())
        {
            std::cout << "Cannot read images from camera" << std::endl;
            break;
        }

        // convert middle part to gray
        panel_middle = image(cv::Rect(delta, 0, image.cols / 3, image.rows));
        cv::cvtColor(panel_middle, panel_middle, cv::COLOR_BGR2GRAY);
        cv::cvtColor(panel_middle, panel_middle, cv::COLOR_GRAY2BGR);

        // copy middle part back to image
        panel_middle.copyTo(image(cv::Rect(delta, 0, image.cols / 3, image.rows)));

        // convert right panel
        panel_right = image(cv::Rect(2 * delta, 0, image.cols / 3, image.rows));
        cv::cvtColor(panel_right, panel_right, cv::COLOR_BGR2GRAY);

        // apply filter onto right panel
        cv::Canny(panel_right, panel_right, 100, 300, 3, true);
        cv::cvtColor(panel_right, panel_right, cv::COLOR_GRAY2BGR);

        // copy right panel back
        panel_right.copyTo(image(cv::Rect(delta * 2, 0, image.cols / 3, image.rows)));

        // show feed
        //cv::imshow("Camera Feed", image);
        std::vector<uchar> buff;
        cv::imencode(".jpg",image, buff);

        // std::cout << image.data << std::flush;
        fwrite(buff.data(), buff.size(), 1, stdout);

        // stop when user presses ESC
        if (cv::waitKey(1000.0 / FPS) == 27)
            break;
    }

    return 0;
}
