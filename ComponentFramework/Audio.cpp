#include "Audio.h"
#include <iostream>

Audio::Audio()
{
	FMOD::System_Create(&fmodSystem);
	fmodSystem->init(32, FMOD_INIT_NORMAL, nullptr);
}

Audio::~Audio()
{
	for (auto& sound : soundMap)
	{
		sound.second->release(); // Release all sounds currently mapped
	}
	soundMap.clear();

	fmodSystem->close();
	fmodSystem->release();
}


bool Audio::loadSoundFromFile(const std::string soundName, const char* soundFile, FMOD_MODE mode, FMOD_CREATESOUNDEXINFO* soundInfo)
{
	soundMap[soundName]; // Add sound to map (default nullptr)

	if (fmodSystem->createSound(soundFile, mode, soundInfo, &soundMap[soundName])) return true; // Created sound

	std::cout << "Failed to create sound: " << soundName << std::endl;
	return false; // Return fail if sound was not made
}

bool Audio::playSound(const std::string soundName, bool paused)
{

	FMOD::Sound* sound = soundMap.find(soundName)->second;

	// If sound is not nullptr
	if (sound)
	{
		FMOD::Channel* channel;

		if (fmodSystem->playSound(sound, nullptr, paused, &channel)) return true; // Returns true if sound was played

		std::cout << "Sound found but could not be played, name: " << soundName << std::endl;
		return false;
	}

	return false; // Sound not found (most likely spelling error)
}




