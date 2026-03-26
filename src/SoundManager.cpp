#include "SoundManager.h"
#include <iostream>
#include <cmath>
#include <vector>
#include <algorithm>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// ============================================================
// CONSTRUCTOR / DESTRUCTOR
// ============================================================
SoundManager::SoundManager()
    : soundEnabled(true), masterVolume(1.0f), musicVolume(0.5f)
{
}

SoundManager::~SoundManager() {
    cleanup();
}

bool SoundManager::initialize() {
    createFallbackSounds();
    return true;
}

// ============================================================
// PCM HELPER: builds a mono 44100 Hz 16-bit buffer
// ============================================================

// Laser "pew" – short descending tone with envelope
static sf::SoundBuffer makeLaserShoot() {
    const int sampleRate = 44100;
    const float duration = 0.12f;
    int samples = static_cast<int>(sampleRate * duration);
    std::vector<sf::Int16> data(samples);
    for (int i = 0; i < samples; ++i) {
        float t  = static_cast<float>(i) / sampleRate;
        float env = std::exp(-t * 18.0f);                            // fast decay
        float freq = 900.0f - 700.0f * (t / duration);              // sweep down
        float wave = std::sin(2.0f * (float)M_PI * freq * t)
                   + 0.3f * std::sin(4.0f * (float)M_PI * freq * t); // slight harmonic
        data[i] = static_cast<sf::Int16>(wave * env * 22000.0f);
    }
    sf::SoundBuffer buf;
    buf.loadFromSamples(data.data(), data.size(), 1, sampleRate);
    return buf;
}

// Enemy laser – slightly lower / harsher
static sf::SoundBuffer makeEnemyShoot() {
    const int sampleRate = 44100;
    const float duration = 0.15f;
    int samples = static_cast<int>(sampleRate * duration);
    std::vector<sf::Int16> data(samples);
    for (int i = 0; i < samples; ++i) {
        float t   = static_cast<float>(i) / sampleRate;
        float env = std::exp(-t * 14.0f);
        float freq = 450.0f - 300.0f * (t / duration);
        // square-ish wave for harsher sound
        float wave = (std::sin(2.0f * (float)M_PI * freq * t) > 0) ? 1.0f : -1.0f;
        data[i] = static_cast<sf::Int16>(wave * env * 18000.0f);
    }
    sf::SoundBuffer buf;
    buf.loadFromSamples(data.data(), data.size(), 1, sampleRate);
    return buf;
}

// Explosion – noise burst with rumble
static sf::SoundBuffer makeExplosion() {
    const int sampleRate = 44100;
    const float duration = 0.55f;
    int samples = static_cast<int>(sampleRate * duration);
    std::vector<sf::Int16> data(samples);
    unsigned int seed = 12345;
    auto rng = [&]() -> float {
        seed = seed * 1664525u + 1013904223u;
        return (static_cast<float>(seed & 0xFFFF) / 32768.0f) - 1.0f;
    };
    for (int i = 0; i < samples; ++i) {
        float t   = static_cast<float>(i) / sampleRate;
        float env = std::exp(-t * 5.5f);
        // mix noise with a low rumble tone
        float noise = rng();
        float rumble = std::sin(2.0f * (float)M_PI * 55.0f * t) * 0.4f;
        float wave = (noise * 0.7f + rumble) * env;
        data[i] = static_cast<sf::Int16>(
            std::max(-32767.0f, std::min(32767.0f, wave * 30000.0f)));
    }
    sf::SoundBuffer buf;
    buf.loadFromSamples(data.data(), data.size(), 1, sampleRate);
    return buf;
}

// Power-up pickup – rising arpeggio
static sf::SoundBuffer makePowerUpPickup() {
    const int sampleRate = 44100;
    const float duration = 0.40f;
    int samples = static_cast<int>(sampleRate * duration);
    std::vector<sf::Int16> data(samples);
    // Three notes: C5 E5 G5
    const float notes[] = {523.25f, 659.25f, 783.99f};
    for (int i = 0; i < samples; ++i) {
        float t   = static_cast<float>(i) / sampleRate;
        int note  = static_cast<int>(t / (duration / 3.0f));
        if (note > 2) note = 2;
        float freq = notes[note];
        float env  = std::exp(-std::fmod(t, duration / 3.0f) * 12.0f);
        float wave = std::sin(2.0f * (float)M_PI * freq * t) * env;
        data[i] = static_cast<sf::Int16>(wave * 24000.0f);
    }
    sf::SoundBuffer buf;
    buf.loadFromSamples(data.data(), data.size(), 1, sampleRate);
    return buf;
}

