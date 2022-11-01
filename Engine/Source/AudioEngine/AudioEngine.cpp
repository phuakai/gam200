/******************************************************************************/
/*!
\file   AudioEngine.cpp
\author Ow Xin Rui, Pang Tun Dagh
\par    email: xinrui.ow@digipen.edu, tundagh.pang@digipen.edu
\par    DigiPen login: xinrui.ow
\par    Course: CSD2400
\brief
  The following file contains the functions that help to get audio files from the 
  resources folder and then plays and releases the audio files.
*/
/******************************************************************************/

#include <AudioEngine.h>

namespace spooky
{
    // Audio Engine Constructor
    Implementation::Implementation()
    {
        result = FMOD::System_Create(&mpSystem);
        result = mpSystem->getVersion(&mpVersion);
        /*if (mpVersion < FMOD_VERSION)
        {
          ErrorIf(true, "FMOD lib version doesn't match header version\n");
        }*/
        CAudioEngine::ErrorCheck(result = mpSystem->init(32, FMOD_INIT_NORMAL, extradriverdata));
    }

    // Audio Engine Desturctor
    Implementation::~Implementation() {}

    // Audio Engine Update
    void Implementation::Update() {
        std::vector<ChannelMap::iterator> pStoppedChannels;
        for (auto it = mChannels.begin(), itEnd = mChannels.end(); it != itEnd; ++it)
        {
            bool bIsPlaying = false;
            it->second->isPlaying(&bIsPlaying);
            if (!bIsPlaying)
            {
                pStoppedChannels.push_back(it);
            }
        }
        for (auto& it : pStoppedChannels)
        {
            mChannels.erase(it);
        }
        CAudioEngine::ErrorCheck(mpSystem->update());
    }

    Implementation* sgpImplementation = nullptr;

    // Audio Engine Init
    void CAudioEngine::Init() {
        sgpImplementation = new Implementation;
    }

    // Audio Engine Update
    void CAudioEngine::Update() {
        sgpImplementation->Update();
    }

    // Loads audio file from Resource folder
    void CAudioEngine::LoadSound(const std::string& strSoundName, bool b3d, bool bLooping, bool bStream)
    {
        auto tFoundIt = sgpImplementation->mSounds.find(strSoundName);
        if (tFoundIt != sgpImplementation->mSounds.end())
            return;

        FMOD_MODE eMode = FMOD_DEFAULT;
        eMode |= b3d ? FMOD_3D : FMOD_2D;
        eMode |= bLooping ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF;
        eMode |= bStream ? FMOD_CREATESTREAM : FMOD_CREATECOMPRESSEDSAMPLE;

        FMOD::Sound* pSound = nullptr;
        CAudioEngine::ErrorCheck(sgpImplementation->mpSystem->createSound(strSoundName.c_str(), eMode, 0, &pSound));
        if (pSound) {
            sgpImplementation->mSounds[strSoundName] = pSound;
        }
    }

    // Unloads audio file
    void CAudioEngine::UnLoadSound(const std::string& strSoundName)
    {
        auto tFoundIt = sgpImplementation->mSounds.find(strSoundName);
        if (tFoundIt == sgpImplementation->mSounds.end())
            return;

        CAudioEngine::ErrorCheck(tFoundIt->second->release());
        sgpImplementation->mSounds.erase(tFoundIt);
    }

    // Plays the audio file
    int CAudioEngine::PlaySound(const std::string& strSoundName, const Vector2& vPosition, float fVolumedB)
    {
        int nChannelId = sgpImplementation->mnNextChannelId++;
        auto tFoundIt = sgpImplementation->mSounds.find(strSoundName);
        if (tFoundIt == sgpImplementation->mSounds.end())
        {
            LoadSound(strSoundName, false);
            tFoundIt = sgpImplementation->mSounds.find(strSoundName);
            if (tFoundIt == sgpImplementation->mSounds.end())
            {
                return nChannelId;
            }
        }
        FMOD::Channel* pChannel = nullptr;
        CAudioEngine::ErrorCheck(sgpImplementation->mpSystem->playSound(tFoundIt->second, nullptr, true, &pChannel));
        if (pChannel)
        {
            FMOD_MODE currMode;
            tFoundIt->second->getMode(&currMode);
            if (currMode & FMOD_3D) {
                FMOD_VECTOR position = VectorToFmod(vPosition);
                CAudioEngine::ErrorCheck(pChannel->set3DAttributes(&position, nullptr));
            }
            CAudioEngine::ErrorCheck(pChannel->setVolume(dbToVolume(fVolumedB)));
            CAudioEngine::ErrorCheck(pChannel->setPaused(false));
            sgpImplementation->mChannels[nChannelId] = pChannel;
        }
        return nChannelId;
    }

    //stops the Channel being played
    void CAudioEngine::StopChannel(int nChannelId)
    {
        sgpImplementation->mChannels[nChannelId] = nullptr;
    }

    //stops all Channels from playing
    void CAudioEngine::StopAllChannels()
    {
        while (!sgpImplementation->mChannels.empty())
        {
            int counter{ 0 };
            StopChannel(counter);
            counter++;
        }
    }

    // Sets Channel Volume
    void CAudioEngine::SetChannelVolume(int nChannelId, float fVolumedB)
    {
        auto tFoundIt = sgpImplementation->mChannels.find(nChannelId);
        if (tFoundIt == sgpImplementation->mChannels.end())
            return;

        CAudioEngine::ErrorCheck(tFoundIt->second->setVolume(dbToVolume(fVolumedB)));
    }

    //checks if the channel is still playing audio
    bool CAudioEngine::IsPlaying(int nChannelId) const
    {
        auto tFoundIt = sgpImplementation->mChannels.find(nChannelId);
        return (tFoundIt == sgpImplementation->mChannels.end()) ? true : false;
    }

    // Converts Struct Vector to FMOD Vector
    FMOD_VECTOR CAudioEngine::VectorToFmod(const Vector2& vPosition) {
        FMOD_VECTOR fVec;
        fVec.x = vPosition.x;
        fVec.y = vPosition.y;
        //fVec.z = vPosition.z;
        return fVec;
    }

    float CAudioEngine::dbToVolume(float dB)
    {
        return powf(10.0f, 0.05f * dB);
    }

    // Volume Control (1.0f Max)
    float CAudioEngine::VolumeTodb(float volume)
    {
        return 20.0f * log10f(volume);
    }

    // FMOD Error Checking output
    int CAudioEngine::ErrorCheck(FMOD_RESULT result) {
        /*if (result != FMOD_OK) {
            std::cout << "FMOD ERROR " << std::endl;
            ErrorIf(true, FMOD_ErrorString(result));
            return 1;
        }*/
        // cout << "FMOD all good" << endl;
        return 0;
    }

    // Deletes Audio Engine
    void CAudioEngine::Shutdown() {
        delete sgpImplementation;
    }
}
