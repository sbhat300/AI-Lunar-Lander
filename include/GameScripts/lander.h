#ifndef LANDER_H
#define LANDER_H

#include <Engine/engine.h>
#include <Engine/sharedData.h>
#include <GameScripts/TextureManager/textureManager.h>

#include <GameScripts/goal.h>

class landerScript : public baseScript
{
    public:
        float enginePower = 600.0f;
        float landerVertices[16] = {
            1 / 2.0f,  1 / 2.0f, 1.0f, 1.0f,  // top right
            -1 / 2.0f,  1 / 2.0f, 0.0f, 1.0f,   // top left
            -1.0f, -1 / 2.0f, 0.0f, 0.0f, // bottom left 
            1.0f, -1 / 2.0f, 1.0f, 0.0f// bottom right
        };
        int landerIndices[6] = { 
                                0, 1, 3,  
                                1, 2, 3    
                            }; 
        float landerCollider[8] = {
            1 / 2.0f,  1 / 2.0f,
            -1 / 2.0f,  1 / 2.0f,
            -1.0f, -1 / 2.0f,
            1.0f, -1 / 2.0f 
        };
        goalScript* goal;
        float score;
        float angleWeight = 50.0f, velocityWeight = 100.0f, progressWeight = 500.0f, timePenalty = 0.3f, spinWeight = 30.0f;
        float startDist, lastDist;

        void collisionCallback(entity* first, entity* second, glm::vec2 collisionNormal, float penetrationDepth, int contactPoints, glm::vec2 cp1, glm::vec2 cp2)
        {
            engine::solver.registerCollision(first, second, contactPoints, collisionNormal, penetrationDepth, cp1, cp2);

            float velocitySq = (parent->rigidbody.velocity.x * parent->rigidbody.velocity.x + parent->rigidbody.velocity.y * parent->rigidbody.velocity.y);
            float velocity = velocitySq / 40000;
            float groundAngle = std::atan2(collisionNormal.y, collisionNormal.x) + (M_PI / 2.0f);
            float orientationMatch = std::cos(parent->rotation - groundAngle);
            float spin = parent->rigidbody.angularVelocity;
            float goalDist = glm::distance(parent->position, goal->goal);

            score -= velocityWeight * velocity;
            score += angleWeight * orientationMatch;
            score -= spinWeight * spin * spin;

            if(velocitySq < 400.0f && orientationMatch > 0.9f && std::abs(spin) < 1.5f && goalDist < 60) score += 1000.0f;

            std::cout << "Final score: " << score << std::endl;
        }

        void start() override
        {
            parent->addTag("lander");

            score = 0;
            parent->addPolygon();
            parent->polygonInstance.initPolygon(4, landerVertices, 6, landerIndices);
            parent->polygonInstance.polygonTexture = textureManager::defaultTexture;
            parent->polygonInstance.shaderProgram = engine::shared.mainShaderID;
            parent->polygonInstance.setColor(glm::vec3(0.7f, 0.7f, 0.7f));
            parent->addPolygonCollider();
            parent->collider.debugShaderProgram = engine::shared.pointShaderID;
            parent->collider.initPolygon(4, landerCollider);
            parent->collider.currentBitmask |= collisionLayer::LANDER;
            parent->collider.filterBitmask |= collisionLayer::LANDER;
            parent->addPolygonRigidbody(10, 0, 0, 0.3f);
            parent->rigidbody.setPolygonMomentOfInertia();


            parent->collider.setCollisionCallback([this](entity* first, entity* second, glm::vec2 collisionNormal, float penetrationDepth, int contactPoints, glm::vec2 cp1, glm::vec2 cp2){
                collisionCallback(first, second, collisionNormal, penetrationDepth, contactPoints, cp1, cp2);});
        }

        void preUpdate() override
        {
            startDist = glm::distance(parent->position, goal->goal);
            lastDist = 0;
        }

        void update() override
        {
        }

        void fixedUpdate() override
        {
            if(inputHandler::buttons[GLFW_KEY_A].down)
                parent->rigidbody.addForceAtPoint(-enginePower * std::sin(parent->rotation), enginePower * std::cos(parent->rotation), parent->collider.points[2].x, parent->collider.points[2].y);
            if(inputHandler::buttons[GLFW_KEY_D].down)
                parent->rigidbody.addForceAtPoint(-enginePower * std::sin(parent->rotation), enginePower * std::cos(parent->rotation), parent->collider.points[3].x, parent->collider.points[3].y);
            parent->rigidbody.gravity(50);
            
            float dist = glm::distance(parent->position, goal->goal);
            float progress = 1.0f - (dist / startDist);
            score += progressWeight * (progress - lastDist) - timePenalty;
            lastDist = progress;
        }

};

#endif