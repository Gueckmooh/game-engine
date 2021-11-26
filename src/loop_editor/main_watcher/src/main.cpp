#include <cmath>
// #include <ctime>
#include <chrono>
#include <dlfcn.h>
#include <file_watcher/file_watcher.hpp>
#include <filesystem>
#include <iostream>
#include <main_loop/game_data.hpp>
#include <memory>
#include <thread>

#include <audio/audio_engine.hpp>
// #include <audio/examples/examples.hpp>
#include <audio/sound_data.hpp>
#include <logging/logger.hpp>
#include <window/bitmap.hpp>
// #include <window/examples/examples.hpp>
#include <window/input.hpp>
#include <window/video_mode.hpp>
#include <window/window.hpp>

using namespace game_data;

using namespace std::chrono_literals;

using namespace draw::basics;
using namespace glm_compat;

namespace {
void waitToBeOnTime(std::chrono::duration<double> targetSecondsPerFrame) {
    static auto lastCounter = std::chrono::high_resolution_clock::now();
    auto workCounter        = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> workSecondsElapsed = workCounter - lastCounter;
    std::chrono::duration<double> secondsElapsed     = workSecondsElapsed;
    if (secondsElapsed < targetSecondsPerFrame) {
        while (secondsElapsed < targetSecondsPerFrame) {
            secondsElapsed = std::chrono::high_resolution_clock::now() - lastCounter;
        }
    }
    lastCounter = std::chrono::high_resolution_clock::now();
}

void initLogger() {
    logging::logger.enable("sound");
    logging::logger.enable("window");
    logging::logger.enable("main-watcher");
    // logging::logger.enable("main");
}

void computeFPS() {
    static auto begin                     = std::chrono::high_resolution_clock::now();
    auto end                              = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - begin;
    double msPerFrame                     = elapsed.count() * 1000;   // / 1'000'000;
    double fps                            = 1000 / msPerFrame;
    logging::logger.info("main.loop.fps")
        << (int)round(fps) << "fps " << (msPerFrame) << "mspf" << std::endl;
    begin = std::chrono::high_resolution_clock::now();
}

#ifdef __MAIN_LOOP_SHARED_LIBRARY__
typedef void (*initInputManagerTy)(window::input::InputManager& inputManager);
// typedef void (*processInputsTy)(window::input::InputManager& inputManager, GameData&
// gd); typedef void (*renderBitmapTy)(window::BitMap& bitmap, GameData& gd);
typedef void (*gameUpdateAndRenderTy)(window::BitMap& bitmap, game_data::GameData& gd,
                                      window::input::InputManager& im);
typedef void (*initializeGameTy)();

void initInputManagerStub(window::input::InputManager&) {}
initInputManagerTy initInputManager = initInputManagerStub;

// void processInputsStub(window::input::InputManager&, GameData&) {}
// processInputsTy processInputs = processInputsStub;

// void renderBitmapStub(window::BitMap&, GameData&) {}
// renderBitmapTy renderBitmap = renderBitmapStub;

void gameUpdateAndRenderStub(window::BitMap&, game_data::GameData&,
                             window::input::InputManager&) {}
gameUpdateAndRenderTy gameUpdateAndRender = gameUpdateAndRenderStub;

void initializeGameStub() {}
initializeGameTy initializeGame = initializeGameStub;

void* libHandle;
bool loadLibrary(std::string& filename) {
    std::string myFilename{ "/tmp/engine_main_loop.so" };
    std::filesystem::copy_file(filename, myFilename,
                               std::filesystem::copy_options::overwrite_existing);

    void* handle = dlopen(myFilename.c_str(), RTLD_LAZY);
    if (!handle) {
        std::cerr << "Cannot open library " << myFilename << std::endl;
        return false;
    }

    dlerror();
    {
        gameUpdateAndRenderTy fcnHandle =
            (gameUpdateAndRenderTy)dlsym(handle, "gameUpdateAndRender");
        const char* dlsymError = dlerror();
        if (dlsymError) {
            std::cerr << "Cannot load foo: " << dlsymError << std::endl;
            dlclose(handle);
            goto error;
        }
        gameUpdateAndRender = fcnHandle;
    }

    {
        initInputManagerTy fcnHandle =
            (initInputManagerTy)dlsym(handle, "initInputManager");
        const char* dlsymError = dlerror();
        if (dlsymError) {
            std::cerr << "Cannot load foo: " << dlsymError << std::endl;
            dlclose(handle);
            goto error;
        }
        initInputManager = fcnHandle;
    }

    {
        initializeGameTy fcnHandle = (initializeGameTy)dlsym(handle, "initializeGame");
        const char* dlsymError     = dlerror();
        if (dlsymError) {
            std::cerr << "Cannot load foo: " << dlsymError << std::endl;
            dlclose(handle);
            goto error;
        }
        initializeGame = fcnHandle;
    }
    // {
    //     processInputsTy fcnHandle = (processInputsTy)dlsym(handle, "processInputs");
    //     const char* dlsymError    = dlerror();
    //     if (dlsymError) {
    //         std::cerr << "Cannot load foo: " << dlsymError << std::endl;
    //         dlclose(handle);
    //         goto error;
    //     }
    //     processInputs = fcnHandle;
    // }
    // {
    //     renderBitmapTy fcnHandle = (renderBitmapTy)dlsym(handle, "renderBitmap");
    //     const char* dlsymError   = dlerror();
    //     if (dlsymError) {
    //         std::cerr << "Cannot load foo: " << dlsymError << std::endl;
    //         dlclose(handle);
    //         goto error;
    //     }
    //     renderBitmap = fcnHandle;
    // }
    libHandle = handle;

    return true;
error:
    initInputManager = initInputManagerStub;
    // processInputs    = processInputsStub;
    // renderBitmap     = renderBitmapStub;
    gameUpdateAndRender = gameUpdateAndRenderStub;
    return false;
}

void unloadLibrary() {
    initInputManager = initInputManagerStub;
    // processInputs    = processInputsStub;
    // renderBitmap     = renderBitmapStub;
    gameUpdateAndRender = gameUpdateAndRenderStub;
    initializeGame      = initializeGameStub;
    if (libHandle) dlclose(libHandle);
    libHandle = nullptr;
}
#else
extern "C" void initInputManager(window::input::InputManager& inputManager);
extern "C" void gameUpdateAndRender(window::BitMap& bitmap, game_data::GameData& gd,
                                    window::input::InputManager& im);
extern "C" void initializeGame();
#endif

}   // namespace