// Shield / damage hit – thud
static sf::SoundBuffer makeShieldHit() {
    const int sampleRate = 44100;
    const float duration = 0.20f;
    int samples = static_cast<int>(sampleRate * duration);
    std::vector<sf::Int16> data(samples);
    unsigned int seed = 99999;
    auto rng = [&]() -> float {
        seed = seed * 1664525u + 1013904223u;
        return (static_cast<float>(seed & 0xFFFF) / 32768.0f) - 1.0f;
    };
    for (int i = 0; i < samples; ++i) {
        float t   = static_cast<float>(i) / sampleRate;
        float env = std::exp(-t * 20.0f);
        float val = (rng() * 0.5f + std::sin(2.0f * (float)M_PI * 200.0f * t) * 0.5f) * env;
        data[i] = static_cast<sf::Int16>(val * 26000.0f);
    }
    sf::SoundBuffer buf;
    buf.loadFromSamples(data.data(), data.size(), 1, sampleRate);
    return buf;
}

// Game Over – descending sad melody
static sf::SoundBuffer makeGameOver() {
    const int sampleRate = 44100;
    const float duration = 1.2f;
    int samples = static_cast<int>(sampleRate * duration);
    std::vector<sf::Int16> data(samples);
    // G4 E4 C4 (sad descending)
    const float notes[] = {392.0f, 329.63f, 261.63f};
    const float noteDur  = duration / 3.0f;
    for (int i = 0; i < samples; ++i) {
        float t    = static_cast<float>(i) / sampleRate;
        int note   = static_cast<int>(t / noteDur);
        if (note > 2) note = 2;
        float local = std::fmod(t, noteDur);
        float freq  = notes[note];
        float env   = 1.0f - (local / noteDur);    // linear fade each note
        float wave  = std::sin(2.0f * (float)M_PI * freq * t)
                    + 0.2f * std::sin(2.0f * (float)M_PI * freq * 2.0f * t);
        data[i] = static_cast<sf::Int16>(wave * env * 22000.0f);
    }
    sf::SoundBuffer buf;
    buf.loadFromSamples(data.data(), data.size(), 1, sampleRate);
    return buf;
}

// Level Complete – ascending fanfare
static sf::SoundBuffer makeLevelComplete() {
    const int sampleRate = 44100;
    const float duration = 1.0f;
    int samples = static_cast<int>(sampleRate * duration);
    std::vector<sf::Int16> data(samples);
    // C5 E5 G5 C6
    const float notes[]   = {523.25f, 659.25f, 783.99f, 1046.5f};
    const float noteDur   = duration / 4.0f;
    for (int i = 0; i < samples; ++i) {
        float t    = static_cast<float>(i) / sampleRate;
        int note   = static_cast<int>(t / noteDur);
        if (note > 3) note = 3;
        float freq = notes[note];
        float local = std::fmod(t, noteDur);
        float env   = std::exp(-local * 5.0f);
        float wave  = std::sin(2.0f * (float)M_PI * freq * t)
                    + 0.15f * std::sin(2.0f * (float)M_PI * freq * 3.0f * t);
        data[i] = static_cast<sf::Int16>(wave * env * 24000.0f);
    }
    sf::SoundBuffer buf;
    buf.loadFromSamples(data.data(), data.size(), 1, sampleRate);
    return buf;
}

// Background pulsing ambient loop (procedural chiptune)
static sf::SoundBuffer makeBackgroundMusic() {
    const int sampleRate = 44100;
    const float duration = 4.0f;  // loop every 4 s
    int samples = static_cast<int>(sampleRate * duration);
    std::vector<sf::Int16> data(samples);

    // Simple bass line: A2 A2 E3 A2 D3 A2 E3 F3
    const float bassNotes[] = {110.0f, 110.0f, 164.81f, 110.0f, 146.83f, 110.0f, 164.81f, 174.61f};
    const float beatDur = duration / 8.0f;
    // Pad / atmosphere: slow-LFO sine
    for (int i = 0; i < samples; ++i) {
        float t = static_cast<float>(i) / sampleRate;

        // Bass
        int beat     = static_cast<int>(t / beatDur);
        if (beat > 7) beat = 7;
        float bFreq  = bassNotes[beat];
        float bLocal = std::fmod(t, beatDur);
        float bEnv   = 0.7f * std::exp(-bLocal * 6.0f) + 0.1f;
        float bass   = std::sin(2.0f * (float)M_PI * bFreq * t) * bEnv;

        // Atmospheric pad (two slowly interacting sines)
        float pad = 0.12f * std::sin(2.0f * (float)M_PI * 220.0f * t
                                    + 0.4f * std::sin(2.0f * (float)M_PI * 0.5f * t));
        pad      += 0.08f * std::sin(2.0f * (float)M_PI * 329.63f * t
                                    + 0.3f * std::sin(2.0f * (float)M_PI * 0.7f * t));

        // Subtle hi-hat tick
        unsigned int seed2 = static_cast<unsigned int>(i * 7 + 42);
        seed2 = seed2 * 1664525u + 1013904223u;
        float hatNoise = (static_cast<float>(seed2 & 0xFFFF) / 32768.0f) - 1.0f;
        float hatEnv   = std::exp(-std::fmod(t, beatDur * 0.5f) * 80.0f) * 0.08f;
        float hat      = hatNoise * hatEnv;

        float mix = bass * 0.55f + pad + hat;
        mix = std::max(-1.0f, std::min(1.0f, mix));
        data[i] = static_cast<sf::Int16>(mix * 20000.0f);
    }
    sf::SoundBuffer buf;
    buf.loadFromSamples(data.data(), data.size(), 1, sampleRate);
    return buf;
}

