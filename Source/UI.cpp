#include "UI.h"


UI::UI() : uiShaderProgram(nullptr) {}
UI::~UI() = default;

UI& UI::GetInstance() {
    static UI instance;
    return instance;
}

void UI::Setup(Shader* shaderProgram, Texture* atlas, Window* window) {
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

    //vertexArrayObject.Bind();
    //vertexBufferObject.Bind();
    //vertexBufferObject.SetBufferData(sizeof(vertices), vertices);
    //vertexArrayObject.LinkAttribute(vertexBufferObject, 0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (void*)0);
    //vertexArrayObject.LinkAttribute(vertexBufferObject, 1, 2, GL_FLOAT, false, sizeof(float) * 4, (void*)(sizeof(float) * 2));
    //indexBufferObject.Bind();
    //indexBufferObject.SetBufferData(sizeof(indices), indices);

    uiShaderProgram = shaderProgram;
    uiAtlas = atlas;
    UI::window = window;
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
    return glm::vec2((pixelPosition.x / UI::GetInstance().window->GetWidth()) * 2 - 1, (pixelPosition.y / UI::GetInstance().window->GetHeight()) * 2 - 1);
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

    glm::mat4 modelMatrix = glm::mat4(1.0f);
    glm::vec2 ndcPosition = PixelsToNDC(position);
    glm::vec2 ndcSize = PixelsToNDC(size) + glm::vec2(1, 1);
    modelMatrix = glm::translate(modelMatrix, glm::vec3(ndcPosition.x, ndcPosition.y, 0.0f));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(ndcSize.x, ndcSize.y, 1.0f));
    ui.uiShaderProgram->SetUniformMatrix4fv("modelMatrix", modelMatrix);

    GLCall(glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(sizeof(indices)), GL_UNSIGNED_INT, 0));
}

bool UIElement::GetHovered() {
    glm::vec2 mousePosition = InputHandler::GetInstance().GetMousePosition();

    return (mousePosition.x >= position.x && mousePosition.y >= position.y && mousePosition.x <= position.x + size.x && mousePosition.y <= position.y + size.y);
}