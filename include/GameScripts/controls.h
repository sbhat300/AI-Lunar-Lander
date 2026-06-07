#ifndef CONTROLS_H
#define CONTROLS_H

#include <Engine/inputHandler.h>
#include <Engine/engine.h>
#include <ImguiImplementation/imguiInitialize.h>
#include <Scripting/baseScript.h>

class controlsScript : public baseScript 
{
    public:
        void update() override 
        {
            ImGuiIO& io = ImGui::GetIO();
            if (inputHandler::buttons[GLFW_KEY_ESCAPE].down)
                engine::shutdown();

            if(!io.WantCaptureMouse && inputHandler::buttons[GLFW_MOUSE_BUTTON_1].pressed)
            {
                gui::currentID = engine::grid.testPoint(inputHandler::worldMousePos.x, inputHandler::worldMousePos.y);
            }
        }
};

#endif