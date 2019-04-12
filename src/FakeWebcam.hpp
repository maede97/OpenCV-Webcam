#ifndef FAKE_WEBCAM_HPP
#define FAKE_WEBCAM_HPP

#include <opencv2/opencv.hpp>
#include <string>

class FakeWebcam
{
public:
    FakeWebcam();
    ~FakeWebcam();
    enum EFFECT
    {
        None,
        Gray,
        Edges,
        Pixelate,
        Inversion,
        Black,
        Blur,
        Brighter,
        Darker,
        Face,
        Sepia,
        FishEye,
        Image, // show image on click
        Video  // show video on click
    };

    void readConfig(const char *filename);
    void run();

private:
    struct VideoHolder;

    struct ClickableEffect;

    void handleClick(int event, int x, int y);
    static void handleClick(int event, int x, int y, int, void *userdata);

    void applyGray(cv::Mat &in);
    void applyEdge(cv::Mat &in);
    void applyPixelate(cv::Mat &in);
    void applyInversion(cv::Mat &in);
    void applyBlack(cv::Mat &in);
    void applyBlur(cv::Mat &in);
    void applyBrighter(cv::Mat &in, int a = 10);
    void applyDarker(cv::Mat &in, int a = 10);
    void applyImage(cv::Mat &in, cv::Mat image);
    void applyVideo(cv::Mat &in, VideoHolder *vh);
    void applyFaceDetection(cv::Mat &in);
    void applySepia(cv::Mat &in);
    void applyFishEye(cv::Mat &in);

    void applyEffects(cv::Mat &in, cv::Mat &out);

    void placeImages(cv::Mat &in, int offset, int width, int height);
    void placeVideos(cv::Mat &in, int offset, int width, int height);

    std::vector<ClickableEffect> allPossibleEffects;
    std::vector<ClickableEffect> appliedEffects;

    static const int NROWS = 4;
    static const int NCOLS = 3;

    cv::String WINDOW_NAME = "maingui";
    cv::String WINDOW_TITLE = "Choose Effect";

    int camera = 0;

    std::vector<cv::Mat> images;
    std::vector<VideoHolder *> videos;

    cv::CascadeClassifier faceCascade;
};

#endif