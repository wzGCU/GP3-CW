#include "GameObject.h"

GameObject::GameObject()
{
	active = true;
}

void GameObject::draw(Mesh *mesh)
{
	mesh->draw();
}

void GameObject::setActive(bool set)
{
	active = set;
}

void GameObject::update(Mesh* mesh)
{
	mesh->updateSphereData(*tObject.GetPos(), 0.62f);
}

void GameObject::updateAsteroidSphere(Mesh* mesh)
{
	mesh->updateSphereData(*tObject.GetPos(), 1.0f);
}
void GameObject::transformPositions(glm::vec3 pos, glm::vec3 rot, glm::vec3 scale)
{
	tObject.SetPos(pos);
	tObject.SetRot(rot);
	tObject.SetScale(scale);
}


void GameObject::moveForward(float amt)
{
	*tObject.GetPos() += forward * amt;
}

void GameObject::yaw(float yaw)
{
	*tObject.GetRot() += glm::vec3(0, 0, yaw);

	calculateForward(yaw);
}

void GameObject::calculateForward(float angle)
{
	static const glm::vec3 UP(0.0f, 0.0f, 1.0f);

	glm::mat4 rotation = glm::rotate(angle, UP);

	forward = glm::vec3(glm::normalize(rotation * glm::vec4(forward, 0.0)));
	right = glm::vec3(glm::normalize(rotation * glm::vec4(right, 0.0)));
	up = glm::cross(forward, right);
}

void GameObject::setForward(glm::vec3 modelPos)
{
	glm::vec3 pos = *tObject.GetPos();
	forward = glm::vec3(glm::normalize(modelPos - pos));
	glm::lookAt(pos, pos + forward, up);
}