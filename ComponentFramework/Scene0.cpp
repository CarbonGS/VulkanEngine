#include <glew.h>
#include <iostream>
#include "Debug.h"
#include "Scene0.h"
#include <MMath.h>
#include "VulkanRenderer.h"
#include "OpenGLRenderer.h"
#include "Profiler.h"



Scene0::Scene0(Renderer *renderer_, Audio& audioSystem_):
	Scene(renderer_, audioSystem_),renderer(renderer_), 
	camera(nullptr), actorPool(5), audioSystem(audioSystem_)
{
	Debug::Info("Created Scene0: ", __FILE__, __LINE__);
}

Scene0::~Scene0() {
}

bool Scene0::OnCreate() {

	int width = 0, height = 0;
	float aspectRatio;

	switch (renderer->getRendererType()){
	case RendererType::VULKAN:
	{
		
		VulkanRenderer* vRenderer;
		vRenderer = dynamic_cast<VulkanRenderer*>(renderer);

		// Camera creation
		camera = new Camera(vRenderer);

		SDL_GetWindowSize(vRenderer->getWindow(), &width, &height);
		aspectRatio = static_cast<float>(width) / static_cast<float>(height);
		DualQuat initialOrientation(Vec3(0.0f, 0.0f, -5.0f));
		camera->OnCreate(aspectRatio, initialOrientation);

		lightsUBO = vRenderer->CreateUniformBuffers<LightsData>();

		// Lights setup
		lights.numLights = 3;
		lights.ambient = Vec4(0.1f, 0.1f, 0.1f, 0.0f);

		// R
		lights.diffuse[0] = redLight;
		lights.specular[0] = Vec4(0.0f, 0.5f, 0.0f, 0.0f);
		lights.pos[0] = Vec4(-12.0f, 0.0f, -5.0f, 0.0f);

		// G
		lights.diffuse[1] = greenLight;
		lights.specular[1] = Vec4(0.5f, 0.0f, 0.0f, 0.0f);
		lights.pos[1] = Vec4(0.0f, 5.0f, 0.0f, 0.0f);

		// B
		lights.diffuse[2] = blueLight;
		lights.specular[2] = Vec4(0.0f, 0.0f, 0.6f, 0.0f);
		lights.pos[2] = Vec4(15.0f, 0.0f, 0.0f, 0.0f);

		vRenderer->UpdateUniformBuffer<LightsData>(lights, lightsUBO);

		Quaternion orient = QMath::angleAxisRotation(90.0f, Vec3(0.0f, 1.0f, 0.0f));

		// Mario actor creation
		mario = actorPool.Acquire();

		mario->createTexture(vRenderer->Create2DTextureImage("./textures/mario_fire.png"));
		mario->createMesh(vRenderer->LoadModelIndexed("./meshes/Mario.obj"));

		DescriptorSetBuilder marioDescriptorSetBuilder(vRenderer->getDevice());
		marioDescriptorSetBuilder.add(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT, 1, camera->getCameraUBO());
		marioDescriptorSetBuilder.add(1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
		VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 1, lightsUBO);
		marioDescriptorSetBuilder.add(2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 1, &mario->GetSampler());
		DescriptorSetInfo marioDescSetInfo = marioDescriptorSetBuilder.BuildDescriptorSet(vRenderer->getNumSwapchains());
		PipelineInfo marioPipelineInfo = vRenderer->CreateGraphicsPipeline(marioDescSetInfo.descriptorSetLayout, "shaders/multiPhong.vert.spv", "shaders/multiPhong.frag.spv");
		
		mario->OnCreate(Vec3(0, 0, 0), Vec3(0, 0, 0), orient, marioDescSetInfo, marioPipelineInfo);

		actors["mario"] = mario;

		// BRZ actor creation
		brz = actorPool.Acquire();

		brz->createTexture(vRenderer->Create2DTextureImage("./textures/brz.png"));
		brz->createMesh(vRenderer->LoadModelIndexed("./meshes/brz.obj"));

		DescriptorSetBuilder brzDescriptorSetBuilder(vRenderer->getDevice());
		brzDescriptorSetBuilder.add(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT, 1, camera->getCameraUBO());
		brzDescriptorSetBuilder.add(1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
		VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 1, lightsUBO);
		brzDescriptorSetBuilder.add(2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 1, &brz->GetSampler());
		DescriptorSetInfo brzDescSetInfo = brzDescriptorSetBuilder.BuildDescriptorSet(vRenderer->getNumSwapchains());
		PipelineInfo brzPipelineInfo = vRenderer->CreateGraphicsPipeline(brzDescSetInfo.descriptorSetLayout, "shaders/multiPhong.vert.spv", "shaders/multiPhong.frag.spv");

		brz->OnCreate(Vec3(0, 0, 0), Vec3(0, 0, 0), orient, brzDescSetInfo, brzPipelineInfo);

		actors["brz"] = brz;	
	
	}
		break;

	case RendererType::OPENGL:
		break;
	}

	return true;
}

