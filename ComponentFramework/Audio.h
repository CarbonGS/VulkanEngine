#pragma once

#include <fmod.hpp>
#include <unordered_map>
#include <string>

class Audio
{
private:
	FMOD::System* fmodSystem = nullptr;

	std::unordered_map<std::string, FMOD::Sound*> soundMap; // Name, Sound (sound file)

public:
	Audio();
	~Audio();

	// soundName: Any name - Use exact name in soundMap for use
	// soundFile: Exact soundfile name and type
	// mode: Use FMOD_DEFAULT or 0 unless you want to change behavior of the file (ie loop infinitely, stream from disk, 3d/2d etc..)
	// soundInfo: Leave nullptr unless you want things like procedural audio or playing only part of the sound file.
	//
	// Returns success/fail
	bool loadSoundFromFile(const std::string soundName, const char* soundFile, FMOD_MODE mode, FMOD_CREATESOUNDEXINFO* soundInfo);

	// soundName: The name you made when loading sound from file (not file name itself, unless you named it that)
	// paused: Creates channel in paused state
	// 
	// Returns success/fail
	bool playSound(const std::string soundName, bool paused);

};