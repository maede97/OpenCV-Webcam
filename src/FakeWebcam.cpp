#include "FakeWebcam.hpp"

#include <fstream>
#include <iostream>
#include <ctime>

#include "helper.cpp"

FakeWebcam::FakeWebcam()
{
}
FakeWebcam::~FakeWebcam()
{
}

void FakeWebcam::handleClick(int event, int x, int y, int, void *userdata)
{
    FakeWebcam *camClass = reinterpret_cast<FakeWebcam *>(userdata);
    camClass->handleClick(event, x, y);
}

void FakeWebcam::handleClick(int event, int x, int y)
{
    if (event != cv::EVENT_LBUTTONDOWN)
        return;
    for (auto &e : allPossibleEffects)
    {
        if (e.contains(x, y))
        {
            if (e.getEffect() == EFFECT::None)
            {
                appliedEffects = std::vector<ClickableEffect>();
                cv::setWindowTitle(WINDOW_NAME, WINDOW_TITLE);
                return;
            }
            else if (e.getEffect() == EFFECT::Black)
            {
                appliedEffects = std::vector<ClickableEffect>();
                cv::setWindowTitle(WINDOW_NAME, WINDOW_TITLE);
            }

            appliedEffects.push_back(e);

            std::string title = "";
            for (const ClickableEffect &e : appliedEffects)
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

void FakeWebcam::applyGray(cv::Mat &in)
{
    cv::cvtColor(in, in, cv::COLOR_BGR2GRAY);
    cv::cvtColor(in, in, cv::COLOR_GRAY2BGR);
}
void FakeWebcam::applyEdge(cv::Mat &in)
{
    cv::cvtColor(in, in, cv::COLOR_BGR2GRAY);
    cv::Canny(in, in, 100, 300, 3);
    cv::cvtColor(in, in, cv::COLOR_GRAY2BGR);
}
void FakeWebcam::applyPixelate(cv::Mat &in)
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
void FakeWebcam::applyInversion(cv::Mat &in)
{
    cv::bitwise_not(in, in);
}
void FakeWebcam::applyBlack(cv::Mat &in)
{
    in = cv::Scalar(0, 0, 0);
}
void FakeWebcam::applyBlur(cv::Mat &in)
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
void FakeWebcam::applyBrighter(cv::Mat &in, int a)
{
    in.convertTo(in, -1, 1, a);
}
void FakeWebcam::applyDarker(cv::Mat &in, int a)
{
    in.convertTo(in, -1, 1, -a);
}

void FakeWebcam::applyVideo(cv::Mat &in, VideoHolder *vh)
{
    cv::Mat image = vh->peekImage();
    applyImage(in, image);
}

void FakeWebcam::applyImage(cv::Mat &in, cv::Mat image)
{
    applyBlack(in);
    float fac_height = in.size().height / (1.0 * image.size().height);
    float fac_width = in.size().width / (1.0 * image.size().width);

    int img_height, img_width;

    if (fac_height < fac_width)
    {
        // scale down on height
        float factor = image.size().height / (1.0 * image.size().width);
        img_height = in.size().height;
        img_width = img_height / factor;
        cv::resize(image, image, cv::Size(img_width, img_height));
        image.copyTo(in(cv::Rect(0, 0, img_width, img_height)));
    }
    else
    {
        // scale down on width
        float factor = image.size().width / (1.0 * image.size().height);
        img_width = in.size().width;
        img_height = img_width / factor;
        cv::resize(image, image, cv::Size(img_width, img_height));
        image.copyTo(in(cv::Rect(0, 0, img_width, img_height)));
    }
}
void FakeWebcam::applyEffects(cv::Mat &in, cv::Mat &out)
{
    cv::Size size = out.size();
    const int height = size.height / NROWS;
    const int width = size.width / NCOLS;

    cv::Size newSize(width, height); // size of each effect

    allPossibleEffects = std::vector<ClickableEffect>();

    // normal
    cv::Mat effect_none;
    cv::resize(in, effect_none, newSize);
    effect_none.copyTo(out(cv::Rect(0, 0, width, height)));
    allPossibleEffects.push_back(ClickableEffect(0, 0, width, height, "None", EFFECT::None));

    // Gray
    cv::Mat effect_gray;
    in.copyTo(effect_gray);
    applyGray(effect_gray);
    cv::resize(effect_gray, effect_gray, newSize);
    effect_gray.copyTo(out(cv::Rect(width, 0, width, height)));
    allPossibleEffects.push_back(ClickableEffect(width, 0, width, height, "Gray", EFFECT::Gray));

    // edge detection
    cv::Mat effect_edges;
    in.copyTo(effect_edges);
    applyEdge(effect_edges);
    cv::resize(effect_edges, effect_edges, newSize);
    effect_edges.copyTo(out(cv::Rect(2 * width, 0, width, height)));
    allPossibleEffects.push_back(ClickableEffect(2 * width, 0, width, height, "Edges", EFFECT::Edges));

    // Pixelate
    cv::Mat effect_pixelate;
    in.copyTo(effect_pixelate);
    applyPixelate(effect_pixelate);
    cv::resize(effect_pixelate, effect_pixelate, newSize);
    effect_pixelate.copyTo(out(cv::Rect(0, height, width, height)));
    allPossibleEffects.push_back(ClickableEffect(0, height, width, height, "Pixelate", EFFECT::Pixelate));

    // Inversion
    cv::Mat effect_inversion;
    in.copyTo(effect_inversion);
    applyInversion(effect_inversion);
    cv::resize(effect_inversion, effect_inversion, newSize);
    effect_inversion.copyTo(out(cv::Rect(width, height, width, height)));
    allPossibleEffects.push_back(ClickableEffect(width, height, width, height, "Inversion", EFFECT::Inversion));

    // Black image
    cv::Mat effect_black;
    in.copyTo(effect_black);
    applyBlack(effect_black);
    cv::resize(effect_black, effect_black, newSize);
    effect_black.copyTo(out(cv::Rect(width * 2, height, width, height)));
    allPossibleEffects.push_back(ClickableEffect(2 * width, height, width, height, "Black", EFFECT::Black));

    //
    cv::Mat effect_blur;
    in.copyTo(effect_blur);
    applyBlur(effect_blur);
    cv::resize(effect_blur, effect_blur, newSize);
    effect_blur.copyTo(out(cv::Rect(0, height * 2, width, height)));
    allPossibleEffects.push_back(ClickableEffect(0, height * 2, width, height, "Blur", EFFECT::Blur));

    // Brighter
    cv::Mat effect_brighter;
    in.copyTo(effect_brighter);
    applyBrighter(effect_brighter, 50);
    cv::resize(effect_brighter, effect_brighter, newSize);
    effect_brighter.copyTo(out(cv::Rect(width, height * 2, width, height)));
    allPossibleEffects.push_back(ClickableEffect(width, height * 2, width, height, "Brighter", EFFECT::Brighter));

    // Darker
    cv::Mat effect_darker;
    in.copyTo(effect_darker);
    applyDarker(effect_darker, 50);
    cv::resize(effect_darker, effect_darker, newSize);
    effect_darker.copyTo(out(cv::Rect(width * 2, height * 2, width, height)));
    allPossibleEffects.push_back(ClickableEffect(width * 2, height * 2, width, height, "Darker", EFFECT::Darker));

    // Face Detection
    cv::Mat effect_face;
    in.copyTo(effect_face);
    applyFaceDetection(effect_face);
    cv::resize(effect_face, effect_face, newSize);
    effect_face.copyTo(out(cv::Rect(0, height * 3, width, height)));
    allPossibleEffects.push_back(ClickableEffect(0, height * 3, width, height, "Face", EFFECT::Face));

    // Sepia
    cv::Mat effect_sepia;
    in.copyTo(effect_sepia);
    applySepia(effect_sepia);
    cv::resize(effect_sepia, effect_sepia, newSize);
    effect_sepia.copyTo(out(cv::Rect(width, height * 3, width, height)));
    allPossibleEffects.push_back(ClickableEffect(width, height * 3, width, height, "Sepia", EFFECT::Sepia));

    // FishEye
    cv::Mat effect_fisheye;
    in.copyTo(effect_fisheye);
    applyFishEye(effect_fisheye);
    cv::resize(effect_fisheye, effect_fisheye, newSize);
    effect_fisheye.copyTo(out(cv::Rect(width * 2, height * 3, width, height)));
    allPossibleEffects.push_back(ClickableEffect(width * 2, height * 3, width, height, "FishEye", EFFECT::FishEye));
}
void FakeWebcam::readConfig(const char *filename)
{
    std::ifstream s(filename);
    if (!s.is_open())
        throw std::runtime_error("could not open file");
    std::string line;
    while (getline(s, line))
    {
        // comments on lines
        if (line[0] == '#' || line.empty())
            continue;
        auto delimiterPos = line.find("=");
        auto name = line.substr(0, delimiterPos);
        auto value = line.substr(delimiterPos + 1);
        if (name == "Camera")
        {
            camera = std::stoi(value);
        }
        else if (name == "Image")
        {
            images.push_back(cv::imread(value));
        }
        else if (name == "Video")
        {
            videos.push_back(new VideoHolder(value));
        }
        else if (name == "FaceCascade")
        {
            faceCascade.load(value);
        }
    }
}

void FakeWebcam::run()
{
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

    const int Image_Width = 200;
    const int Video_Width = 200;

    // create gui
    cv::namedWindow(WINDOW_NAME, cv::WINDOW_KEEPRATIO);
    cv::setWindowTitle(WINDOW_NAME, WINDOW_TITLE);

    cv::setMouseCallback(WINDOW_NAME, handleClick, this);

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
        for (ClickableEffect &e : appliedEffects)
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
            case EFFECT::Face:
            {
                applyFaceDetection(stream_image);
                break;
            }
            case EFFECT::Sepia:
            {
                applySepia(stream_image);
                break;
            }
            case EFFECT::FishEye:
            {
                applyFishEye(stream_image);
                break;
            }
            case EFFECT::Image:
            {
                applyImage(stream_image, e.getImage());
                break;
            }

            case EFFECT::Video:
                applyVideo(stream_image, e.getVideo());

            default:
                break;
            }
        }

        cv::Mat show;
        show.create(SHOW_HEIGHT, SHOW_WIDTH + Stream_Width + Image_Width + Video_Width, input_image.type());
        applyBlack(show);

        output_image.copyTo(show(cv::Rect(0, 0, SHOW_WIDTH, SHOW_HEIGHT)));
        cv::Mat stream_small;
        stream_small.create(Stream_Height, Stream_Width, input_image.type());
        cv::resize(stream_image, stream_small, stream_small.size());

        stream_small.copyTo(show(cv::Rect(SHOW_WIDTH, (SHOW_HEIGHT - Stream_Height) / 2, Stream_Width, Stream_Height)));

        // copy images to show file
        placeImages(show, SHOW_WIDTH + Stream_Width, Image_Width, SHOW_HEIGHT);
        placeVideos(show, SHOW_WIDTH + Stream_Width + Image_Width, Video_Width, SHOW_HEIGHT);

        // show image
        cv::imshow(WINDOW_NAME, show);

        // create buffer for feed
        std::vector<uchar> buff;
        cv::imencode(".jpg", stream_image, buff);
        // write buffer to stdout
        fwrite(buff.data(), buff.size(), 1, stdout);

        //stop when user presses ESC
        int keyCode = cv::waitKey(1000.0 / FPS);
        if (keyCode == 27)
            break;
        if (keyCode == 32)
        {
            // space
            // save image to disk
            auto t = std::time(0);
            cv::imwrite("screenshot" + std::to_string(t) + ".png", stream_image);
        }
    }
}

