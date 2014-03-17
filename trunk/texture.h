#pragma once

class texture
{
public:
    texture(void);
    ~texture(void);

    void load(const char* filename);
    void draw(float x, float y, float width, float height);

    void bind();
    void unbind();

    unsigned mWidth;
    unsigned mHeight;

    bool mLoaded;

private:
    unsigned int mTextureId;
};
