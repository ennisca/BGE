#include "PhysicsGame1.h"
#include "PhysicsController.h"
#include "Sphere.h"
#include "PhysicsCamera.h"
#include "Box.h"
#include "Cylinder.h"
#include "Steerable3DController.h"
#include "Ground.h"
#include "Content.h"
#include <btBulletDynamicsCommon.h>
#include <gtc/quaternion.hpp>
#include <gtx/quaternion.hpp>
#include <gtx/euler_angles.hpp>
#include <gtx/norm.hpp>
#include "VectorDrawer.h"
#include "Utils.h"
#include "Salamander.h"

using namespace BGE;

PhysicsGame1::PhysicsGame1(void)
{
}

PhysicsGame1::~PhysicsGame1(void)
{
}

shared_ptr<PhysicsController> cyl;
std::shared_ptr<GameComponent> station;

bool PhysicsGame1::Initialise() 
{	
	shared_ptr<PhysicsController> ground = physicsFactory->CreateGroundPhysics();
	ground->rigidBody->setFriction(0);
	// physicsFactory->CreateGroundPhysics();
	physicsFactory->CreateCameraPhysics();	

	// dynamicsWorld->setGravity(btVector3(0, -9.8, 0));

	salamander = make_shared<Salamander>(physicsFactory);
	salamander->Initialise();
	Attach(salamander);

	float w = 1;
	float h = 1;
	float d = 4;
	glm::vec3 position = glm::vec3(5, 5, -15);
	int numSections = 4;

	salamander->CreateSalamander(position, numSections, w, h, d);

	if (!Game::Initialise()) {
		return false;
	}

	camera->transform->position = glm::vec3(5, 20, 0);
	camera->transform->orientation = glm::angleAxis(-45.0f, glm::vec3(1, 0, 0));
	
	return true;
}

void BGE::PhysicsGame1::Update(float timeDelta)
{
	Game::Update(timeDelta);
}

void BGE::PhysicsGame1::Cleanup()
{
	Game::Cleanup();
}
