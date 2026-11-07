#pragma once

#include <fmod.hpp>
#include <fmod_errors.h>
#include <unordered_map>
#include <string>



class Audio
{
private:
	struct AudioStruct
	{
		std::unordered_map<std::string, FMOD::Sound*> loadedSounds; // Name, Sound (sound file)
		std::unordered_map<std::string, FMOD::Channel*> activeChannels; // soundName (bound to sound in soundMap), Channel
	};

	FMOD::System* fmodSystem = nullptr;

	AudioStruct internal; // Internal data of audio object (sounds and channels)

public:
	Audio();
	~Audio();

	// Summary: Loads sound file into memory from files
	// soundName: Any name - Use exact name in soundMap for use
	// soundFilePath: Exact path for sound file (including the file itself and its type)
	// mode: Use FMOD_DEFAULT or 0 unless you want to change behavior of the file (ie loop infinitely, stream from disk, 3d/2d etc..)
	// soundInfo: Leave nullptr unless you want things like procedural audio or playing only part of the sound file.
	//
	// Returns success/fail
	bool loadSoundFromFile(const std::string soundName, const char* soundFilePath, FMOD_MODE mode, FMOD_CREATESOUNDEXINFO* soundInfo);

	// Summary: Plays sound based off name (can start paused)
	// soundName: The name you made when loading sound from file (not file name itself, unless you named it that)
	// paused: Creates channel in paused state
	// 
	// Returns success/fail
	bool playSound(const std::string soundName, bool paused);

	// Summary: Manually Stops active channels and clears channel (does not clear any loaded sounds from internal only channels)
	// soundName: The name you made when loading sound from file (not file name itself, unless you named it that)
	//
	// Return success/fail
	bool stopSound(const std::string soundName);

	// Summary: Checks if sound channel is currently valid and active
	// soundName: The name you made when loading sound from file (not file name itself, unless you named it that)
	//
	// Returns true if sound is playing else false
	bool isSoundPlaying(const std::string soundName);

	// Checks for stopped sounds per frame
	void Update();

};