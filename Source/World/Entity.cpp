#include "Entity.h"
#include "AssetManager.h"
#include "Block.h"
#include "ModHandler.h"
#include "World.h"

Entity::Entity(float entityX, float entityY, float entityZ, World* world) : entityStats(EntityStats()), entityData(EntityData()), entityTransform(EntityTransform()), world(world) {
	entityTransform.position.x = entityX;
	entityTransform.position.y = entityY;
	entityTransform.position.z = entityZ;

    entityRenderData.oldPosition = entityTransform.position;
    entityRenderData.oldOrientation = entityTransform.orientation;
	entityRenderData.oldUpDirection = entityTransform.upDirection;
	entityRenderData.oldVelocity = entityTransform.velocity;
    entityRenderData.oldPositionOffset = entityRenderData.positionOffset;
    entityRenderData.oldOrientationOffset = entityRenderData.orientationOffset;

	protectedEntityData.AUID = CreateAUID();
}

void Entity::SetupRenderComponents(AssetStringID modelID, AssetStringID atlasID, AssetStringID textureID) {
	entityModel = *assetManager.GetEntityModel(modelID);
	entityTexture = assetManager.GetTextureAtlas(atlasID);
	entityTextureAtlasData = assetManager.GetTextureAtlasData(textureID);

	TextureAtlasData atlasData = (*entityTextureAtlasData)[0];
	int atlasSize = static_cast<int>(entityTexture->atlasSize.x);

	GLfloat textureCoordinates[] = {
    	static_cast<GLfloat>(atlasData.xPosition) / atlasSize, static_cast<GLfloat>(atlasData.yPosition) / atlasSize,
	    static_cast<GLfloat>(atlasData.xPosition + atlasData.xSize) / atlasSize, static_cast<GLfloat>(atlasData.yPosition) / atlasSize,
	   	static_cast<GLfloat>(atlasData.xPosition + atlasData.xSize) / atlasSize, static_cast<GLfloat>(atlasData.yPosition + atlasData.ySize) / atlasSize,
	    static_cast<GLfloat>(atlasData.xPosition) / atlasSize, static_cast<GLfloat>(atlasData.yPosition + atlasData.ySize) / atlasSize
    };

	std::vector<GLfloat> newVertices;
	newVertices.reserve((entityModel.vertices.size() / 3) * 5);
	for (int vertex = 0, latestTextureCoordinate = 0; vertex < entityModel.vertices.size(); vertex += 3, latestTextureCoordinate += 2) {
		newVertices.push_back(entityModel.vertices[vertex]);
		newVertices.push_back(entityModel.vertices[vertex + 1]);
		newVertices.push_back(entityModel.vertices[vertex + 2]);
		newVertices.push_back(textureCoordinates[latestTextureCoordinate]);
		newVertices.push_back(textureCoordinates[latestTextureCoordinate + 1]);
	}

	entityModel.vertices = std::move(newVertices);

	vertexBufferObject.SetupBuffer();
	vertexArrayObject.SetupArrayObject();
	indexBufferObject.SetupBuffer();
}

EntityStats Entity::GetEntityStats() const {
	return entityStats;
}

EntityData Entity::GetEntityData() const {
	return entityData;
}

EntityTransform Entity::GetEntityTransform() const {
	return entityTransform;
}

ProtectedEntityData Entity::GetProtectedEntityData() const {
	return protectedEntityData;
}

void Entity::SetEntityStats(EntityStats newEntityStats) {
	entityStats = newEntityStats;
}

void Entity::SetEntityData(EntityData newEntityData) {
	entityData = newEntityData;
}

void Entity::SetEntityTransform(EntityTransform newEntityTransform) {
	entityTransform = newEntityTransform;
}

void Entity::DamageEntity(float damage) {
	entityStats.health -= damage * (1 - static_cast<float>(static_cast<float>(entityStats.armor) / 100));
}

