#include "UI.h"
#include "TextRenderer.h"

UIButton::UIButton(glm::vec2 position, glm::vec2 scale, std::string eventToTrigger, std::string elementLabel) : UIElement(position, scale, eventToTrigger), elementLabel(elementLabel) {
    UIButton::size = glm::vec2(512, 128) * glm::vec2(scale.x, scale.y);
}

void UIButton::Render() {
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

    ui.uiAtlas->SetActive();
    ui.uiAtlas->Bind();

    ui.uiShaderProgram->Bind();

    ui.vertexArrayObject.Bind();
    ui.vertexBufferObject.Bind();
    ui.vertexBufferObject.SetBufferData(sizeof(vertices), vertices);
    ui.vertexArrayObject.LinkAttribute(ui.vertexBufferObject, 0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (void*)0);
    ui.vertexArrayObject.LinkAttribute(ui.vertexBufferObject, 1, 2, GL_FLOAT, false, sizeof(float) * 4, (void*)(sizeof(float) * 2));
    ui.indexBufferObject.Bind();
    ui.indexBufferObject.SetBufferData(sizeof(indices), indices);
        
    ui.vertexArrayObject.Bind();
    ui.vertexBufferObject.Bind();
    ui.indexBufferObject.Bind();

    if ((GetHovered())) {
        if (UI::GetInstance().GetInputHandler().GetMouseButtonState(GLFW_MOUSE_BUTTON_LEFT)) {
            textureIndex = 2;
        } else {
            textureIndex = 1;
        }
    } else if (tabSelected) {
        textureIndex = 1;
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
    ui.uiShaderProgram->SetUniform1i("caluclateTextureCoordinates", GL_TRUE);

    GLCall(glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(sizeof(indices)), GL_UNSIGNED_INT, 0));

    if (elementLabel != "") {
        UI& ui = UI::GetInstance();
        glm::vec2 textDimensions = ui.uiTextRenderer->MeasureText(elementLabel, 2);
        ui.uiTextRenderer->RenderText(elementLabel, (position.x + size.x / 2) - (textDimensions.x / 2), (position.y + size.y / 2) - 24, 2, glm::vec3(150, 150, 150));
    }
}

bool UIButton::OnClick() {
    if (!GetHovered() || eventToTrigger == "") {
        return false;
    }

    EventManager::GetInstance().TriggerEvent(eventToTrigger);
    return true;
}

void UIButton::OnHover() {
    hoverSelected = true;
}

void UIButton::SetSelected(bool selected) {
    tabSelected = selected;
}