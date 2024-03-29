#pragma once

#include "Mesh.h"
#include "transform.h"
#include "Shader.h"


class GameObject
{

public:
	GameObject();
	void moveTo(glm::vec3 pos);
	void setRotation(glm::vec3 rot);
	void setScale(glm::vec3 scale);
	void update(Mesh* mesh);
	void draw(Mesh* mesh);
	void setActive(bool set);
	bool getActive() { return active; }

	glm::mat4 getModel() { return tObject.GetModel(); }
	Transform getTM() { return tObject; }

	float getSphereRadiusinGameObject(Mesh* mesh) {
		float sphereRadius = mesh->getSphereRadius();
		return sphereRadius;
	}
	//glm::vec3* getPos() { return &pos; }
	glm::vec3* getTPosition() { return tObject.GetPos(); }

private:
	Transform tObject;
	Camera cameraIn;
	Shader shaderIn;
	
	glm::vec3 pos;
	bool active;

};