void Entity::Update() {
	entityRenderData.oldPosition = entityTransform.position;
	entityRenderData.oldOrientation = entityTransform.orientation;
	entityRenderData.oldUpDirection = entityTransform.upDirection;
	entityRenderData.oldVelocity = entityTransform.velocity;
	entityRenderData.oldPositionOffset = entityRenderData.positionOffset;
	entityRenderData.oldOrientationOffset = entityRenderData.orientationOffset;

	glm::ivec3 oldGlobalChunkPosition = entityTransform.globalChunkPosition;
	entityTransform.globalChunkPosition = glm::ivec3(
		Physics::FloorDiv(entityTransform.position.x, chunkSize),
		Physics::FloorDiv(entityTransform.position.y, chunkSize),
		Physics::FloorDiv(entityTransform.position.z, chunkSize)
	);
	entityTransform.localChunkPosition = glm::ivec3(
		Physics::PositiveModulo(entityTransform.position.x, chunkSize), 
		Physics::PositiveModulo(entityTransform.position.y, chunkSize), 
		Physics::PositiveModulo(entityTransform.position.z, chunkSize)
	);

	if (oldGlobalChunkPosition != entityTransform.globalChunkPosition) {
		entityData.currentChunkPtr = world->GetChunkHandler().GetChunk(entityTransform.globalChunkPosition.x, entityTransform.globalChunkPosition.y, entityTransform.globalChunkPosition.z, false);
	}

	return;
}

void Entity::Render() {
	Shader* shader = assetManager.GetShaderProgram(AssetStringID{"kiwicubed", "entity_shader"});

	shader->Bind();

	entityTexture->SetActive();
	entityTexture->Bind();
    
    vertexArrayObject.Bind();
    vertexBufferObject.Bind();
    vertexBufferObject.SetBufferData(sizeof(GLfloat) * entityModel.vertices.size(), entityModel.vertices.data());
    vertexArrayObject.LinkAttribute(vertexBufferObject, 0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 5, (void*)0);
	vertexArrayObject.LinkAttribute(vertexBufferObject, 1, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 5, (void*)(3 * sizeof(GLfloat)));
    indexBufferObject.Bind();
    indexBufferObject.SetBufferData(sizeof(GLuint) * entityModel.indices.size(), entityModel.indices.data());
    
    vertexArrayObject.Bind();
    vertexBufferObject.Bind();
    indexBufferObject.Bind();
    
    glm::mat4 modelMatrix = glm::mat4(1.0f);

	glm::vec3 interpolatedPosition = entityRenderData.oldPosition + (entityTransform.position - entityRenderData.oldPosition) * world->GetPartialTicks();
	glm::vec3 interpolatedOrientation = entityRenderData.oldOrientation + (entityTransform.orientation - entityRenderData.oldOrientation) * world->GetPartialTicks();
	glm::vec3 interpolatedPositionOffset = entityRenderData.oldPositionOffset + (entityRenderData.positionOffset - entityRenderData.oldPositionOffset) * world->GetPartialTicks();
	glm::vec3 interpolatedOrientationOffset = entityRenderData.oldOrientationOffset + (entityRenderData.orientationOffset - entityRenderData.oldOrientationOffset) * world->GetPartialTicks();
	
    modelMatrix = glm::translate(modelMatrix, interpolatedPosition);
	modelMatrix = glm::translate(modelMatrix, interpolatedPositionOffset);
	modelMatrix *= glm::mat4_cast(glm::quat(glm::radians(interpolatedOrientation)));
	modelMatrix *= glm::mat4_cast(glm::quat(glm::radians(interpolatedOrientationOffset)));
    shader->SetUniformMatrix4fv("modelMatrix", modelMatrix);
    
    GLCall(glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(entityModel.indices.size()), GL_UNSIGNED_INT, 0));
}

unsigned long long Entity::CreateAUID() {
	std::random_device random;
	std::mt19937 generator(random());
	std::uniform_int_distribution<unsigned long long> distribution;
	
	return distribution(generator);
}

void Entity::Delete() {

}