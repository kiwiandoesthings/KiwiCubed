#include "UI.h"
#include "Events.h"


UI::UI() : uiShaderProgram(nullptr) {}
UI::~UI() = default;

UI& UI::GetInstance() {
    static UI instance;
    return instance;
}

void UI::Setup(Shader* shaderProgram, Texture* atlas) {
    OVERRIDE_LOG_NAME("UI Render Components Setup");
    if (!renderComponentsSetup) {
        vertexBufferObject.SetupBuffer();
        vertexArrayObject.SetupArrayObject();
        indexBufferObject.SetupBuffer();
    
        renderComponentsSetup = true;
    } else {
        ERR("Tried to setup render components when they were already setup, aborting");
        return;
    }

    uiShaderProgram = shaderProgram;
    uiAtlas = atlas;

    inputHandler.SetupCallbacks(Window::GetInstance().GetWindowInstance());
    inputHandler.RegisterMouseButtonCallback(GLFW_MOUSE_BUTTON_LEFT, [&](int button){
        for (int iterator = 0; iterator < currentScreen->uiElements.size(); ++iterator) {
            if (currentScreen->uiElements[iterator]->OnClick()) {
                return;
            }
        }
    });
}

void UI::Render() {
    uiAtlas->SetActive();
    uiAtlas->Bind();
    currentScreen->Render();
}

void UI::AddScreen(UIScreen* screen) {
    uiScreens.emplace_back(screen);
}

void UI::SetCurrentScreen(UIScreen* screen) {
    currentScreen = screen;
}

UIScreen* UI::GetScreen(std::string& screenName) {
    for (int iterator = 0; iterator < uiScreens.size(); ++iterator) {
        if (uiScreens[iterator]->screenName == screenName) {
            return uiScreens[iterator];
        }
    }
    return nullptr;
}

UIScreen* UI::GetCurrentScreen() {
    return currentScreen;
}

std::string UI::GetCurrentScreenName() {
    return currentScreen->screenName;
}

InputHandler& UI::GetInputHandler() {
    return inputHandler;
}

UIScreen::UIScreen(std::string screenName) : screenName(screenName) {
    OVERRIDE_LOG_NAME("UI Initialization");
    INFO("Created screen \"" + screenName + "\"");
}

void UIScreen::Render() {
    for (int i = 0; i < uiElements.size(); i++) {
        uiElements[i]->Render();
    }
}

void UIScreen::AddUIElement(UIElement* element) {
    uiElements.emplace_back(element);
}

glm::vec2 UIElement::PixelsToNDC(glm::vec2 pixelPosition) {
    return glm::vec2((pixelPosition.x / Window::GetInstance().GetWidth()) * 2 - 1, (pixelPosition.y / Window::GetInstance().GetHeight()) * 2 - 1);
}

void UIElement::Render() {
    GLfloat vertices[] = {
        // Positions      // Texture Coordinates
        0.0f, 0.0f, 0.0f, 0.0f,
	    1.0f, 0.0f, 1.0f, 0.0f,
	    1.0f, 1.0f, 1.0f, 1.0f,
	    0.0f, 1.0f, 0.0f, 1.0f
    };

    GLuint indices[] = {
        0, 1, 2,
	    2, 3, 0,
    };

    UI& ui = UI::GetInstance();

    ui.vertexArrayObject.Bind();
    ui.vertexBufferObject.Bind();
    ui.vertexBufferObject.SetBufferData(sizeof(vertices), vertices);
    ui.vertexArrayObject.LinkAttribute(ui.vertexBufferObject, 0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (void*)0);
    ui.vertexArrayObject.LinkAttribute(ui.vertexBufferObject, 1, 2, GL_FLOAT, false, sizeof(float) * 4, (void*)(sizeof(float) * 2));
    ui.indexBufferObject.Bind();
    ui.indexBufferObject.SetBufferData(sizeof(indices), indices);

    ui.uiShaderProgram->Bind();
    ui.vertexArrayObject.Bind();
    ui.vertexBufferObject.Bind();
    ui.indexBufferObject.Bind();

    if (GetHovered()) {
        if (UI::GetInstance().GetInputHandler().GetMouseButtonState(GLFW_MOUSE_BUTTON_LEFT)) {
            textureIndex = 2;
        } else {
            textureIndex = 1;
        }
    } else {
        textureIndex = 0;
    }

    glm::mat4 modelMatrix = glm::mat4(1.0f);
    glm::vec2 ndcPosition = PixelsToNDC(position);
    glm::vec2 ndcSize = PixelsToNDC(size) + glm::vec2(1, 1);
    modelMatrix = glm::translate(modelMatrix, glm::vec3(ndcPosition.x, ndcPosition.y, 0.0f));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(ndcSize.x, ndcSize.y, 1.0f));
    ui.uiShaderProgram->SetUniformMatrix4fv("modelMatrix", modelMatrix);
    ui.uiShaderProgram->SetUniform1ui("textureIndex", textureIndex);

    GLCall(glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(sizeof(indices)), GL_UNSIGNED_INT, 0));
}

bool UIElement::OnClick() {
    if (!GetHovered() || eventToTrigger == "") {
        return false;
    }

    EventManager::GetInstance().TriggerEvent(eventToTrigger);
    return true;
}

bool UIElement::GetHovered() {
    glm::vec2 mousePosition = UI::GetInstance().GetInputHandler().GetMousePosition();
    int windowHeight = Window::GetInstance().windowHeight;
    mousePosition.y = windowHeight - mousePosition.y;

    return (mousePosition.x >= position.x && mousePosition.y >= position.y && mousePosition.x <= position.x + size.x && mousePosition.y <= position.y + size.y);
    return false;
}