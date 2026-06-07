#include <iostream>
#include <Shader/shader.h>
#include <glm/glm.hpp>
#include <entity.h>
#include <unordered_map> 

#include <Engine/sharedData.h>
#include <Engine/engine.h>
#include <Engine/inputHandler.h>
#include <Scripting/baseScript.h>
#include <config.h>

#include <GameScripts/controls.h>
#include <GameScripts/TextureManager/textureManager.h>
#include <GameScripts/floor.h>
#include <GameScripts/lander.h>
#include <GameScripts/goal.h>
#include <GameScripts/ai.h>

void initializeEngine();

bool humanPlayer = false;

int main() { 
    initializeEngine();
    textureManager::loadTextures();

    entity controls("controls");
    controls.addScript<controlsScript>();

    unsigned int inputs[] = {GLFW_KEY_ESCAPE, GLFW_MOUSE_BUTTON_1, GLFW_KEY_A, GLFW_KEY_W, GLFW_KEY_S, GLFW_KEY_D};
    inputHandler::trackKeys(&inputs[0], 6);

    entity floor("floor", glm::vec2(0, -200), glm::vec2(600, 50), 0);
    floor.addScript<floorScript>();

    entity* goal;
    entity* lander;
    entity* ai;
    if(humanPlayer)
    {
        goal = new entity("goal", glm::vec2(0, 0), glm::vec2(40, 50), 0);
        goal->addScript<goalScript>();

        lander = new entity("lander", glm::vec2(0, 300), glm::vec2(50, 50), 0);
        lander->addScript<landerScript>();
        ((landerScript*)lander->scripts[0])->goal = (goalScript*)goal->scripts[0];
    }
    else
    {
        ai = new entity("ai");
        ai->addScript<aiScript>();
    }

    // engine::grid.wantsDraw = true;
    engine::run();

    textureManager::deleteTextures();

    if(humanPlayer)
    {
        delete goal;
        delete lander;
    }
    else delete ai;

	return 0;
}

void initializeEngine()
{
    engine::setupWindow(600, 800);
    engine::initialize(ROOT_DIR);
    engine::initializeSpatialHashGrid(900, 700, glm::vec2(3, 3), glm::vec2(-400, -400));
    engine::initializeSolver(0.01f, 0.1f, 0.1f, 1.0f);
    engine::setBackgroundColor(0, 0, 0);

    Shader shader("gravityVShader", "gravityFShader");
    Shader pointShader("pointVShader", "pointFShader");
    Shader rayShader("rayVShader", "rayFShader");

    engine::configureShader(shader);
    engine::configureShader(pointShader);
    engine::configureShader(rayShader);
    engine::shared.mainShaderID = shader.ID;
    engine::shared.pointShaderID = pointShader.ID;
    engine::shared.rayShaderID = rayShader.ID;

    setup::angularDamping = 0.999f;
}