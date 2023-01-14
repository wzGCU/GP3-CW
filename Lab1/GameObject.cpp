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

void GameObject::moveTo(glm::vec3 pos)
{
	tObject.SetPos(pos);
}

void GameObject::setRotation(glm::vec3 rot) 
{
	tObject.SetRot(rot);
}

void GameObject::setScale(glm::vec3 scale)
{
	tObject.SetScale(scale);
}

