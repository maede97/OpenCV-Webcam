#include <iostream>
#include <opencv2/opencv.hpp>
#include <exception>

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
    Darker
};

struct Effect
{
    Effect(int x, int y, int width, int height, cv::String name, EFFECT eff) : x_(x), y_(y), width_(width), height_(height), name_(name), eff_(eff)
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

    EFFECT getEffect() const
    {
        return eff_;
    }

    int x_;
    int y_;
    int width_;
    int height_;
    EFFECT eff_;
    cv::String name_;
};

struct Effects
{
    void addEffect(Effect e)
    {
        effects_.push_back(e);
    }
    std::vector<Effect> getEffects() const
    {
        return effects_;
    }
    std::vector<Effect> effects_;
};

std::vector<Effect> appliedEffects;

static const int NROWS = 3;
static const int NCOLS = 3;

cv::String WINDOW_NAME = "maingui";
cv::String WINDOW_TITLE = "Choose Effect";

Effects eff;

static void handleClick(int event, int x, int y, int, void *)
{
    if (event != cv::EVENT_LBUTTONDOWN)
        return;
    for (Effect &e : eff.getEffects())
    {
        if (e.contains(x, y))
        {
            if (e.getEffect() == EFFECT::None)
            {
                appliedEffects = std::vector<Effect>();
                cv::setWindowTitle(WINDOW_NAME, WINDOW_TITLE);
                return;
            }
            else if (e.getEffect() == EFFECT::Black)
            {
                appliedEffects = std::vector<Effect>();
                cv::setWindowTitle(WINDOW_NAME, WINDOW_TITLE);
            }

            appliedEffects.push_back(e);

            std::string title = "";
            for (const Effect &e : appliedEffects)
            {
                title += e.getName() + ", ";
            }

            cv::setWindowTitle(WINDOW_NAME, WINDOW_TITLE + " (Current: " + title.substr(0, title.length() - 2) + ")");

            if (appliedEffects.size() > 4)
            {
                cv::setWindowTitle(WINDOW_NAME, WINDOW_TITLE + " (Current: " + std::to_string(appliedEffects.size()) + " applied)");
            }

            break;
        }
    }
}

void applyGray(cv::Mat &in)
{
    cv::cvtColor(in, in, cv::COLOR_BGR2GRAY);
    cv::cvtColor(in, in, cv::COLOR_GRAY2BGR);
}

void applyEdge(cv::Mat &in)
{
    cv::cvtColor(in, in, cv::COLOR_BGR2GRAY);
    cv::Canny(in, in, 100, 300, 3);
    cv::cvtColor(in, in, cv::COLOR_GRAY2BGR);
}

void applyPixelate(cv::Mat &in)
{
    int size = 15; //only odd!
    int max_step = (size - 1) / 2;
    for (int i = max_step; i < in.rows - max_step; i += size)
    {
        for (int j = max_step; j < in.cols - max_step; j += size)
        {
            cv::Vec3b colour = in.at<cv::Vec3b>(cv::Point(j, i));
            for (int k = -max_step; k <= max_step; k++)
            {
                for (int l = -max_step; l <= max_step; l++)
                {
                    in.at<cv::Vec3b>(cv::Point(j - k, i - l)) = colour;
                }
            }
        }
    }
}

void applyInversion(cv::Mat &in)
{
    cv::bitwise_not(in, in);
}

void applyBlack(cv::Mat &in)
{
    in = cv::Scalar(0, 0, 0);
}

void applyBlur(cv::Mat &in)
{
    cv::Mat original(in.size(), in.type());
    cv::pyrDown(in, in);
    cv::pyrDown(in, in);
    cv::Mat temp(original.size(), original.type());
    cv::bilateralFilter(in, temp, 9, 9, 7);
    cv::pyrUp(temp, in);
    cv::pyrUp(in, in);
    cv::resize(in, in, original.size());
}
void applyBrighter(cv::Mat &in, int a = 10)
{
    in.convertTo(in, -1, 1, a);
}
void applyDarker(cv::Mat &in, int a = 10)
{
    in.convertTo(in, -1, 1, -a);
}

