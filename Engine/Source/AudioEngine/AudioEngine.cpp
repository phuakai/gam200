/******************************************************************************/
/*!
\file   audioengine.cpp
\author ow xin rui, pang tun dagh
\par    email: xinrui.ow@digipen.edu, tundagh.pang@digipen.edu
\par    digipen login: xinrui.ow
\par    course: csd2400
\brief
  the following file contains the functions that help to get audio files from the 
  resources folder and then plays and releases the audio files.
*/
/******************************************************************************/

#include <audioengine.h>

namespace spooky
{
    // audio engine constructor
    Implementation::Implementation()
    {
        result = FMOD::System_Create(&mpSystem);
        result = mpSystem->getVersion(&mpVersion);
        /*if (mpversion < fmod_version)
        {
          errorif(true, "fmod lib version doesn't match header version\n");
        }*/
        CAudioEngine::ErrorCheck(result = mpSystem->init(32, FMOD_INIT_NORMAL, extradriverdata));
    }

    // audio engine desturctor
    Implementation::~Implementation() {}

    // audio engine update
    void Implementation::Update() {
        std::vector<ChannelMap::iterator> pstoppedchannels;
        for (auto it = mChannels.begin(), itend = mChannels.end(); it != itend; ++it)
        {
            bool bisplaying = false;
            it->second->isPlaying(&bisplaying);
            if (!bisplaying)
            {
                pstoppedchannels.push_back(it);
            }
        }
        for (auto& it : pstoppedchannels)
        {
            mChannels.erase(it);
        }
        CAudioEngine::ErrorCheck(mpSystem->update());
    }

    Implementation* sgpimplementation = nullptr;

    // audio engine init
    void CAudioEngine::Init() {
        sgpimplementation = new Implementation;
    }

    // audio engine update
    void CAudioEngine::Update() {
        sgpimplementation->Update();
    }

    // loads audio file from resource folder
    void CAudioEngine::LoadSound(const std::string& strsoundname, bool b3d, bool blooping, bool bstream)
    {
        auto tfoundit = sgpimplementation->mSounds.find(strsoundname);
        if (tfoundit != sgpimplementation->mSounds.end())
            return;

        FMOD_MODE emode = FMOD_DEFAULT;
        emode |= b3d ? FMOD_3D : FMOD_2D;
        emode |= blooping ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF;
        emode |= bstream ? FMOD_CREATESTREAM : FMOD_CREATECOMPRESSEDSAMPLE;

        FMOD::Sound* psound = nullptr;
        CAudioEngine::ErrorCheck(sgpimplementation->mpSystem->createSound(strsoundname.c_str(), emode, 0, &psound));
        if (psound) {
            sgpimplementation->mSounds[strsoundname] = psound;
        }
    }

    // unloads audio file
    void CAudioEngine::UnLoadSound(const std::string& strsoundname)
    {
        auto tfoundit = sgpimplementation->mSounds.find(strsoundname);
        if (tfoundit == sgpimplementation->mSounds.end())
            return;

        CAudioEngine::ErrorCheck(tfoundit->second->release());
        sgpimplementation->mSounds.erase(tfoundit);
    }

    // plays the audio file
    int CAudioEngine::PlaySound(const std::string& strsoundname, const Vector2& vposition, float fvolumedb)
    {
        int nchannelid = sgpimplementation->mnNextChannelId++;
        auto tfoundit = sgpimplementation->mSounds.find(strsoundname);
        if (tfoundit == sgpimplementation->mSounds.end())
        {
            LoadSound(strsoundname, false);
            tfoundit = sgpimplementation->mSounds.find(strsoundname);
            if (tfoundit == sgpimplementation->mSounds.end())
            {
                return nchannelid;
            }
        }
        FMOD::Channel* pchannel = nullptr;
        CAudioEngine::ErrorCheck(sgpimplementation->mpSystem->playSound(tfoundit->second, nullptr, true, &pchannel));
        if (pchannel)
        {
            FMOD_MODE currmode;
            tfoundit->second->getMode(&currmode);
            if (currmode & FMOD_3D) {
                FMOD_VECTOR position = VectorToFmod(vposition);
                CAudioEngine::ErrorCheck(pchannel->set3DAttributes(&position, nullptr));
            }
            CAudioEngine::ErrorCheck(pchannel->setVolume(dbToVolume(fvolumedb)));
            CAudioEngine::ErrorCheck(pchannel->setPaused(false));
            sgpimplementation->mChannels[nchannelid] = pchannel;
        }
        return nchannelid;
    }

    //stops the channel being played
    void CAudioEngine::StopChannel(int nchannelid)
    {
        sgpimplementation->mChannels[nchannelid] = nullptr;
    }

    //stops all channels from playing
    void CAudioEngine::StopAllChannels()
    {
        while (!sgpimplementation->mChannels.empty())
        {
            int counter{ 0 };
            StopChannel(counter);
            counter++;
        }
    }

    // sets channel volume
    void CAudioEngine::SetChannelVolume(int nchannelid, float fvolumedb)
    {
        auto tfoundit = sgpimplementation->mChannels.find(nchannelid);
        if (tfoundit == sgpimplementation->mChannels.end())
            return;

        CAudioEngine::ErrorCheck(tfoundit->second->setVolume(dbToVolume(fvolumedb)));
    }

    //checks if the channel is still playing audio
    bool CAudioEngine::IsPlaying(int nchannelid) const
    {
        auto tfoundit = sgpimplementation->mChannels.find(nchannelid);
        return (tfoundit == sgpimplementation->mChannels.end()) ? true : false;
    }

    // converts struct vector to fmod vector
    FMOD_VECTOR CAudioEngine::VectorToFmod(const Vector2& vposition) {
        FMOD_VECTOR fvec;
        fvec.x = vposition.x;
        fvec.y = vposition.y;
        //fvec.z = vposition.z;
        return fvec;
    }

    float CAudioEngine::dbToVolume(float db)
    {
        return powf(10.0f, 0.05f * db);
    }

    // volume control (1.0f max)
    float CAudioEngine::VolumeTodb(float volume)
    {
        return 20.0f * log10f(volume);
    }

    // fmod error checking output
    int CAudioEngine::ErrorCheck(FMOD_RESULT result) {
        /*if (result != fmod_ok) {
            std::cout << "fmod error " << std::endl;
            errorif(true, fmod_errorstring(result));
            return 1;
        }*/
        // cout << "fmod all good" << endl;
        return 0;
    }

    // deletes audio engine
    void CAudioEngine::Shutdown() {
        delete sgpimplementation;
    }
}
