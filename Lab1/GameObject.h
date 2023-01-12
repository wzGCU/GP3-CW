#pragma once

#include "Mesh.h"
#include "transform.h"
#include "Shader.h"


class GameObject
{

public:
	GameObject();
	void transformPositions(glm::vec3 pos, glm::vec3 rot, glm::vec3 scale);
	void update(Mesh* mesh);
	void draw(Mesh* mesh);
	void setActive(bool set);
	void updateAsteroidSphere(Mesh* mesh);

	bool getActive() { return active; }
	glm::mat4 getModel() { return tObject.GetModel(); }
	Transform getTM() { return tObject; }

	float getSphereRadiusinGameObject(Mesh* mesh) {
		float spherePosition = mesh->getSphereRadius();
		return spherePosition;
	}
	glm::vec3* getPos() { return &pos; }
	glm::vec3* getPosition() { return tObject.GetPos(); }

	void moveForward(float amt);
	void yaw(float yaw);
	void GameObject::setForward(glm::vec3 modelPos);

	glm::vec3 fwd() { return forward; }
	glm::vec3 rght() { return right; }
	glm::vec3 uP() { return up; }

	float rad2deg = 57.2958;
	float deg2rad = 1 / 57.2958;

private:
	Transform tObject;
	Camera cameraIn;
	Shader shaderIn;

	void calculateForward(float angle);

	glm::vec3 forward = glm::vec3(1.0, 0.0, 0.0);
	glm::vec3 up = glm::vec3(0.0, 1.0, 0.0);
	glm::vec3 right = glm::vec3(0.0, 0.0, 1.0);
	glm::vec3 rot = this->rot;
	
	glm::vec3 pos;
	bool active;
	float counter = 0.0f;

	float deltaTime;
};