void applyEffects(cv::Mat &in, cv::Mat &out)
{
    cv::Size size = out.size();
    const int height = size.height / NROWS;
    const int width = size.width / NCOLS;

    cv::Size newSize(width, height); // size of each effect

    // normal
    cv::Mat effect_none;
    cv::resize(in, effect_none, newSize);
    effect_none.copyTo(out(cv::Rect(0, 0, width, height)));
    eff.addEffect(Effect(0, 0, width, height, "None", EFFECT::None));

    // Gray
    cv::Mat effect_gray;
    in.copyTo(effect_gray);
    applyGray(effect_gray);
    cv::resize(effect_gray, effect_gray, newSize);
    effect_gray.copyTo(out(cv::Rect(width, 0, width, height)));
    eff.addEffect(Effect(width, 0, width, height, "Gray", EFFECT::Gray));

    // edge detection
    cv::Mat effect_edges;
    in.copyTo(effect_edges);
    applyEdge(effect_edges);
    cv::resize(effect_edges, effect_edges, newSize);
    effect_edges.copyTo(out(cv::Rect(2 * width, 0, width, height)));
    eff.addEffect(Effect(2 * width, 0, width, height, "Edges", EFFECT::Edges));

    // Pixelate
    cv::Mat effect_pixelate;
    in.copyTo(effect_pixelate);
    applyPixelate(effect_pixelate);
    cv::resize(effect_pixelate, effect_pixelate, newSize);
    effect_pixelate.copyTo(out(cv::Rect(0, height, width, height)));
    eff.addEffect(Effect(0, height, width, height, "Pixelate", EFFECT::Pixelate));

    // Inversion
    cv::Mat effect_inversion;
    in.copyTo(effect_inversion);
    applyInversion(effect_inversion);
    cv::resize(effect_inversion, effect_inversion, newSize);
    effect_inversion.copyTo(out(cv::Rect(width, height, width, height)));
    eff.addEffect(Effect(width, height, width, height, "Inversion", EFFECT::Inversion));

    // Black image
    cv::Mat effect_black;
    in.copyTo(effect_black);
    applyBlack(effect_black);
    cv::resize(effect_black, effect_black, newSize);
    effect_black.copyTo(out(cv::Rect(width * 2, height, width, height)));
    eff.addEffect(Effect(2 * width, height, width, height, "Black", EFFECT::Black));

    //
    cv::Mat effect_blur;
    in.copyTo(effect_blur);
    applyBlur(effect_blur);
    cv::resize(effect_blur, effect_blur, newSize);
    effect_blur.copyTo(out(cv::Rect(0, height * 2, width, height)));
    eff.addEffect(Effect(0, height * 2, width, height, "Blur", EFFECT::Blur));

    // Brighter
    cv::Mat effect_brighter;
    in.copyTo(effect_brighter);
    applyBrighter(effect_brighter, 50);
    cv::resize(effect_brighter, effect_brighter, newSize);
    effect_brighter.copyTo(out(cv::Rect(width, height * 2, width, height)));
    eff.addEffect(Effect(width, height * 2, width, height, "Brighter", EFFECT::Brighter));

    // Darker
    cv::Mat effect_darker;
    in.copyTo(effect_darker);
    applyDarker(effect_darker, 50);
    cv::resize(effect_darker, effect_darker, newSize);
    effect_darker.copyTo(out(cv::Rect(width * 2, height * 2, width, height)));
    eff.addEffect(Effect(width * 2, height * 2, width, height, "Darker", EFFECT::Darker));
}

int main(int argc, char const *argv[])
{
    const int camera = (argc == 1) ? 0 : std::stoi(argv[1]);
    // open camera
    cv::VideoCapture cap(camera);
    if (!cap.isOpened())
    {
        throw std::runtime_error("Unable to open camera");
    }

    // get constants:
    const int width = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_WIDTH));
    const int height = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_HEIGHT));
    const double FPS = cap.get(cv::CAP_PROP_FPS);

    const double factor = width / (1.0 * height);

    const int SHOW_WIDTH = 600;
    const int SHOW_HEIGHT = SHOW_WIDTH / NCOLS / factor * NROWS;

    const int Stream_Width = 400;
    const int Stream_Height = Stream_Width / factor;

    // create gui
    cv::namedWindow(WINDOW_NAME, cv::WINDOW_KEEPRATIO);
    cv::setWindowTitle(WINDOW_NAME, WINDOW_TITLE);

    cv::setMouseCallback(WINDOW_NAME, handleClick, 0);

    // declare all image_placeholders
    cv::Mat input_image;
    cv::Mat output_image;
    cv::Mat stream_image;

    while (true)
    {
        // read image from capture
        cap >> input_image;
        if (input_image.empty())
        {
            throw std::runtime_error("Can't read images from camera");
        }

        stream_image = input_image;

        // proccess image
        output_image.create(SHOW_HEIGHT, SHOW_WIDTH, input_image.type());
        applyEffects(input_image, output_image);

        // apply chosen effects onto stream image
        for (Effect &e : appliedEffects)
        {
            switch (e.getEffect())
            {
            case EFFECT::Gray:
            {
                applyGray(stream_image);
                break;
            }
            case EFFECT::Edges:
            {
                applyEdge(stream_image);
                break;
            }
            case EFFECT::Pixelate:
            {
                applyPixelate(stream_image);
                break;
            }
            case EFFECT::Inversion:
            {
                applyInversion(stream_image);
                break;
            }
            case EFFECT::Black:
            {
                applyBlack(stream_image);
                break;
            }
            case EFFECT::Blur:
            {
                applyBlur(stream_image);
                break;
            }
            case EFFECT::Brighter:
            {
                applyBrighter(stream_image);
                break;
            }
            case EFFECT::Darker:
            {
                applyDarker(stream_image);
                break;
            }
            default:
                break;
            }
        }

        cv::Mat show;
        show.create(SHOW_HEIGHT, SHOW_WIDTH + Stream_Width, input_image.type());
        applyBlack(show);

        output_image.copyTo(show(cv::Rect(0, 0, SHOW_WIDTH, SHOW_HEIGHT)));
        cv::Mat stream_small;
        stream_small.create(Stream_Height, Stream_Width, input_image.type());
        cv::resize(stream_image, stream_small, stream_small.size());
        stream_small.copyTo(show(cv::Rect(SHOW_WIDTH, (SHOW_HEIGHT - Stream_Height) / 2, Stream_Width, Stream_Height)));

        // show image
        cv::imshow(WINDOW_NAME, show);

        // create buffer for feed
        std::vector<uchar> buff;
        cv::imencode(".jpg", stream_image, buff);
        // write buffer to stdout
        fwrite(buff.data(), buff.size(), 1, stdout);

        //stop when user presses ESC
        if (cv::waitKey(1000.0 / FPS) == 27)
            break;
    }

    return 0;
}