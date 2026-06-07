#include "textureManager.h"
#include <Rendering/texture.h>
#include <FileLoader/fileLoader.h>
#include <Engine/setup.h>

texture* textureManager::defaultTexture = 0;
texture* textureManager::goalTexture = 0;

void textureManager::loadTextures()
{
    defaultTexture = new texture();
    goalTexture = new texture();
    goalTexture->loadTexture(fileLoader::loadTGA("goal", setup::windows).c_str());
}

void textureManager::deleteTextures()
{
    delete defaultTexture;
    delete goalTexture;
}