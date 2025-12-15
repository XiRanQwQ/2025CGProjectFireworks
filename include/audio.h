#ifndef AUDIO_H
#define AUDIO_H

#include <AL/al.h>
#include <AL/alc.h>
#include <string>
#include <vector>

class AudioManager {
public:
    static AudioManager& getInstance();

    bool init();

    void shutdown();

    // Load WAV file, supports 8/16-bit PCM
    ALuint loadWAV(const std::string& filename);

    // Load OGG file
    ALuint loadOGG(const std::string& filename);

    // Play sound with specified volume and pitch
    void playSound(ALuint buffer, float volume = 1.0f, float pitch = 1.0f);

    // Update audio sources, clean up finished sounds
    void update();

    int getNumActiveSources();

private:
    AudioManager() = default;
    ~AudioManager();

    ALCdevice* device = nullptr;
    ALCcontext* context = nullptr;
    std::vector<ALuint> activeSources;
};

#endif // AUDIO_H