#pragma once

#include "Mesh.h"
#include "transform.h"


class GameObject
{

public:
	GameObject();
	void transformPositions(glm::vec3 pos, glm::vec3 rot, glm::vec3 scale);
	void update(Mesh* mesh);
	void draw(Mesh *mesh);
	void setActive(bool set);
	
	Transform getTM() { return tObject; }
	Transform tObject;
	
	bool active;
	float counter = 0.0f;

	float deltaTime;
};


