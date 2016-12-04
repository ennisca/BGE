#include "Salamander.h"
#include "Content.h"
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
	elapsed = -90.0f;
	walking = stepLeft = stepRight = false;
	walking = true;
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
		sectionHeight,
		1
		);

	 CreateLegs(
		bodySections[bodySections.size() - 1],
		sectionDepth / 2,
		sectionWidth,
		sectionHeight,
		-1
		); 

	return head;
}

shared_ptr<PhysicsController> Salamander::CreateBodySection(glm::vec3 position, shared_ptr<PhysicsController> sectionToConnect, float w, float h, float d)
{
	shared_ptr<PhysicsController> section = physicsFactory->CreateBox(w, h, d, position, glm::quat());
	bodySections.push_back(section);

	btHingeConstraint * hinge = new btHingeConstraint(*sectionToConnect->rigidBody, *section->rigidBody, btVector3(0, 0, 2.5f), btVector3(0, 0, -2.5f), btVector3(0, 1, 0), btVector3(0, 1, 0), true);
	hinge->setLimit(btScalar(-0.20f), btScalar(0.20f));
	// hinge->setLimit(btScalar(0), btScalar(0));

	physicsFactory->dynamicsWorld->addConstraint(hinge);

	return section;
}

void Salamander::CreateLegs(shared_ptr<PhysicsController> bodySection, float w, float h, float d, int angleMultiplier)
{
	int rightLeftMupltiplier[] = { -1, 1 };

	float bodyWidth = d, bodyDepth = w;

	for each (int side in rightLeftMupltiplier)
	{
		float angle = 30.0f * angleMultiplier;
		glm::vec3 offset = glm::vec3(w, 0, 0) * float(side);

		// Create upper leg section
		glm::vec3 position = bodySection->transform->position + offset;
		shared_ptr<PhysicsController> upperLeg = physicsFactory->CreateBox(w, h, d, position, glm::angleAxis(angle, glm::vec3(0, 1, 0)));
		btHingeConstraint * hinge = new btHingeConstraint(
			*bodySection->rigidBody,
			*upperLeg->rigidBody,
			btVector3(side * bodyWidth / 2, 0, 0),
			btVector3(-side * w * 0.625f, 0, 0),
			btVector3(0, 1, 0),
			btVector3(0, 1, 0),
			true
			);
		physicsFactory->dynamicsWorld->addConstraint(hinge);

		// Create lower leg section
		offset = glm::vec3(w * side, -w, 0);
		position += offset;
		shared_ptr<PhysicsController> lowerLeg = physicsFactory->CreateBox(h, w, d, position, glm::quat());
		hinge = new btHingeConstraint(
			*upperLeg->rigidBody,
			*lowerLeg->rigidBody,
			btVector3(side * w * 0.625f, 0, 0),
			btVector3(0, w * 0.625f, 0),
			btVector3(0, 0, 1),
			btVector3(0, 0, 1)
			);
		hinge->setLimit(side * -glm::quarter_pi<float>() / 2, side * -glm::half_pi<float>());

		if (side == -1)
		{
			hinge->setLimit(glm::quarter_pi<float>() / 2, glm::half_pi<float>());
		}
		else
		{
			hinge->setLimit(-glm::half_pi<float>(), -glm::quarter_pi<float>());
		}

		physicsFactory->dynamicsWorld->addConstraint(hinge);
		legs.push_back(upperLeg);
		lowerLegs.push_back(lowerLeg);
	}
}

void Salamander::Update(float timeDelta)
{
	const Uint8 * keyState = Game::Instance()->GetKeyState();

	speed = 4;

	if (keyState[SDL_SCANCODE_K])
	{
		walking = true;
	}

	if (keyState[SDL_SCANCODE_L])
	{
		walking = false;
	}

	if (walking) 
	{
		if (glm::sin(elapsed * 0.4) > 0)
		{
			cout << "greater" << endl;
			stepLeft = false;
			stepRight = true;
		}

		if (glm::sin(elapsed * 0.4) < 0)
		{
			cout << "less" << endl;
			stepLeft = true;
			stepRight = false;
		}
	}

	if (stepLeft)
	{
		legs[0]->rigidBody->activate();
		legs[0]->rigidBody->applyForce(btVector3(0, 0, speed), btVector3(-2.0f, 0, 0));
		lowerLegs[0]->rigidBody->activate();
		lowerLegs[0]->rigidBody->applyForce(btVector3(speed / 2, 0, 0), btVector3(0, -2.0f, 0));

		legs[1]->rigidBody->activate();
		legs[1]->rigidBody->applyForce(btVector3(0, 0, -speed), btVector3(2.0f, 0, 0));
		lowerLegs[1]->rigidBody->activate();
		lowerLegs[1]->rigidBody->applyForce(btVector3(speed / 2, 0, 0), btVector3(0, -2.0f, 0));

		legs[2]->rigidBody->activate();
		legs[2]->rigidBody->applyForce(btVector3(0, 0, -speed), btVector3(-2.0f, 0, 0));
		lowerLegs[2]->rigidBody->activate();
		lowerLegs[2]->rigidBody->applyForce(btVector3(-speed / 2, 0, 0), btVector3(0, -2.0f, 0));

		legs[3]->rigidBody->activate();
		legs[3]->rigidBody->applyForce(btVector3(0, 0, speed), btVector3(2.0f, 0, 0));
		lowerLegs[3]->rigidBody->activate();
		lowerLegs[3]->rigidBody->applyForce(btVector3(-speed / 2, 0, 0), btVector3(0, -2.0f, 0));
	}

	if (stepRight)
	{
		legs[0]->rigidBody->activate();
		legs[0]->rigidBody->applyForce(btVector3(0, 0, -speed), btVector3(-2.0f, 0, 0));
		lowerLegs[0]->rigidBody->activate();
		lowerLegs[0]->rigidBody->applyForce(btVector3(-speed / 2, 0, 0), btVector3(0, -2.0f, 0));

		legs[1]->rigidBody->activate();
		legs[1]->rigidBody->applyForce(btVector3(0, 0, speed), btVector3(2.0f, 0, 0));
		lowerLegs[1]->rigidBody->activate();
		lowerLegs[1]->rigidBody->applyForce(btVector3(-speed / 2, 0, 0), btVector3(0, -2.0f, 0));

		legs[2]->rigidBody->activate();
		legs[2]->rigidBody->applyForce(btVector3(0, 0, speed), btVector3(-2.0f, 0, 0));
		lowerLegs[2]->rigidBody->activate();
		lowerLegs[2]->rigidBody->applyForce(btVector3(speed / 2, 0, 0), btVector3(0, -2.0f, 0));

		legs[3]->rigidBody->activate();
		legs[3]->rigidBody->applyForce(btVector3(0, 0, -speed), btVector3(2.0f, 0, 0));
		lowerLegs[3]->rigidBody->activate();
		lowerLegs[3]->rigidBody->applyForce(btVector3(speed / 2, 0, 0), btVector3(0, -2.0f, 0));
	}

	elapsed += timeDelta;

	GameComponent::Update(timeDelta);
}
