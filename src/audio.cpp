#include "audio.h"
#include <iostream>
#include <vector>
#include <cstdio>   // for fopen, fread ��
#define STB_VORBIS_HEADER_ONLY
#include "stb_vorbis.c"  
AudioManager& AudioManager::getInstance() {
    static AudioManager instance;
    return instance;
}
int AudioManager::getNumActiveSources() {
    return activeSources.size();
}
bool AudioManager::init() {
    device = alcOpenDevice(nullptr);
    if (!device) {
        std::cerr << "Failed to open OpenAL device" << std::endl;
        return false;
    }

    context = alcCreateContext(device, nullptr);
    if (!context || !alcMakeContextCurrent(context)) {
        std::cerr << "Failed to create OpenAL context" << std::endl;
        return false;
    }

    alListener3f(AL_POSITION, 0.0f, 0.0f, 0.0f);
    alListener3f(AL_VELOCITY, 0.0f, 0.0f, 0.0f);
    return true;
}

AudioManager::~AudioManager() {
    shutdown();
}

void AudioManager::shutdown() {
    // Clean up all active sources
    for (ALuint source : activeSources) {
        alDeleteSources(1, &source);
    }
    activeSources.clear();

    alcMakeContextCurrent(nullptr);
    if (context) alcDestroyContext(context);
    if (device) alcCloseDevice(device);
}

ALuint AudioManager::loadWAV(const std::string& filename) {
    FILE* file = nullptr;
    errno_t err = fopen_s(&file, filename.c_str(), "rb");
    if (err != 0 || file == nullptr) {
        std::cerr << "Failed to open WAV file: " << filename << std::endl;
        return 0;
    }

    char chunkID[4];
    uint32_t chunkSize;
    char format[4];

    // RIFF header
    if (fread(chunkID, 1, 4, file) != 4 ||
        fread(&chunkSize, 4, 1, file) != 1 ||
        fread(format, 1, 4, file) != 4) {
        std::cerr << "Failed to read WAV header: " << filename << std::endl;
        fclose(file);
        return 0;
    }

    if (strncmp(chunkID, "RIFF", 4) != 0 || strncmp(format, "WAVE", 4) != 0) {
        std::cerr << "Invalid WAV header: " << filename << std::endl;
        fclose(file);
        return 0;
    }

    // ���� fmt �� data chunk
    uint16_t audioFormat = 0, numChannels = 0, bitsPerSample = 0;
    uint32_t sampleRate = 0, dataSize = 0;
    char subchunkID[4];
    uint32_t subchunkSize;
    std::vector<char> soundData;

    while (fread(subchunkID, 1, 4, file) == 4) {
        fread(&subchunkSize, 4, 1, file);

        if (strncmp(subchunkID, "fmt ", 4) == 0) {
            fread(&audioFormat, 2, 1, file);
            fread(&numChannels, 2, 1, file);
            fread(&sampleRate, 4, 1, file);
            uint32_t byteRate;
            uint16_t blockAlign;
            fread(&byteRate, 4, 1, file);
            fread(&blockAlign, 2, 1, file);
            fread(&bitsPerSample, 2, 1, file);

            // ����ʣ��� fmt ���ݣ���� subchunkSize > 16��
            if (subchunkSize > 16) {
                fseek(file, subchunkSize - 16, SEEK_CUR);
            }
        }
        else if (strncmp(subchunkID, "data", 4) == 0) {
            dataSize = subchunkSize;
            soundData.resize(dataSize);
            size_t readBytes = fread(soundData.data(), 1, dataSize, file);
            if (readBytes != dataSize) {
                std::cerr << "Failed to read audio data: " << filename << std::endl;
                fclose(file);
                return 0;
            }
            break;
        }
        else {
            // ����δ֪ chunk
            fseek(file, subchunkSize, SEEK_CUR);
        }
    }

    fclose(file);

    if (dataSize == 0 || audioFormat != 1 || (bitsPerSample != 8 && bitsPerSample != 16)) {
        std::cerr << "Unsupported WAV format: " << filename
            << " (only 8/16-bit uncompressed PCM supported)" << std::endl;
        return 0;
    }

    ALenum alFormat = 0;
    if (numChannels == 1) {
        alFormat = (bitsPerSample == 16) ? AL_FORMAT_MONO16 : AL_FORMAT_MONO8;
    }
    else if (numChannels == 2) {
        alFormat = (bitsPerSample == 16) ? AL_FORMAT_STEREO16 : AL_FORMAT_STEREO8;
    }
    else {
        std::cerr << "Unsupported channel count: " << numChannels << std::endl;
        return 0;
        
    }

    ALuint buffer;
    alGenBuffers(1, &buffer);
    alBufferData(buffer, alFormat, soundData.data(), static_cast<ALsizei>(dataSize), static_cast<ALsizei>(sampleRate));

    ALenum error = alGetError();
    if (error != AL_NO_ERROR) {
        std::cerr << "OpenAL error while loading buffer: " << error << std::endl;
        alDeleteBuffers(1, &buffer);
        return 0;
    }
    
   
    return buffer;

}

void AudioManager::playSound(ALuint buffer, float volume, float pitch) {
    if (buffer == 0) return;

    ALuint source;
    alGenSources(1, &source);

    alSourcei(source, AL_BUFFER, static_cast<ALint>(buffer));
    alSourcef(source, AL_GAIN, volume);
    alSourcef(source, AL_PITCH, pitch);
    alSourcei(source, AL_LOOPING, AL_FALSE);
    alSource3f(source, AL_POSITION, 0.0f, 0.0f, 0.0f);

    alSourcePlay(source);
    activeSources.push_back(source);
    std::cout<<"successfully played sound"<<std::endl;
}

void AudioManager::update() {
    // Check all active sources
    for (auto it = activeSources.begin(); it != activeSources.end();) {
        ALuint source = *it;
        ALint state;
        alGetSourcei(source, AL_SOURCE_STATE, &state);

        if (state != AL_PLAYING) {
            // Source is finished, clean it up

            alDeleteSources(1, &source);
            it = activeSources.erase(it);
        } else {
            ++it;
        }
    }
}
ALuint AudioManager::loadOGG(const std::string& filename) {
    int channels, sampleRate;
    short* data;
    int samples = stb_vorbis_decode_filename(filename.c_str(), &channels, &sampleRate, &data);

    if (samples <= 0) {
        std::cerr << "Failed to decode OGG file: " << filename << std::endl;
        return 0;
    }

    ALenum format = (channels == 1) ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;

    ALuint buffer;
    alGenBuffers(1, &buffer);
    alBufferData(buffer, format, data, samples * channels * sizeof(short), sampleRate);

    free(data);  // stb_vorbis 

    ALenum error = alGetError();
    if (error != AL_NO_ERROR) {
        std::cerr << "OpenAL error while loading OGG buffer: " << error << std::endl;
        alDeleteBuffers(1, &buffer);
        return 0;
    }
   
    
    return buffer;
}