void Scene0::HandleEvents(const SDL_Event& sdlEvent) {
	
	float aspectRatio = static_cast<float>(sdlEvent.window.data1) / static_cast<float>(sdlEvent.window.data2);
	static int colorState = 0;

		switch (sdlEvent.type) {
		case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED:
			printf("size changed %d %d\n", sdlEvent.window.data1, sdlEvent.window.data2);
			if(renderer->getRendererType() == RendererType::VULKAN){
				camera->setPerspective(Vec4(45.0f, aspectRatio, 0.5f, 100.0f));
				dynamic_cast<VulkanRenderer*>(renderer)->RecreateSwapChain();
			}
			break;
		case SDL_EVENT_KEY_UP:

			if (sdlEvent.key.key == SDLK_1)
			{
				// Set all lights to off
				for (uint32_t i = 0; i < lights.numLights; ++i)
				{
					lights.diffuse[i] *= 0;
					lights.specular[i] *= 0;
				}

				// Loop through all 3 states
				colorState = (colorState + 1) % 4;

				switch (colorState)
				{
				case 1:
					lights.diffuse[0] = redLight;
					break;
				case 2:
					lights.diffuse[1] = greenLight;
					break;
				case 3:
					lights.diffuse[2] = blueLight;
					break;
				case 0:
					break;
				}

			}
			dynamic_cast<VulkanRenderer*>(renderer)->UpdateUniformBuffer<LightsData>(lights, lightsUBO); // updates buffer to apply changes
			break;
		default:
			if (renderer->getRendererType() == RendererType::VULKAN) {
				dynamic_cast<VulkanRenderer*>(renderer)->RecreateSwapChain();
			}
			break;
		}	
}

void Scene0::Update(const float deltaTime) {
	static float elapsedTime = 0.0f;
	elapsedTime += deltaTime;
	//luigisModelMatrix = MMath::translate(2.0f, 0.0f, 0.0f) * MMath::rotate(elapsedTime * 90.0f, Vec3(1.0f, 0.0f, 0.0f));

	// Update all actors
	for (auto actor : actors)
	{
		actor.second->Update(deltaTime);
	}

	/*int profilerTest = 0;
	for (int i = 0; i < 10000000; ++i)
	{
		profilerTest = i;
	}*/

	Matrix4 marioMatrix = MMath::translate(0.0f, -1.0f, 0.0f) * MMath::rotate(elapsedTime * 90.0f, Vec3(0.0f, 1.0f, 0.0f)) * MMath::scale(0.5f, 0.5f, 0.5f);
	mario->setModelMatrix(marioMatrix);
}

void Scene0::Render() const {
		switch (renderer->getRendererType()) {

		case RendererType::VULKAN:
			VulkanRenderer* vRenderer;
			vRenderer = dynamic_cast<VulkanRenderer*>(renderer);
		
			vRenderer->RecordCommandBuffers(Recording::START);

			// Render all actors
			for (auto actor : actors)
			{
				actor.second->Render(vRenderer);
			}
		

			ImGui::BeginChild("Scene Actor Count", ImVec2(300,0), ImGuiChildFlags_Borders | ImGuiChildFlags_AutoResizeX | ImGuiChildFlags_AutoResizeY);
			ImGui::Text("Total Actors Loaded: %d", actors.size());
			ImGui::SetWindowFontScale(1.5f);

			ImGui::EndChild();
			ImGui::End();

			ImGui::Render();

			vRenderer->RecordCommandBuffers(Recording::STOP);
			vRenderer->Render();
			break;

		case RendererType::OPENGL:
			OpenGLRenderer* glRenderer;
			glRenderer = dynamic_cast<OpenGLRenderer*>(renderer);
			/// Clear the screen
			glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glEnable(GL_DEPTH_TEST);
			glEnable(GL_CULL_FACE);
			/// Draw your scene here
			glUseProgram(0);
		
			break;
		}
}


void Scene0::OnDestroy() {
	VulkanRenderer* vRenderer;
	vRenderer = dynamic_cast<VulkanRenderer*>(renderer);
	if(vRenderer){
		vkDeviceWaitIdle(vRenderer->getDevice());
		vRenderer->DestroyCommandBuffers();

		size_t acquired = actorPool.FreeCount(); // Get pre release free space in pool (ie how many objects we used - pool size)

		// Destroy all actors
		for (auto actor : actors)
		{
			actor.second->OnDestroy(vRenderer);
			actorPool.Release(actor.second);			
		}

		size_t released = actorPool.FreeCount(); // Get post release free space in pool (ideally pool size)

		std::cout << "Released: " << released - acquired << " actors\n"; // Subtract released (pool size) from acquired (objects used - pool size) to get total count

		actors.clear();
		vRenderer->DestroyUBO(lightsUBO);

		if (camera) {
			camera->OnDestroy();
			delete camera;
			camera = nullptr;
		}		
	}	
}