void FakeWebcam::placeImages(cv::Mat &in, int offset, int width, int height)
{
    int amount = images.size();
    int curr_h = 0;
    for (cv::Mat &image : images)
    {
        cv::Mat temp(image.size(), image.type());
        int h = image.size().height * width / image.size().width;

        cv::resize(image, temp, cv::Size(width, h));
        temp.copyTo(in(cv::Rect(offset, curr_h, width, h)));
        allPossibleEffects.push_back(ClickableEffect(offset, curr_h, width, h, "Image", EFFECT::Image, image));
        curr_h += h;
    }
}

void FakeWebcam::placeVideos(cv::Mat &in, int offset, int width, int height)
{
    int amount = videos.size();
    int curr_h = 0;
    for (VideoHolder *vh : videos)
    {
        cv::Mat image = vh->getFrame();

        cv::Mat temp(image.size(), image.type());
        int h = image.size().height * width / image.size().width;

        cv::resize(image, temp, cv::Size(width, h));
        temp.copyTo(in(cv::Rect(offset, curr_h, width, h)));
        allPossibleEffects.push_back(ClickableEffect(offset, curr_h, width, h, "Video", EFFECT::Video, cv::Mat(), vh));
        curr_h += h;
    }
}

void FakeWebcam::applyFaceDetection(cv::Mat &in)
{
    if (faceCascade.empty())
    {
        return;
    }
    std::vector<cv::Rect> faces;
    cv::Mat frameGray;
    cv::cvtColor(in, frameGray, cv::COLOR_BGR2GRAY);
    faceCascade.detectMultiScale(frameGray, faces, 1.1, 1);
    for (cv::Rect &r : faces)
    {
        cv::Mat face_Rect = in(r);
        face_Rect.setTo(cv::Scalar(0));
    }
}

