#pragma once

void initAudio();
void unloadAudio();
void pushSound(float (*waveform)(float), float volume, float value, float duration);
float sineWave(float x);
float triangleWave(float x);