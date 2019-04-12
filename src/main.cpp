#include "FakeWebcam.hpp"

int main(int argc, char const *argv[])
{
    FakeWebcam cam;
    if (argc > 1)
    {
        cam.readConfig(argv[1]);
    }
    cam.run();
    return 0;
}