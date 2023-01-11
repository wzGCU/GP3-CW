#include "GameObject.h"

GameObject::GameObject()
{
	active = true;
}

void GameObject::draw(Mesh *mesh)
{
	tObject.SetRot(glm::vec3(0.0, 0.0, 0.0));
	tObject.SetScale(glm::vec3(0.1, 0.1, 0.1));
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

void GameObject::transformPositions(glm::vec3 pos, glm::vec3 rot, glm::vec3 scale)
{
	tObject.SetPos(pos);
	tObject.SetRot(rot);
	tObject.SetScale(scale);
}