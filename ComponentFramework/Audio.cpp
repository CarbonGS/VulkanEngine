#include "Audio.h"
#include <iostream>

Audio::Audio()
{
	FMOD::System_Create(&fmodSystem);
	fmodSystem->init(32, FMOD_INIT_NORMAL, nullptr);
}

Audio::~Audio()
{
	auto map = &internal.loadedSounds;

	for (auto& [name, sound] : internal.loadedSounds)
	{
		sound->release(); // Release all sounds currently mapped
	}
	for (auto& [name, channel] : internal.activeChannels)
	{
		channel->stop(); // Stop all channels
	}

	internal.loadedSounds.clear();
	internal.activeChannels.clear();

	fmodSystem->close();
	fmodSystem->release();
}


bool Audio::loadSoundFromFile(const std::string soundName, const char* soundFilePath, FMOD_MODE mode, FMOD_CREATESOUNDEXINFO* soundInfo)
{
	FMOD_RESULT result = fmodSystem->createSound(soundFilePath, mode, soundInfo, &internal.loadedSounds[soundName]);

	if (result == FMOD_OK)
	{
		std::cout << "Loaded sound: " << soundName << std::endl;
		return true; // Created sound
	}

	internal.loadedSounds.erase(soundName);

	std::cout << "Failed to load sound: " << soundName << " FMOD ERROR: " << FMOD_ErrorString(result) << std::endl;

	return false; // Sound not created
}

bool Audio::playSound(const std::string soundName, bool paused)
{

	auto it = internal.loadedSounds.find(soundName);

	if (it == internal.loadedSounds.end())
	{
		std::cout << "Sound not found: " << soundName << std::endl;
		return false;
	}

	FMOD::Channel* channel = nullptr;

	FMOD_RESULT result = fmodSystem->playSound(it->second, nullptr, paused, &channel);

	internal.activeChannels[soundName] = channel; // store active channel

	if (result == FMOD_OK) 
		return true; // Returns true if sound was played

	std::cout << "Sound not played: " << soundName << " FMOD ERROR: " << FMOD_ErrorString(result) << std::endl;
	return false; // Sound failed to play
}

bool Audio::stopSound(const std::string soundName)
{
	auto it = internal.activeChannels.find(soundName);

	if (it == internal.activeChannels.end()) // 
	{
		std::cout << "Could not stop sound: " << soundName << std::endl;
		return false;
	}

	it->second->stop(); // stop playing audio
	auto channelData = internal.activeChannels.find(soundName); // Get our sound that occupied said channel
	internal.activeChannels.erase(channelData); // Remove channel from active channels

	return true; // Succesfully stopped sound
}

bool Audio::isSoundPlaying(const std::string soundName)
{
	auto it = internal.activeChannels.find(soundName); // Get active channel of soundName

	if (it == internal.activeChannels.end()) // Failed to find sound with active channel
	{
		return false;
	}

	bool result = false;

	it->second->isPlaying(&result); // Record result of channel that target sound is playing

	if (!result) // Sound is not playing
	{
		return false;
	}
	
	return true;
}

void Audio::Update()
{
	for (auto& channel : internal.activeChannels)
	{
		if (channel.second != nullptr) {
			if (!isSoundPlaying(channel.first)) // Checks if channel is currently inactive
			{
				stopSound(channel.first); // Clears inactive channels
			}
		}
	}
}






