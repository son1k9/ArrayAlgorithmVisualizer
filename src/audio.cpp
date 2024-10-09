#include <list>
#include <mutex>
#include "math.h"
#include "raylib.h"
#include "audio.h"

constexpr int sampleRate = 44100;

float sineWave(float x) {
    return sinf(2 * PI * x);
}

float triangleWave(float x) {
    float intermediate = fmod(x + .25f, 1.0f);
    return intermediate < .5f ? 4 * intermediate - 1 : -4 * intermediate + 3;
}

static float frequency(float value) {
    return 1320.0f * value;
}

struct SoundData
{
    /** The waveform of the sound
     * @param generic_parameter_name A float representing the number of waves plus the portion of a wave passed
     * @returns A float representing the height of the wave at the time inputted
     */
    float (*waveform)(float);
    /** The volume of the sound */
    float volume;
    /** The value of the array item represented by this sound to be converted into its frequency; should be between 0 and 1 */
    float value;
    /** The duration of the sound; how long it should sustain (in seconds) */
    float duration;
    /** An internal variable whose initial value should be set to 0 representing the amound of time (in seconds) elapsed since the sound begun playing */
    float elapsed;
    /** An internal variable whose initial value should be set to 1 representing the portion of the sound's amplitude that should remain at this point */
    float remaining_amplitude;
};

std::mutex soundListMutex;
std::list<SoundData> soundList;

void pushSound(float (*waveform)(float), float volume, float value, float duration) {
    soundListMutex.lock();
    soundList.push_back(SoundData{ waveform, volume, value, duration, 0.0f, 1.0f });
    soundListMutex.unlock();
}

static short nextSample() {
    float accumulated_amplitude = 0.0f;
    soundListMutex.lock();
    for (auto currentItem = soundList.begin(); currentItem != soundList.end(); ) {
        accumulated_amplitude += currentItem->waveform(frequency(currentItem->value) * currentItem->elapsed) * currentItem->volume * currentItem->remaining_amplitude;
        currentItem->remaining_amplitude -= 1.0f / currentItem->duration / sampleRate;

        if (currentItem->remaining_amplitude >= 0) {
            currentItem->elapsed += 1.0f / sampleRate;
            currentItem++;
        }
        else {
            currentItem = soundList.erase(currentItem);
        }
    }
    soundListMutex.unlock();
    return accumulated_amplitude >= 1 ? 32767 : accumulated_amplitude < -1 ? -32768 : accumulated_amplitude * 32768.0f;
}

static AudioStream audio_stream;

static void audioCallback(void* buffer, unsigned int num_samples) {
    for (unsigned int i = 0; i < num_samples; i++) {
        static_cast<short*>(buffer)[i] = nextSample();
    }
}

void initAudio() {
    audio_stream = LoadAudioStream(sampleRate, 16, 1);
    SetAudioStreamCallback(audio_stream, audioCallback);
    PlayAudioStream(audio_stream);
};

void unloadAudio() {
    StopAudioStream(audio_stream);
    UnloadAudioStream(audio_stream);
}