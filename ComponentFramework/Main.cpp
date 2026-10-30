#include <iostream>
#include <string>
#include "Debug.h"
#include "SceneManager.h"
#include "MemoryMonitor.h"
#include "MMath.h"
using namespace MATH;

int main(int argc, char* args[]) {
	//_CrtSetBreakAlloc(4123);
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	
	
#pragma warning(disable : 4996) 
	/*if (const char* env_p = std::getenv("PATH")) {
		std::cout << "Your PATH is: " << env_p << '\n';
	}*/

	static_assert(sizeof(int*) == 8, "Vulkan no longer supports x86 builds");

	Debug::DebugInit("Vulkan Graphics Engine Log");
	Debug::Info("Starting the GameSceneManager", __FILE__, __LINE__);
	SceneManager* gsm = new SceneManager();
	if (gsm->Initialize("Vulkan Graphics Engine", 1280, 720) ==  true) {
		gsm->Run();
	} 
	delete gsm;
	checkForLeaks();
	exit(0);

}