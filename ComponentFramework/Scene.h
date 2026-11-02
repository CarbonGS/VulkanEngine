#ifndef SCENE_H
#define SCENE_H
#include "Renderer.h"
#include "Audio.h"

union SDL_Event;

class Scene {
public:
	explicit Scene(Renderer* renderer_, Audio& audioSystem_){};
	virtual ~Scene() {}

	virtual bool OnCreate() = 0;
	virtual void OnDestroy() = 0;
	virtual void Update(const float deltaTime) = 0;
	virtual void Render() const  = 0 ;
	virtual void HandleEvents(const SDL_Event &sdlEvent) = 0;
protected:
	//Renderer *renderer;
	//Audio& audioSystem;
};
#endif