// ============================================================
// CREATE / LOAD ALL SOUNDS
// ============================================================
void SoundManager::createFallbackSounds() {
    struct SoundEntry {
        SoundType  type;
        sf::SoundBuffer (*factory)();
        float volume;
    };

    SoundEntry entries[] = {
        { SoundType::PLAYER_SHOOT,    makeLaserShoot,    70.0f },
        { SoundType::ENEMY_SHOOT,     makeEnemyShoot,    55.0f },
        { SoundType::EXPLOSION,       makeExplosion,     85.0f },
        { SoundType::POWERUP_PICKUP,  makePowerUpPickup, 80.0f },
        { SoundType::POWERUP_ACTIVATE,makePowerUpPickup, 90.0f },
        { SoundType::SHIELD_HIT,      makeShieldHit,     65.0f },
        { SoundType::GAME_OVER,       makeGameOver,      90.0f },
        { SoundType::LEVEL_COMPLETE,  makeLevelComplete, 90.0f },
    };

    for (auto& e : entries) {
        auto buf  = std::make_unique<sf::SoundBuffer>(e.factory());
        auto snd  = std::make_unique<sf::Sound>(*buf);
        snd->setVolume(e.volume * masterVolume);
        sounds[e.type]       = std::move(snd);
        soundBuffers[e.type] = std::move(buf);
    }

    // Background music via sf::Music streaming from a temp buffer stored as SoundBuffer
    // We store it as BACKGROUND_MUSIC SoundBuffer and play looped via sf::Sound
    auto bgBuf = std::make_unique<sf::SoundBuffer>(makeBackgroundMusic());
    auto bgSnd = std::make_unique<sf::Sound>(*bgBuf);
    bgSnd->setLoop(true);
    bgSnd->setVolume(musicVolume * 100.0f * masterVolume);
    sounds[SoundType::BACKGROUND_MUSIC]       = std::move(bgSnd);
    soundBuffers[SoundType::BACKGROUND_MUSIC] = std::move(bgBuf);

    std::cout << "SoundManager: " << (int)(sizeof(entries)/sizeof(entries[0]) + 1)
              << " procedural sounds generated." << std::endl;
}

void SoundManager::loadUserSounds() {
    // Optional – extend to load .ogg/.wav files here in future
}

// ============================================================
// PLAY
// ============================================================
void SoundManager::playSound(SoundType type) {
    if (!soundEnabled) return;
    auto it = sounds.find(type);
    if (it != sounds.end() && it->second) {
        it->second->stop();
        it->second->play();
    }
}

void SoundManager::playBackgroundMusic(bool loop) {
    auto it = sounds.find(SoundType::BACKGROUND_MUSIC);
    if (it != sounds.end() && it->second) {
        it->second->setLoop(loop);
        if (it->second->getStatus() != sf::Sound::Playing) {
            it->second->play();
        }
    }
}

void SoundManager::stopBackgroundMusic() {
    auto it = sounds.find(SoundType::BACKGROUND_MUSIC);
    if (it != sounds.end() && it->second) {
        it->second->stop();
    }
}

// ============================================================
// VOLUME CONTROL
// ============================================================
void SoundManager::setMasterVolume(float volume) {
    masterVolume = std::max(0.0f, std::min(1.0f, volume));
    for (auto& [type, snd] : sounds) {
        if (snd && type != SoundType::BACKGROUND_MUSIC) {
            snd->setVolume(snd->getVolume()); // re-apply master (simplified)
        }
    }
}

void SoundManager::setMusicVolume(float volume) {
    musicVolume = std::max(0.0f, std::min(1.0f, volume));
    auto it = sounds.find(SoundType::BACKGROUND_MUSIC);
    if (it != sounds.end() && it->second) {
        it->second->setVolume(musicVolume * 100.0f);
    }
}

float SoundManager::getMasterVolume() const { return masterVolume; }
float SoundManager::getMusicVolume()  const { return musicVolume;  }

void SoundManager::setSoundEnabled(bool enabled) { soundEnabled = enabled; }
bool SoundManager::isSoundEnabled()  const       { return soundEnabled;    }

void SoundManager::cleanup() {
    for (auto& [type, snd] : sounds) {
        if (snd) snd->stop();
    }
    sounds.clear();
    soundBuffers.clear();
}
