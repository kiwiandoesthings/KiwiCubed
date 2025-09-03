#include "UI.h"
#include "Events.h"
#include "TextRenderer.h"


UI::UI() : uiShaderProgram(nullptr) {}
UI::~UI() = default;

UI& UI::GetInstance() {
    static UI instance;
    return instance;
}

void UI::Setup(Shader* shaderProgram, Texture* atlas, TextRenderer* textRenderer) {
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
    uiTextRenderer = textRenderer;

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

    std::string realName = screenName.substr(3);

    EventManager::GetInstance().RegisterEvent("ui/move_screen_" + realName);
    EventManager::GetInstance().AddEventToDo("ui/move_screen_" + realName, [&](Event& event) {
        UI::GetInstance().SetCurrentScreen(this);
    });

    INFO("Created screen \"" + screenName + "\"");
}

void UIScreen::Render() {
    UI& ui = UI::GetInstance();
    for (int iterator = 0; iterator < uiElements.size(); ++iterator) {
        uiElements[iterator]->Render();
    }
    for (int iterator = 0; iterator < uiElements.size(); ++iterator) {
        UIElement* element = uiElements[iterator];
        std::string* elementLabel = element->GetElementLabel();
        if (*elementLabel != "") {
            glm::vec2* position = element->GetPosition();
            glm::vec2* size = element->GetSize();
            glm::vec2 textDimensions = ui.uiTextRenderer->MeasureText(*elementLabel, 2);
            ui.uiTextRenderer->RenderText(*elementLabel, (position->x + size->x / 2) - (textDimensions.x / 2), (position->y + size->y / 2) - 24, 2, glm::vec3(150, 150, 150));
        }
    }
}

void UIScreen::AddUIElement(UIElement* element) {
    uiElements.emplace_back(element);
}

glm::vec2 UIElement::PixelsToNDC(glm::vec2 pixelPosition) {
    return glm::vec2((pixelPosition.x / Window::GetInstance().GetWidth()) * 2 - 1, (pixelPosition.y / Window::GetInstance().GetHeight()) * 2 - 1);
}

UIElement::UIElement(glm::vec2 position, glm::vec2 scale, std::string eventToTrigger, std::string elementLabel) : position(position), scale(scale), eventToTrigger(eventToTrigger), elementLabel(elementLabel) {
    size = glm::vec2(512, 128) * glm::vec2(scale.x, scale.y);
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

std::string* UIElement::GetEventTrigger() {
    return &eventToTrigger;
}

std::string* UIElement::GetElementLabel() {
    return &elementLabel;
}

glm::vec2* UIElement::GetPosition() {
    return &position;
}

glm::ivec2* UIElement::GetScale() {
    return &scale;
}

glm::vec2* UIElement::GetSize() {
    return &size;
}