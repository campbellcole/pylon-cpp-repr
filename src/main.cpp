#include <pylon/PylonIncludes.h>
#include <pylon/BaslerUniversalInstantCamera.h>
#include <chrono>

#define READOUT_FPS
#define READOUT_TIMINGS

using std::cout;
using std::endl;
using std::chrono::steady_clock;
using namespace Pylon;
using namespace Basler_UniversalCameraParams;

template <
    class result_t = std::chrono::milliseconds,
    class clock_t = std::chrono::steady_clock,
    class duration_t = std::chrono::milliseconds>
auto since(std::chrono::time_point<clock_t, duration_t> const &start)
{
    return std::chrono::duration_cast<result_t>(clock_t::now() - start);
}

int main()
{
    Pylon::PylonAutoInitTerm autoInitTerm;

    auto exitCode = 0;

    try
    {
        cout << "Connecting to camera..." << endl;

        CBaslerUniversalInstantCamera camera(CTlFactory::GetInstance().CreateFirstDevice());

        auto cameraInfo = camera.GetDeviceInfo();

        cout << "Using device: " << cameraInfo.GetFriendlyName() << endl;

        camera.RegisterConfiguration(new CSoftwareTriggerConfiguration(), RegistrationMode_ReplaceAll, Cleanup_Delete);

        camera.Open();

        camera.ExposureAuto = ExposureAuto_Off;
        camera.ExposureMode = ExposureMode_Timed;
        camera.ExposureTime = 10000.0;

        camera.LineSelector = LineSelector_Line2;
        camera.LineSource = LineSource_UserOutput1;
        camera.UserOutputSelector = UserOutputSelector_UserOutput1;
        camera.UserOutputValue = false;

        camera.MaxNumBuffer = 200;
        camera.MaxNumGrabResults = 200;

        cout << "Resulting frame rate: " << camera.ResultingFrameRate.GetValue() << endl;

        camera.StartGrabbing(GrabStrategy_OneByOne);

        bool outputValue = true;

        CGrabResultPtr ptrGrabResult;

#ifdef READOUT_FPS
        int frameCounter = 0;
        auto lastSecond = steady_clock::now();
#endif

        while (camera.IsGrabbing())
        {
            camera.UserOutputValue = !outputValue;

#ifdef READOUT_TIMINGS
            auto start = steady_clock::now();
#endif
            if (camera.WaitForFrameTriggerReady(200, TimeoutHandling_ThrowException))
            {
#ifdef READOUT_TIMINGS
                cout << "Trigger ready in " << since(start).count() << "ms" << endl;
                start = steady_clock::now();
#endif
                camera.ExecuteSoftwareTrigger();
#ifdef READOUT_TIMINGS
                cout << "Trigger executed in " << since(start).count() << "ms" << endl;
                start = steady_clock::now();
#endif
            }

            camera.RetrieveResult(200, ptrGrabResult, TimeoutHandling_ThrowException);
#ifdef READOUT_TIMINGS
            cout << "Retrieved result in " << since(start).count() << "ms" << endl;
#endif

            if (ptrGrabResult->GrabSucceeded())
            {
                const uint8_t *buf = (uint8_t *)ptrGrabResult->GetBuffer();
                const uint8_t *last = buf + ptrGrabResult->GetImageSize();

                outputValue = !outputValue;
            }
            else
            {
                cout << "Error: "
                     << std::hex
                     << ptrGrabResult->GetErrorCode()
                     << std::dec
                     << " "
                     << ptrGrabResult->GetErrorDescription()
                     << endl;
            }
#ifdef READOUT_FPS
            if (since(lastSecond).count() >= 1000)
            {
                cout << "FPS: " << frameCounter << endl;
                frameCounter = 0;
                lastSecond = steady_clock::now();
            }
            else
            {
                frameCounter++;
            }
#endif
        }
    }
    catch (const GenericException &e)
    {
        cout << "Error: " << e.GetDescription() << endl;
        exitCode = 1;
    }

    return exitCode;
}