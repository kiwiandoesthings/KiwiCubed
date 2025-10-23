#include "Entity.h"
#include "AssetManager.h"
#include "Block.h"
#include "ModHandler.h"
#include "World.h"

Entity::Entity(float entityX, float entityY, float entityZ, World* world) : entityStats(EntityStats()), entityData(EntityData()), world(world) {
	entityData.position.x = entityX;
	entityData.position.y = entityY;
	entityData.position.z = entityZ;

	protectedEntityData.UUID = CreateUUID().c_str();
}

void Entity::SetupRenderComponents(AssetStringID modelID, AssetStringID atlasID, AssetStringID textureID) {
	entityModel = *assetManager.GetEntityModel(modelID);
	entityTexture = assetManager.GetTextureAtlas(atlasID);
	entityTextureAtlasData = assetManager.GetTextureAtlasData(textureID);

	TextureAtlasData atlasData = (*entityTextureAtlasData)[0];
	int atlasSize = entityTexture->atlasSize.x;

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

	entityData.orientation.y += 150.0f * Globals::GetInstance().deltaTime;
	entityRenderData.positionOffset.y = sin(world->GetTotalTicks() / 3.0f) * 0.025;

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

std::string Entity::CreateUUID() {
	std::array<uint8_t, 16> uuidBytes;

	std::random_device random;
	std::mt19937 gen(random());
	std::uniform_int_distribution<unsigned int> dist(0, 255);

	for (auto &byte : uuidBytes) {
		byte = dist(gen);
	}
	
	uuidBytes[6] = (uuidBytes[6] & 0x0F) | 0x40;
	uuidBytes[8] = (uuidBytes[8] & 0x3F) | 0x80;

	std::ostringstream stringStream;
	stringStream << std::hex << std::setfill('0');
	for (size_t i = 0; i < uuidBytes.size(); i++) {
		stringStream << std::setw(2) << static_cast<unsigned int>(uuidBytes[i]);
		if (i == 3 || i == 5 || i == 7 || i == 9) {
			stringStream << "-";
		}
	}
	
	return stringStream.str();
}

void Entity::Delete() {

}