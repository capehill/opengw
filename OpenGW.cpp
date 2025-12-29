#include "defines.hpp"
#include "game.hpp"
#include "scene.hpp"

#include "blur.hpp"
#include "sincos.hpp"

#include <GL/glu.h>
#include <SDL.h>

#include <cstdio>
#include <memory>

// declare image buffers
#ifdef __amigaos4__
// MiniGL / Warp3D requires power of two size
static constexpr int blurBufferWidth = 256;
static constexpr int blurBufferHeight = 256;
#else
static constexpr int blurBufferWidth = 500;
static constexpr int blurBufferHeight = 250;
#endif

static std::vector<ColorRGB> blurBuffer(blurBufferWidth * blurBufferHeight);

static SDL_Window* window;
static SDL_GLContext context;

static void OGLCreate();
static void OGLDestroy();
static void OGLSize(int cx, int cy);

std::unique_ptr<scene> oglScene;

static bool oglInited = false;

// our OpenGL texture handles
static unsigned int texOffscreen;

static void createOffscreens();
static void drawOffscreens();
static void run();

#define CONTEXT_PRIMARY 0
#define CONTEXT_GLOW    1

static int mWidth, mHeight;

static Uint32 lastTime;
static Uint32 fpsTime;
static int frameCount;
static int fps;

const Uint8* keyboardState;

static bool handleEvents()
{
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        switch (e.type) {
        case SDL_QUIT:
            printf("Quit\n");
            return false;
        case SDL_WINDOWEVENT:
        {
            if (e.window.event == SDL_WINDOWEVENT_RESIZED) {
                // printf("%d %d\n", e.window.data1, e.window.data2);
                OGLSize(e.window.data1, e.window.data2);
            }
        } break;
        case SDL_CONTROLLERDEVICEADDED:
            theGame->mControls->handleControllerAdded(e.cdevice.which);
            break;
        case SDL_CONTROLLERDEVICEREMOVED:
            theGame->mControls->handleControllerRemoved(e.cdevice.which);
            break;
        }
    }

    keyboardState = SDL_GetKeyboardState(nullptr);
    return true;
}

int main(int /*argc*/, char** /*argv*/)
{
    printf("SDL_Init\n");
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER) < 0) {
        printf("SDL_Init failed: %s\n", SDL_GetError());
        return 0;
    }

    Uint32 flags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL;
    if (0) {
        flags |= SDL_WINDOW_FULLSCREEN;
    }

    window = SDL_CreateWindow("OpenGL SDL2", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              theGame->mSettings.displayWidth, theGame->mSettings.displayHeight, flags);

    if (window) {
        srand(SDL_GetTicks());
        make_sin_cos_tables();
        oglScene = std::make_unique<scene>();

        OGLCreate();
        run();
        OGLDestroy();

        oglScene.reset();

        SDL_DestroyWindow(window);
    } else {
        printf("Failed to create window: %s\n", SDL_GetError());
    }

    printf("SDL_Quit\n");
    SDL_Quit();

    return 0;
}

static void OGLCreate()
{
    context = SDL_GL_CreateContext(window);

    if (context == nullptr) {
        printf("SDL_GL_CreateContext failed: %s\n", SDL_GetError());
    }

    OGLSize(theGame->mSettings.displayWidth, theGame->mSettings.displayHeight);

    // Do stuff with the context here if needed...
    createOffscreens();

    if (SDL_GL_SetSwapInterval(0) == -1) {
        printf("SDL_GL_SetSwapInterval failed: %s\n", SDL_GetError());
    }

    if (SDL_GL_MakeCurrent(window, context) < 0) {
        printf("SDL_GL_MakeCurrent failed: %s\n", SDL_GetError());
    }

    oglInited = true;
}

static void OGLDestroy()
{
    oglInited = false;

    SDL_GL_MakeCurrent(nullptr, 0);
    SDL_GL_DeleteContext(context);
}

static void OGLSize(int cx, int cy)
{
    oglScene->size(cx, cy);
    mWidth = cx;
    mHeight = cy;
}

static void createOffscreens()
{
    std::vector<char> colorBits(blurBufferWidth * blurBufferHeight * 3);

    // texture creation..
    glGenTextures(1, &texOffscreen);
    glBindTexture(GL_TEXTURE_2D, texOffscreen);

    glTexImage2D(GL_TEXTURE_2D, 0, 3, blurBufferWidth, blurBufferHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, colorBits.data());
    gluBuild2DMipmaps(GL_TEXTURE_2D, 3, blurBufferWidth, blurBufferHeight, GL_RGB, GL_UNSIGNED_BYTE, colorBits.data());

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, 0);
}

