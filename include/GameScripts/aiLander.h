
#ifndef AI_LANDER_H
#define AI_LANDER_H

#include <Engine/engine.h>
#include <Engine/sharedData.h>
#include <GameScripts/TextureManager/textureManager.h>

#include <GameScripts/goal.h>
#include <EngineIncludes/Physics/ray.h>
#include <GameScripts/collisionLayers.h>
#include <NEAT/neat.h>
#include <EngineIncludes/Engine/setup.h>

class aiLanderScript : public baseScript
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
        float angleWeight = 300.0f, velocityWeight = 500.0f, xProgressWeight = 1000.0f, yProgressWeight = 500.0f, timePenalty = 0.2f, spinWeight = 30.0f, uprightPenalty = 0.4f;
        float successBonus = 500.0f, failiurePenalty = 1000.0f;
        float timeLimit = 20.0f;
        NEAT::genome* genome = nullptr;
        NEAT* population = nullptr;
        bool dead;
        std::vector<float> inputs;
        float elapsedTime;
        float startXDist, startYDist, lastXDist, lastYDist;
        int speciesId;

        ray down, left, right; 

        void collisionCallback(entity* first, entity* second, glm::vec2 collisionNormal, float penetrationDepth, int contactPoints, glm::vec2 cp1, glm::vec2 cp2)
        {
            engine::solver.registerCollision(first, second, contactPoints, collisionNormal, penetrationDepth, cp1, cp2);
            
            if(!dead)
            {
                float velocitySq = (parent->rigidbody.velocity.x * parent->rigidbody.velocity.x + parent->rigidbody.velocity.y * parent->rigidbody.velocity.y);
                float velocity = velocitySq / 40000;
                float groundAngle = std::atan2(collisionNormal.y, collisionNormal.x) + (M_PI / 2.0f);
                float orientationMatch = std::cos(parent->rotation - groundAngle);
                float spin = parent->rigidbody.angularVelocity;
                float goalDist = glm::distance(parent->position, goal->goal);

                score -= velocityWeight * velocity;
                score += angleWeight * orientationMatch;
                score -= spinWeight * spin * spin;

                if(velocitySq < 1600.0f && orientationMatch > 0.7f && std::abs(spin) < 2.0) 
                {
                    score += successBonus * 3;

                    if(goalDist < 120.0f) 
                    {
                        score += successBonus * 3;

                        if(velocitySq < 400.0f && orientationMatch  > 0.9f && std::abs(spin) < 1.5f && goalDist < 50.0f)
                        {
                            score += successBonus * 6;
                        }
                    }
                }

                parent->rigidbody.velocity = glm::vec2(0.0f, 0.0f);
                parent->rigidbody.angularVelocity = 0.0f;
                parent->collider.collide = false;
                genome->fitness = score;
                dead = true;
            }
        }

        void start() override
        {
            parent->addTag("lander");

            float r = 0.5f + 0.5f * std::sin(speciesId * 1.3f);
            float g = 0.5f + 0.5f * std::sin(speciesId * 1.7f + 2.0f);
            float b = 0.5f + 0.5f * std::sin(speciesId * 2.1f + 4.0f);

            score = 0;

            parent->addPolygon();
            parent->polygonInstance.initPolygon(4, landerVertices, 6, landerIndices);
            parent->polygonInstance.polygonTexture = textureManager::defaultTexture;
            parent->polygonInstance.shaderProgram = engine::shared.mainShaderID;
            parent->polygonInstance.setColor(glm::vec3(r, g, b));
            parent->addPolygonCollider();
            parent->collider.debugShaderProgram = engine::shared.pointShaderID;
            parent->collider.initPolygon(4, landerCollider);
            parent->collider.currentBitmask |= collisionLayer::LANDER;
            parent->collider.filterBitmask |= collisionLayer::LANDER;
            parent->addPolygonRigidbody(10, 0, 0, 0.3f);
            parent->rigidbody.setPolygonMomentOfInertia();
            dead = false;

            down = ray(glm::vec2(0, 0), glm::vec2(0, -1), 1000, &engine::grid);
            left = ray(glm::vec2(0, 0), glm::vec2(-1, -2), 1000, &engine::grid);
            right = ray(glm::vec2(0, 0), glm::vec2(1, -2), 1000, &engine::grid);

            down.filterBitmask |= collisionLayer::LANDER;
            left.filterBitmask |= collisionLayer::LANDER;
            right.filterBitmask |= collisionLayer::LANDER;

            parent->collider.setCollisionCallback([this](entity* first, entity* second, glm::vec2 collisionNormal, float penetrationDepth, int contactPoints, glm::vec2 cp1, glm::vec2 cp2){
                collisionCallback(first, second, collisionNormal, penetrationDepth, contactPoints, cp1, cp2);});

            elapsedTime = 0.0f;

            inputs.resize(10);
        }

        void preUpdate() override
        {
            startXDist = std::abs(parent->position.x - goal->goal.x);
            startYDist = std::abs(parent->position.y - goal->goal.y);
            lastXDist = 0;
            lastYDist = 0;
        }

        void update() override
        {
        }

        void fixedUpdate() override
        {
            if(!dead)
            {
                elapsedTime += setup::fixedDeltaTime;
                if(elapsedTime >= timeLimit)
                {
                    parent->rigidbody.velocity = glm::vec2(0.0f, 0.0f);
                    parent->rigidbody.angularVelocity = 0.0f;
                    parent->collider.collide = false;
                    score -= failiurePenalty;
                    genome->fitness = score;
                    dead = true;
                    return;
                }
                down.origin = parent->position;
                left.origin = parent->position;
                right.origin = parent->position;
                
                float sinRot = std::sin(parent->rotation);
                float cosRot = std::cos(parent->rotation);
                down.setDirection(sinRot, -cosRot);
                left.setDirection(-cosRot + 2 * sinRot, -sinRot - 2 * cosRot);
                right.setDirection(cosRot + 2 * sinRot, sinRot - 2 * cosRot);

                std::pair<bool, rayData> dCol = down.getFirstCollision();
                std::pair<bool, rayData> lCol = left.getFirstCollision();
                std::pair<bool, rayData> rCol = right.getFirstCollision();

                inputs[0] = parent->rigidbody.velocity.x / 200.0f;
                inputs[1] = parent->rigidbody.velocity.y / 200.0f;
                inputs[2] = parent->rigidbody.angularVelocity / 5.0f;
                inputs[3] = std::sin(parent->rotation);
                inputs[4] = std::cos(parent->rotation);
                inputs[5] = (goal->goal.x - parent->position.x) / 400.0f;
                inputs[6] = (goal->goal.y - parent->position.y) / 300.0f;
                inputs[7] = dCol.first ? 1.0f - glm::distance(parent->position, dCol.second.collisionPoint) / 1000.0f : 0;
                inputs[8] = lCol.first ? 1.0f - glm::distance(parent->position, lCol.second.collisionPoint) / 1000.0f : 0;
                inputs[9] = rCol.first ? 1.0f - glm::distance(parent->position, rCol.second.collisionPoint) / 1000.0f : 0;

                std::vector<float> out = population->feedForward(*genome, inputs, NEAT::sigmoid);
                float leftThrust = out[0] * enginePower;
                parent->rigidbody.addForceAtPoint(-leftThrust * sinRot, leftThrust * cosRot, parent->collider.points[2].x, parent->collider.points[2].y);
                float rightThrust = out[1] * enginePower;
                parent->rigidbody.addForceAtPoint(-rightThrust * sinRot, rightThrust * cosRot, parent->collider.points[3].x, parent->collider.points[3].y);
                parent->rigidbody.gravity(50);
                
                float xDist = startXDist > 0 ? 1.0f - std::abs(parent->position.x - goal->goal.x) / startXDist : 1;
                float yDist = startYDist > 0 ? 1.0f - std::abs(parent->position.y - goal->goal.y) / startYDist : 1;
                float xReward = (xDist - lastXDist) * xProgressWeight;
                float yReward = (yDist - lastYDist) * yProgressWeight;
                
                score += xReward + yReward - timePenalty;

                float uprightMatch = std::cos(parent->rotation); 
                if (uprightMatch < 0.5f) 
                {
                    score -= uprightPenalty; 
                }
                lastXDist = xDist;
                lastYDist = yDist;
            }
        }

};

#endif