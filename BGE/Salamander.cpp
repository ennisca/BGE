#include "Salamander.h"
#include "Utils.h"

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
	elapsed = 0.0f;
	walking = false;
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
		sectionDepth / 2,
		sectionWidth,
		sectionHeight
		);

	 CreateLegs(
		bodySections[bodySections.size() - 1],
		sectionDepth / 2,
		sectionWidth,
		sectionHeight
		); 

	return head;
}

shared_ptr<PhysicsController> Salamander::CreateBodySection(glm::vec3 position, shared_ptr<PhysicsController> sectionToConnect, float w, float h, float d)
{
	shared_ptr<PhysicsController> section = physicsFactory->CreateBox(w, h, d, position, glm::quat());
	bodySections.push_back(section);

	btHingeConstraint * hinge = new btHingeConstraint(*sectionToConnect->rigidBody, *section->rigidBody, btVector3(0, 0, 2.5f), btVector3(0, 0, -2.5f), btVector3(0, 1, 0), btVector3(0, 1, 0), true);
	// hinge->setLimit(btScalar(-0.20f), btScalar(0.20f));
	hinge->setLimit(btScalar(0), btScalar(0));

	physicsFactory->dynamicsWorld->addConstraint(hinge);

	return section;
}

void Salamander::CreateLegs(shared_ptr<PhysicsController> bodySection, float w, float h, float d)
{
	int rightLeftMupltiplier[] = { -1, 1 };

	float bodyWidth = d, bodyDepth = w;

	for each (int side in rightLeftMupltiplier)
	{
		float angle = 30.0f;
		glm::vec3 offset = glm::vec3(w, 0, 0) * float(side);

		// Create upper leg section
		glm::vec3 position = bodySection->transform->position + offset;
		shared_ptr<PhysicsController> upperLeg = physicsFactory->CreateBox(w, h, d, position, glm::angleAxis(angle, glm::vec3(0, 1, 0)));
		btHingeConstraint * hinge = new btHingeConstraint(
			*bodySection->rigidBody,
			*upperLeg->rigidBody,
			btVector3(side * bodyWidth / 2, 0, 0),
			btVector3(-side * w * 0.75f, 0, 0),
			btVector3(0, 1, 0),
			btVector3(0, 1, 0),
			true
			);
		physicsFactory->dynamicsWorld->addConstraint(hinge);

		// Create lower leg section
		/*offset = glm::vec3(d * 0.75f * side, -d, 0);
		position += offset;
		shared_ptr<PhysicsController> lowerLeg = physicsFactory->CreateBox(w, h, d, position, glm::angleAxis(angle, glm::vec3(1, 0, 0)));
		hinge = new btHingeConstraint(
			*upperLeg->rigidBody,
			*lowerLeg->rigidBody,
			btVector3(0, -h / 2, -d / 2),
			btVector3(0, 0, d * 0.625),
			btVector3(1, 0, 0),
			btVector3(1, 0, 0)
			);
		hinge->setLimit(glm::quarter_pi<float>(), glm::quarter_pi<float>());
		physicsFactory->dynamicsWorld->addConstraint(hinge); */
		legs.push_back(upperLeg);
	}
}

void Salamander::Update(float timeDelta)
{

	elapsed += timeDelta;

	GameComponent::Update(timeDelta);
}
