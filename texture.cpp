#include "texture.hpp"
#include "lodepng.h"

#include <SDL3/SDL_opengl.h>

texture::texture(void)
{
    mWidth = 0;
    mHeight = 0;
    mTextureId = 0;
    mLoaded = false;
}

texture::~texture(void)
{
}

void texture::load(const char* filename)
{
    mLoaded = true;

    // Load file and decode image.
    static std::vector<unsigned char> image;

    unsigned error = lodepng::decode(image, mWidth, mHeight, filename);

    // If there's an error, display it.
    if (error != 0) {
        printf("texture::load(%s) error: %s\n", filename, lodepng_error_text(error));
        return;
    }

    glGenTextures(1, &mTextureId);
    glBindTexture(GL_TEXTURE_2D, mTextureId);
    glTexImage2D(GL_TEXTURE_2D, 0, 4, mWidth, mHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, &image[0]);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, 0);

    image.clear();
}

void texture::draw(float x, float y, float width, float height)
{
    // Glowy blending effect
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    // Enable the texture for OpenGL
    glEnable(GL_TEXTURE_2D);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    glBindTexture(GL_TEXTURE_2D, mTextureId);

    glColor4f(1, 1, 1, 1);

    glBegin(GL_QUADS);
    glTexCoord2d(0, 1);
    glVertex2d(x - (width / 2), y - (height / 2));
    glTexCoord2d(1, 1);
    glVertex2d(x + (width / 2), y - (height / 2));
    glTexCoord2d(1, 0);
    glVertex2d(x + (width / 2), y + (height / 2));
    glTexCoord2d(0, 0);
    glVertex2d(x - (width / 2), y + (height / 2));
    glEnd();

    glBindTexture(GL_TEXTURE_2D, 0);

    glDisable(GL_TEXTURE_2D);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
}

void texture::bind()
{
    glEnable(GL_TEXTURE_2D);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glBindTexture(GL_TEXTURE_2D, mTextureId);
}

void texture::unbind()
{
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);
}
