#ifndef GOAL_H
#define GOAL_H

#include <Engine/engine.h>
#include <Engine/sharedData.h>
#include <GameScripts/TextureManager/textureManager.h>
#include <random>
#include <Physics/ray.h>
#include <GameScripts/collisionLayers.h>

class goalScript : public baseScript
{
    public:
        std::random_device rd;
        std::mt19937 gen;
        ray r;
        std::uniform_real_distribution<float> distr;
        glm::vec2 goal;

        void setRandomPosition()
        {
            float x = distr(gen);
            r.origin = glm::vec2(x, 200);
            r.length = 1000;
            r.direction = glm::vec2(0, -1);

            std::pair<bool, rayData> point = r.getFirstCollision();
            
            goal = glm::vec2(point.second.collisionPoint.x, point.second.collisionPoint.y);
            parent->setPosition(x, point.second.collisionPoint.y + 25);
        }
        
        void start() override
        {
            gen = std::mt19937(rd());
            distr = std::uniform_real_distribution<float>(-340, 340);
            r = ray(glm::vec2(0, 0), glm::vec2(0, -1), 0, &engine::grid);
            r.filterBitmask |= collisionLayer::LANDER;
            parent->addPolygon();
            parent->polygonInstance.initRectangle();
            parent->polygonInstance.polygonTexture = textureManager::goalTexture;
            parent->polygonInstance.shaderProgram = engine::shared.mainShaderID;
            parent->polygonInstance.setLayer(0);
            setRandomPosition();
        }

        void update() override
        {
        }

        void fixedUpdate() override
        {

        }
};

#endif