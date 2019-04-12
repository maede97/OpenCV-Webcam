#include "FakeWebcam.hpp"

struct FakeWebcam::VideoHolder
{
    VideoHolder()
    {
    }
    ~VideoHolder()
    {
    }
    VideoHolder(cv::String filename)
    {
        filename_ = filename;
        cap = cv::VideoCapture(filename);
        counter_ = 0;
    }

    cv::Mat peekImage() const { return currImage; }

    cv::Mat getFrame()
    {
        counter_++;
        if (counter_ == cap.get(cv::CAP_PROP_FRAME_COUNT) - 1)
        {
            counter_ = 0;
            cap.release();
            cap.open(filename_);
        }
        cv::Mat image;
        cap >> image;
        currImage = image;
        return image;
    }
    cv::Mat currImage;
    int counter_;
    cv::VideoCapture cap;
    cv::String filename_;
};

struct FakeWebcam::ClickableEffect
{
    ClickableEffect(int x, int y, int width, int height,
                    cv::String name, FakeWebcam::EFFECT eff, cv::Mat image = cv::Mat(),
                    VideoHolder *vh = nullptr)
        : x_(x), y_(y), width_(width), height_(height), name_(name), eff_(eff), image_(image), vh_(vh)
    {
    }
    bool contains(int x, int y)
    {
        return (x_ < x && x_ + width_ > x && y_ < y && y_ + height_ > y);
    }
    cv::String getName() const
    {
        return name_;
    }

    FakeWebcam::EFFECT getEffect() const
    {
        return eff_;
    }

    cv::Mat getImage() const { return image_; }

    VideoHolder *getVideo() const { return vh_; }

    int x_;
    int y_;
    int width_;
    int height_;
    cv::Mat image_;
    FakeWebcam::EFFECT eff_;
    VideoHolder *vh_;
    cv::String name_;
};


namespace FishEye
{
float calc_shift(float x1, float x2, float cx, float k)
{
    float thresh = 1;
    float x3 = x1 + (x2 - x1) * 0.5;
    float res1 = x1 + ((x1 - cx) * k * ((x1 - cx) * (x1 - cx)));
    float res3 = x3 + ((x3 - cx) * k * ((x3 - cx) * (x3 - cx)));

    if (res1 > -thresh and res1 < thresh)
        return x1;
    if (res3 < 0)
    {
        return calc_shift(x3, x2, cx, k);
    }
    else
    {
        return calc_shift(x1, x3, cx, k);
    }
}

float getRadialX(float x, float y, float cx, float cy, float k, bool scale, cv::Vec4f props)
{
    float result;
    if (scale)
    {
        float xshift = props[0];
        float yshift = props[1];
        float xscale = props[2];
        float yscale = props[3];

        x = (x * xscale + xshift);
        y = (y * yscale + yshift);
        result = x + ((x - cx) * k * ((x - cx) * (x - cx) + (y - cy) * (y - cy)));
    }
    else
        result = x + ((x - cx) * k * ((x - cx) * (x - cx) + (y - cy) * (y - cy)));

    return result;
}

float getRadialY(float x, float y, float cx, float cy, float k, bool scale, cv::Vec4f props)
{
    float result;
    if (scale)
    {
        float xshift = props[0];
        float yshift = props[1];
        float xscale = props[2];
        float yscale = props[3];

        x = (x * xscale + xshift);
        y = (y * yscale + yshift);
        result = y + ((y - cy) * k * ((x - cx) * (x - cx) + (y - cy) * (y - cy)));
    }
    else
        result = y + ((y - cy) * k * ((x - cx) * (x - cx) + (y - cy) * (y - cy)));
    return result;
}
} // namespace FishEye