static void drawOffscreens()
{
    int viewport[4];
    glGetIntegerv(GL_VIEWPORT, (int*)viewport);

    if (theGame->mSettings.mEnableGlow) {
        // Draw to the blur texture
        {
            glViewport(0, 0, blurBufferWidth, blurBufferHeight);

            oglScene->draw(scene::RENDERPASS_BLUR);

            // Transfer image to the blur texture
            glBindTexture(GL_TEXTURE_2D, texOffscreen);
            glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 0, 0, blurBufferWidth, blurBufferHeight, 0);
            glBindTexture(GL_TEXTURE_2D, 0);
        }
    }

    // Draw the scene normally
    glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
    oglScene->draw(scene::RENDERPASS_PRIMARY);

    if (theGame->mSettings.mEnableGlow) {
        ////////////////////////////////////////////////
        // Do blur

        // Bind the blur texture and copy the screen bits to it
        glBindTexture(GL_TEXTURE_2D, texOffscreen);
        glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, blurBuffer.data());

        const int blurRadius = (game::mGameMode == game::GAMEMODE_ATTRACT || game::mGameMode == game::GAMEMODE_CREDITED) ? 8 : 4;

        superFastBlur(blurBuffer.data(), blurBufferWidth, blurBufferHeight, blurRadius);
        superFastBlur(blurBuffer.data(), blurBufferWidth, blurBufferHeight, blurRadius);

        // Bind the blur result back to our texture
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, blurBufferWidth, blurBufferHeight, GL_RGB, GL_UNSIGNED_BYTE, blurBuffer.data());

        ////////////////////////////////////////////////
        // Draw the blur texture on top of the existing scene

        // Glowy blending effect
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);

        glEnable(GL_TEXTURE_2D);
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

        if (game::mGameMode == game::GAMEMODE_ATTRACT || game::mGameMode == game::GAMEMODE_CREDITED)
            glColor4f(1, 1, 1, 1);
        else
            glColor4f(1, 1, 1, 1);

        // Draw it on the screen
        glBegin(GL_QUADS);
        glTexCoord2d(0.0, 0.0);
        glVertex2d(-1.0, -1.0);
        glTexCoord2d(1.0, 0.0);
        glVertex2d(1.0, -1.0);
        glTexCoord2d(1.0, 1.0);
        glVertex2d(1.0, 1.0);
        glTexCoord2d(0.0, 1.0);
        glVertex2d(-1.0, 1.0);
        if (game::mGameMode == game::GAMEMODE_ATTRACT || game::mGameMode == game::GAMEMODE_CREDITED) {
            glTexCoord2d(0.0, 0.0);
            glVertex2d(-1.0, -1.0);
            glTexCoord2d(1.0, 0.0);
            glVertex2d(1.0, -1.0);
            glTexCoord2d(1.0, 1.0);
            glVertex2d(1.0, 1.0);
            glTexCoord2d(0.0, 1.0);
            glVertex2d(-1.0, 1.0);
        }
        glEnd();

        glBindTexture(GL_TEXTURE_2D, 0);
        glDisable(GL_TEXTURE_2D);
    }
}

static void updateFps(Uint32 now)
{
    ++frameCount;

    lastTime = now;

    if ((now - fpsTime) > 1000) {
        fpsTime = now;
        fps = (fps + frameCount) / 2;
        frameCount = 0;

        char buf[32];
        snprintf(buf, sizeof(buf), "OpenGW SDL2 - FPS %d", fps);
        SDL_SetWindowTitle(window, buf);
    }
}

static void run()
{
    if (!oglInited)
        return;

    constexpr Uint32 logicRate = 60;
    constexpr Uint32 logicPeriod = 1000 / logicRate;

    Uint32 lastLogicUpdate = SDL_GetTicks();

    bool running = true;

    while (running) {
        const Uint32 now = SDL_GetTicks();

        while ((now - lastLogicUpdate) > logicPeriod) {
            lastLogicUpdate += logicPeriod;
            if (!handleEvents()) {
                running = false;
            }

            oglScene->run();
        }

        drawOffscreens();

        SDL_GL_SwapWindow(window);
        updateFps(now);
    }
}
