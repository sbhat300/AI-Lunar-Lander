#ifndef TEXTUREMANAGER_H
#define TEXTUREMANAGER_H

#include <Rendering/texture.h>

namespace textureManager
{
    extern texture* defaultTexture;
    extern texture* goalTexture;

    void loadTextures();
    void deleteTextures();
}

#endif