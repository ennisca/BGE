#pragma once
#include "GameComponent.h"
#include "PhysicsFactory.h"
#include "PhysicsController.h"

namespace BGE
{
	class Salamander :
		public GameComponent
	{
	private:
		shared_ptr<PhysicsController> CreateBodySection(glm::vec3 position, shared_ptr<PhysicsController> sectionToConnect, float w, float h, float d);
		void CreateLegs(shared_ptr<PhysicsController> bodySection, float w, float h, float d);
		shared_ptr<PhysicsFactory> physicsFactory;

		shared_ptr<PhysicsController> head, tail;
		vector<shared_ptr<PhysicsController>> bodySections, legs;

		float elapsed;
		bool control;
	public:
		Salamander(shared_ptr<PhysicsFactory> _physicsFactory);
		Salamander();
		~Salamander(void);
		bool Initialise();
		void Update(float timeDelta);
		shared_ptr<PhysicsController> CreateSalamander(glm::vec3 position, int numSections, float sectionWidth, float sectionHeight, float sectionDepth);
	};
}