int main() {
#ifdef __MAIN_LOOP_SHARED_LIBRARY__
    file_watcher::FileWatcher watcher;
    std::string libName{ "./build/lib/new_libloop_editor_main_loop.so" };
    watcher.watch(libName);
#endif

    initLogger();
#ifdef __MAIN_LOOP_SHARED_LIBRARY__
    if (!loadLibrary(libName)) {
        std::cerr << "Could not load library..." << std::endl;
        return 1;
    }
#endif

    window::Window win{ { 960, 540 }, "toto", window::Window::NO_RESIZE };
    auto& inputManager = win.inputManager();
    inputManager.addMapping("record",
                            window::input::Input(window::input::key::KeyboardKey::L));
    inputManager.dontRecord("record");
    initInputManager(inputManager);
    initializeGame();

    int monitorRefreshHz             = 60;
    int gameUpdateHz                 = monitorRefreshHz;
    double targetMiliSecondsPerFrame = 1000.0f / (double)gameUpdateHz;

    GameData gd{
        .player = Player(4.5f, 3.0f),
    };

    GameData savedGd;
    auto& bitmap       = win.bitMap();
    bool recording     = false;
    bool replaying     = false;
    int recordedFrames = 0;
    int replayedFrames = 0;
    while (true) {
#ifdef __MAIN_LOOP_SHARED_LIBRARY__
        bool reload = false;
        while (auto event = watcher.pollEvent()) {
            if (event->type == file_watcher::FileWatchEvent::Type::Modified) {
                reload = true;
            }
        }
        if (reload) {
            logging::logger.info("main-watcher") << "reload game code" << std::endl;
            unloadLibrary();
            loadLibrary(libName);
            reload = false;
        }
#endif

        if (recording) {
            // @todo find a way to record and do not lose data...
            recordedFrames++;
        } else if (replaying) {
            replayedFrames++;
            if (replayedFrames >= recordedFrames) {
                replayedFrames = 0;
                gd             = savedGd;
                inputManager.startReplay();
            }
            inputManager.readInput();
        }

        if (inputManager.isDown("record")) {
            if (replaying) {
                replaying = false;
                inputManager.stopReplay();
            } else if (recording) {
                gd = savedGd;
                inputManager.stopRecord();
                inputManager.startReplay();
                recording      = false;
                replaying      = true;
                replayedFrames = 0;
            } else {
                savedGd = gd;
                inputManager.startRecord();
                recording      = true;
                recordedFrames = 0;
            }
        }

        inputManager.computeInput();

        // processInputs(inputManager, gd);
        // renderBitmap(bitmap, gd);
        gameUpdateAndRender(bitmap, gd, inputManager);

        waitToBeOnTime(
            std::chrono::duration<double, std::milli>(targetMiliSecondsPerFrame));
        win.update();

        computeFPS();
    }
    return 0;
}
