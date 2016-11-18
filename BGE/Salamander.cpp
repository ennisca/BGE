#include "Salamander.h"
using namespace BGE;


Salamander::Salamander(shared_ptr<PhysicsFactory> _physicsFactory)
{
	physicsFactory = _physicsFactory;
}

Salamander::Salamander()
{

}

Salamander::~Salamander(void)
{

}

bool Salamander::Initialise()
{
	return true;
}

shared_ptr<PhysicsController> Salamander::CreateSalamander(glm::vec3 position, int numSections, float sectionWidth, float sectionHeight, float sectionDepth)
{
	head = physicsFactory->CreateBox(sectionWidth, sectionHeight, sectionDepth, position, glm::quat());

	glm::vec3 gap = glm::vec3(0, 0, sectionDepth + sectionDepth / 4);

	shared_ptr<PhysicsController> sectionToConnect = head;
	for (int i = 0; i < numSections; i++)
	{
		position += gap;
		sectionToConnect = CreateBodySection(position, sectionToConnect, sectionWidth, sectionHeight, sectionDepth);
		bodySections.push_back(sectionToConnect);
	}

	CreateLegs(
		bodySections[0],
		sectionWidth,
		sectionHeight,
		sectionDepth / 2
		);

	/*CreateLegs(
		bodySections[bodySections.size() - 1],
		sectionWidth,
		sectionHeight,
		sectionDepth
		);*/

	return head;
}

shared_ptr<PhysicsController> Salamander::CreateBodySection(glm::vec3 position, shared_ptr<PhysicsController> sectionToConnect, float w, float h, float d)
{
	shared_ptr<PhysicsController> section = physicsFactory->CreateBox(w, h, d, position, glm::quat());
	bodySections.push_back(section);

	btHingeConstraint * hinge = new btHingeConstraint(*sectionToConnect->rigidBody, *section->rigidBody, btVector3(0, 0, 2.5f), btVector3(0, 0, -2.5f), btVector3(0, 1, 0), btVector3(0, 1, 0), true);
	hinge->setLimit(btScalar(-0.20f), btScalar(0.20f));

	physicsFactory->dynamicsWorld->addConstraint(hinge);

	return section;
}

void Salamander::CreateLegs(shared_ptr<PhysicsController> bodySection, float w, float h, float d)
{
	int rightLeftMupltiplier[] = { -1 };

	for each (int side in rightLeftMupltiplier)
	{
		float angle = -90.0f * side;
		glm::vec3 offset = glm::vec3(d / 2, 0, 0) * float(side);

		// Create upper leg section
		glm::vec3 position = bodySection->transform->position + offset;
		shared_ptr<PhysicsController> upperLeg = physicsFactory->CreateBox(w, h, d, position, glm::angleAxis(angle, glm::vec3(0, 1, 0)));
		btHingeConstraint * hinge = new btHingeConstraint(
			*bodySection->rigidBody,
			*upperLeg->rigidBody,
			btVector3(w * side * 0.5f, 0, 0),
			btVector3(0, 0, d * 0.625),
			btVector3(0, 1, 0),
			btVector3(0, 1, 0),
			true
		);
		physicsFactory->dynamicsWorld->addConstraint(hinge);

		// Create lower leg section
		offset = glm::vec3(d * 0.75f * side, -d, 0);
		position += offset;
		shared_ptr<PhysicsController> lowerLeg = physicsFactory->CreateBox(w, h, d, position, glm::angleAxis(angle, glm::vec3(1, 0, 0)));
	}
}

void Salamander::Update(float timeDelta)
{
	GameComponent::Update(timeDelta);
}