void FakeWebcam::applySepia(cv::Mat &in)
{
    cv::Mat kernel =
        (cv::Mat_<float>(3, 3)
             << 0.272,
         0.534, 0.131,
         0.349, 0.686, 0.168,
         0.393, 0.769, 0.189);
    cv::transform(in, in, kernel);
}

void FakeWebcam::applyFishEye(cv::Mat &in)
{
    const double Cx = in.cols / 2.;
    const double Cy = in.rows / 2.;

    const double k = 0.00002;
    bool scale = false;

    cv::Mat src;
    in.copyTo(src);

    cv::Mat mapx = cv::Mat(src.size(), CV_32FC1);
    cv::Mat mapy = cv::Mat(src.size(), CV_32FC1);

    int w = src.cols;
    int h = src.rows;

    cv::Vec4f props;
    float xShift = FishEye::calc_shift(0, Cx - 1, Cx, k);
    props[0] = xShift;
    float newCenterX = w - Cx;
    float xShift2 = FishEye::calc_shift(0, newCenterX - 1, newCenterX, k);

    float yShift = FishEye::calc_shift(0, Cy - 1, Cy, k);
    props[1] = yShift;
    float newCenterY = w - Cy;
    float yShift2 = FishEye::calc_shift(0, newCenterY - 1, newCenterY, k);

    float xScale = (w - xShift - xShift2) / w;
    props[2] = xScale;
    float yScale = (h - yShift - yShift2) / h;
    props[3] = yScale;

    float *p = mapx.ptr<float>(0);

    for (int y = 0; y < h; y++)
    {
        for (int x = 0; x < w; x++)
        {
            *p++ = FishEye::getRadialX(x, y, Cx, Cy, k, scale, props);
        }
    }

    p = mapy.ptr<float>(0);
    for (int y = 0; y < h; y++)
    {
        for (int x = 0; x < w; x++)
        {
            *p++ = FishEye::getRadialY(x, y, Cx, Cy, k, scale, props);
        }
    }
    cv::remap(src, in, mapx, mapy, cv::INTER_LINEAR, cv::BORDER_CONSTANT);
}