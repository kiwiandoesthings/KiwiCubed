#include "Entity.h"
#include "AssetManager.h"
#include "Block.h"
#include "ModHandler.h"
#include "World.h"

Entity::Entity(float entityX, float entityY, float entityZ, World* world) : entityStats(EntityStats()), entityData(EntityData()), world(world) {
	entityData.position.x = entityX;
	entityData.position.y = entityY;
	entityData.position.z = entityZ;

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

ProtectedEntityData Entity::GetProtectedEntityData() const {
	return protectedEntityData;
}

void Entity::SetEntityStats(EntityStats newEntityStats) {
	entityStats = newEntityStats;
}

void Entity::SetEntityData(EntityData newEntityData) {
	entityData = newEntityData;
}

void Entity::DamageEntity(float damage) {
	entityStats.health -= damage * (1 - static_cast<float>(static_cast<float>(entityStats.armor) / 100));
}

void Entity::Update() {
	entityRenderData.oldPosition = entityData.position;
	entityRenderData.oldOrientation = entityData.orientation;
	entityRenderData.oldUpDirection = entityData.upDirection;
	entityRenderData.oldVelocity = entityData.velocity;
	entityRenderData.oldPositionOffset = entityRenderData.positionOffset;
	entityRenderData.oldOrientationOffset = entityRenderData.orientationOffset;

	glm::ivec3 oldGlobalChunkPosition = entityData.globalChunkPosition;
	entityData.globalChunkPosition = glm::ivec3(
		Physics::FloorDiv(entityData.position.x, chunkSize),
		Physics::FloorDiv(entityData.position.y, chunkSize),
		Physics::FloorDiv(entityData.position.z, chunkSize)
	);
	entityData.localChunkPosition = glm::ivec3(
		Physics::PositiveModulo(entityData.position.x, chunkSize), 
		Physics::PositiveModulo(entityData.position.y, chunkSize), 
		Physics::PositiveModulo(entityData.position.z, chunkSize)
	);

	if (oldGlobalChunkPosition != entityData.globalChunkPosition) {
		entityData.currentChunkPtr = world->GetChunkHandler().GetChunk(entityData.globalChunkPosition.x, entityData.globalChunkPosition.y, entityData.globalChunkPosition.z, false);
	}

	if (entityData.isPlayer) {
		return;
	}

	entityData.orientation.y += 150.0f * Globals::GetInstance().deltaTime;
	entityRenderData.positionOffset.y = sin(world->GetTotalTicks() / 3.0f) * 0.025;

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

	glm::vec3 interpolatedPosition = entityRenderData.oldPosition + (entityData.position - entityRenderData.oldPosition) * world->GetPartialTicks();
	glm::vec3 interpolatedOrientation = entityRenderData.oldOrientation + (entityData.orientation - entityRenderData.oldOrientation) * world->GetPartialTicks();
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
	unsigned long long id = 0;

	std::random_device random;
	std::mt19937 gen(random());
	std::uniform_int_distribution<unsigned int> dist(0, 9);

	for (int iterator = 0; iterator < 64; iterator++) {
		id += static_cast<uint64_t>(dist(gen) * pow(10, iterator));
	}
	
	return id;
}

void Entity::Delete() {

}