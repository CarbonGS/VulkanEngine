#ifndef SCENE0_H
#define SCENE0_H
#include "Scene.h"
#include "Vector.h"
#include "Renderer.h"
#include "Camera.h"
#include "CoreStructs.h"
#include "Actor.h"
#include "ObjectPool.h"

using namespace MATH;

/// Forward declarations 
union SDL_Event;

class Scene0 : public Scene {
private:
	Audio& audioSystem;
	Renderer *renderer;

	Camera *camera;

	ObjectPool<Actor> actorPool;

	std::unordered_map<std::string, Actor*> actors;

	Actor* brz;
	Actor* mario;

	std::vector<BufferMemory> lightsUBO;
	LightsData lights;

	Vec4 redLight = Vec4(1.5f, 0.0f, 0.0f, 0.0f);
	Vec4 greenLight = Vec4(0.0f, 1.5f, 0.0f, 0.0f);
	Vec4 blueLight = Vec4(0.0f, 0.0f, 1.5f, 0.0f);

	CommandBufferData commandBufferData;

public:

	explicit Scene0(Renderer* renderer_, Audio& audioSystem_);
	virtual ~Scene0();

	virtual bool OnCreate() override;
	virtual void OnDestroy() override;
	virtual void Update(const float deltaTime) override;
	virtual void Render() const override;
	virtual void HandleEvents(const SDL_Event &sdlEvent) override;
};


#endif // SCENE0_H