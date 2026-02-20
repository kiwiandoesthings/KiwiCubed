#include "AssetManager.h"
#include "UI.h"


UIImage::UIImage(glm::vec2 position, glm::vec2 size, std::string functionToTrigger, AssetStringID imageStringID, AssetStringID textureAtlasStringID) : UIElement(position, size, functionToTrigger), image(MetaTexture{imageStringID, *assetManager.GetTextureAtlasData(imageStringID)}), textureAtlasStringID(textureAtlasStringID) {
    UIImage::size = glm::vec2(1, 1) * glm::vec2(scale.x, scale.y);
    return;
}

void UIImage::Render(glm::vec2 position, glm::vec2 size, TextureAtlasData atlasData, Texture* atlas) {
    atlas->SetActive();
    atlas->Bind();
    int atlasSize = static_cast<int>(atlas->atlasSize.x);

    GLfloat vertices[] = {
        // Positions      // Texture Coordinates
        0.0f, 0.0f, static_cast<GLfloat>(atlasData.xPosition) / atlasSize, static_cast<GLfloat>(atlasData.yPosition) / atlasSize,
	    1.0f, 0.0f, static_cast<GLfloat>(atlasData.xPosition + atlasData.xSize) / atlasSize, static_cast<GLfloat>(atlasData.yPosition) / atlasSize,
	    1.0f, 1.0f, static_cast<GLfloat>(atlasData.xPosition + atlasData.xSize) / atlasSize, static_cast<GLfloat>(atlasData.yPosition + atlasData.ySize) / atlasSize,
	    0.0f, 1.0f, static_cast<GLfloat>(atlasData.xPosition) / atlasSize, static_cast<GLfloat>(atlasData.yPosition + atlasData.ySize) / atlasSize
    };

    GLuint indices[] = {
        0, 1, 2,
	    2, 3, 0,
    };

    UI& ui = UI::GetInstance();

    ui.uiShaderProgram.Bind();

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

    glm::mat4 modelMatrix = glm::mat4(1.0f);
    glm::vec2 ndcPosition = UIElement::PixelsToNDC(position);
    glm::vec2 ndcSize = PixelsToNDC(size) + glm::vec2(1, 1);
    modelMatrix = glm::translate(modelMatrix, glm::vec3(ndcPosition.x, ndcPosition.y, 0.0f));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(ndcSize.x, ndcSize.y, 1.0f));
    ui.uiShaderProgram.SetUniformMatrix4fv("modelMatrix", modelMatrix);
    
    GLCall(glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(sizeof(indices)), GL_UNSIGNED_INT, 0));
}

void UIImage::Render() {
    TextureAtlasData atlasData = image.atlasData[static_cast<int>(frame)];

    Texture* textureAtlas = assetManager.GetTextureAtlas(textureAtlasStringID);
    textureAtlas->SetActive();
    textureAtlas->Bind();
    int atlasSize = static_cast<int>(textureAtlas->atlasSize.x);
    
    GLfloat vertices[] = {
        // Positions      // Texture Coordinates
        0.0f, 0.0f, static_cast<GLfloat>(atlasData.xPosition) / atlasSize, static_cast<GLfloat>(atlasData.yPosition) / atlasSize,
	    1.0f, 0.0f, static_cast<GLfloat>(atlasData.xPosition + atlasData.xSize) / atlasSize, static_cast<GLfloat>(atlasData.yPosition) / atlasSize,
	    1.0f, 1.0f, static_cast<GLfloat>(atlasData.xPosition + atlasData.xSize) / atlasSize, static_cast<GLfloat>(atlasData.yPosition + atlasData.ySize) / atlasSize,
	    0.0f, 1.0f, static_cast<GLfloat>(atlasData.xPosition) / atlasSize, static_cast<GLfloat>(atlasData.yPosition + atlasData.ySize) / atlasSize
    };

    GLuint indices[] = {
        0, 1, 2,
	    2, 3, 0,
    };

    UI& ui = UI::GetInstance();

    ui.uiShaderProgram.Bind();
    
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
    
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    glm::vec2 ndcPosition = PixelsToNDC(position);
    glm::vec2 ndcSize = PixelsToNDC(size) + glm::vec2(1, 1);
    modelMatrix = glm::translate(modelMatrix, glm::vec3(ndcPosition.x, ndcPosition.y, 0.0f));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(ndcSize.x, ndcSize.y, 1.0f));
    ui.uiShaderProgram.SetUniformMatrix4fv("modelMatrix", modelMatrix);
    
    GLCall(glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(sizeof(indices)), GL_UNSIGNED_INT, 0));
}