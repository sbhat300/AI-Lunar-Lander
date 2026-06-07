#ifndef FLOOR_H
#define FLOOR_H

#include <Engine/engine.h>
#include <Engine/sharedData.h>
#include <GameScripts/TextureManager/textureManager.h>

class floorScript : public baseScript
{
    public: 
        entity* floorSegments[5];
        float rightSlopeVertices[16] = {
            1 / 2.0f,  2.0f, 1.0f, 1.0f,  // top right
            -1 / 2.0f,  1 / 2.0f, 0.0f, 1.0f,   // top left
            -1 / 2.0f, -1 / 2.0f, 0.0f, 0.0f, // bottom left 
            1 / 2.0f, -1 / 2.0f, 1.0f, 0.0f// bottom right
        };
        int rectIndices[6] = { 
                                0, 1, 3,  
                                1, 2, 3    
                            }; 
        float rightSlopeCollider[8] = {
            1 / 2.0f,  2.0f,
            -1 / 2.0f,  1 / 2.0f,
            -1 / 2.0f, -1 / 2.0f,
            1 / 2.0f, -1 / 2.0f 
        };
        float leftSlopeVertices[16] = {
            1 / 2.0f,  1 / 2.0f, 1.0f, 1.0f,  // top right
            -1 / 2.0f,  2.0f, 0.0f, 1.0f,   // top left
            -1 / 2.0f, -1 / 2.0f, 0.0f, 0.0f, // bottom left 
            1 / 2.0f, -1 / 2.0f, 1.0f, 0.0f// bottom right
        };
        float leftSlopeCollider[8] = {
            1 / 2.0f,  1 / 2.0f,
            -1 / 2.0f,  2.0f,
            -1 / 2.0f, -1 / 2.0f,
            1 / 2.0f, -1 / 2.0f 
        };
        void start() override
        {
            floorSegments[0] = new entity("main floor", glm::vec2(-75, -300), glm::vec2(150, 200), 0);
            floorSegments[0]->addPolygon();
            floorSegments[0]->polygonInstance.initRectangle();
            floorSegments[0]->polygonInstance.polygonTexture = textureManager::defaultTexture;
            floorSegments[0]->polygonInstance.shaderProgram = engine::shared.mainShaderID;
            floorSegments[0]->polygonInstance.setColor(glm::vec3(0.9f, 0.9f, 0.9f));
            floorSegments[0]->addPolygonCollider();
            floorSegments[0]->collider.initRectangle();
            floorSegments[0]->addPolygonRigidbody(0, 0, 0, 0.2f);
            floorSegments[0]->polygonInstance.setLayer(7);

            floorSegments[1] = new entity("right side sloped", glm::vec2(82, -232), glm::vec2(250, 150), 0);
            floorSegments[1]->addPolygon();
            floorSegments[1]->polygonInstance.initPolygon(4, rightSlopeVertices, 6, rectIndices);
            floorSegments[1]->polygonInstance.polygonTexture = textureManager::defaultTexture;
            floorSegments[1]->polygonInstance.shaderProgram = engine::shared.mainShaderID;
            floorSegments[1]->polygonInstance.setColor(glm::vec3(0.9f, 0.9f, 0.9f));
            floorSegments[1]->addPolygonCollider();
            floorSegments[1]->collider.initPolygon(4, rightSlopeCollider);
            floorSegments[1]->addPolygonRigidbody(0, 0, 0, 0.2f);
            floorSegments[1]->polygonInstance.setLayer(7);

            floorSegments[2] = new entity("right floor", glm::vec2(298, -204), glm::vec2(270, 300), 0);
            floorSegments[2]->addPolygon();
            floorSegments[2]->polygonInstance.initRectangle();
            floorSegments[2]->polygonInstance.polygonTexture = textureManager::defaultTexture;
            floorSegments[2]->polygonInstance.shaderProgram = engine::shared.mainShaderID;
            floorSegments[2]->polygonInstance.setColor(glm::vec3(0.9f, 0.9f, 0.9f));
            floorSegments[2]->addPolygonCollider();
            floorSegments[2]->collider.initRectangle();
            floorSegments[2]->addPolygonRigidbody(0, 0, 0, 0.2f);
            floorSegments[2]->polygonInstance.setLayer(7);

            floorSegments[3] = new entity("left side up", glm::vec2(-155, -301), glm::vec2(250, 150), 0);
            floorSegments[3]->addPolygon();
            floorSegments[3]->polygonInstance.initPolygon(4, leftSlopeVertices, 6, rectIndices);
            floorSegments[3]->polygonInstance.polygonTexture = textureManager::defaultTexture;
            floorSegments[3]->polygonInstance.shaderProgram = engine::shared.mainShaderID;
            floorSegments[3]->polygonInstance.setColor(glm::vec3(0.9f, 0.9f, 0.9f));
            floorSegments[3]->addPolygonCollider();
            floorSegments[3]->collider.initPolygon(4, leftSlopeCollider);
            floorSegments[3]->addPolygonRigidbody(0, 0, 0, 0.2f);
            floorSegments[3]->polygonInstance.setLayer(7);

            floorSegments[4] = new entity("left slope down", glm::vec2(-350, -302), glm::vec2(351, 150), 0);
            floorSegments[4]->addPolygon();
            floorSegments[4]->polygonInstance.initPolygon(4, rightSlopeVertices, 6, rectIndices);
            floorSegments[4]->polygonInstance.polygonTexture = textureManager::defaultTexture;
            floorSegments[4]->polygonInstance.shaderProgram = engine::shared.mainShaderID;
            floorSegments[4]->polygonInstance.setColor(glm::vec3(0.9f, 0.9f, 0.9f));
            floorSegments[4]->addPolygonCollider();
            floorSegments[4]->collider.initPolygon(4, rightSlopeCollider);
            floorSegments[4]->addPolygonRigidbody(0, 0, 0, 0.2f);
            floorSegments[4]->polygonInstance.setLayer(7);

            for(int i = 0; i < 5; i++)
            {
                floorSegments[i]->collider.debugShaderProgram = engine::shared.pointShaderID;
                floorSegments[i]->addTag("floor");
            }
        }

        ~floorScript() override
        {
            for(int i = 0; i < 5; i++)
            {
                delete floorSegments[i];
            }

        }
};

#endif