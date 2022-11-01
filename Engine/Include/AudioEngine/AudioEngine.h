/******************************************************************************/
/*!
\file   AudioEngine.h
\author Ow Xin Rui, Pang Tun Dagh
\par    email: xinrui.ow@digipen.edu, tundagh.pang@digipen.edu
\par    DigiPen login: xinrui.ow
\par    Course: CSD2400
\brief
  The following file contains the function declarations for the Audio Engine 
  system.
*/
/******************************************************************************/

#ifndef _AUDIO_ENGINE_H_
#define _AUDIO_ENGINE_H_

#include <fmod.hpp>
#include <fmod_errors.h>
#include <string>
#include <map>
#include <vector>
#include <iostream>

namespace spooky
{   
    struct Vector2
    {
        float x;
        float y;
    };
    //Vector_2D Vec2;

    struct Implementation { //audio engine constructor struct, handles the channels and sounds proper
        Implementation();   //audio engine constructor
        ~Implementation();  //audio engine destructor

        void Update();      //update loop
        FMOD_RESULT result;
        FMOD::System* mpSystem;
        unsigned int mpVersion;
        void* extradriverdata = 0;

        int mnNextChannelId = 0;

        typedef std::map<std::string, FMOD::Sound*> SoundMap;
        typedef std::map<int, FMOD::Channel*> ChannelMap;

        SoundMap mSounds;
        ChannelMap mChannels;
    };

    class CAudioEngine {    //audio engine functions
    public:
        static void Init();     //initialises the audio engine using implementation class
        static void Update();   //update loop
        static void Shutdown(); //shuts down audio engine
        static int ErrorCheck(FMOD_RESULT result);

        void LoadSound(const std::string& strSoundName, bool b3d = true, bool bLooping = false, bool bStream = false);
        void UnLoadSound(const std::string& strSoundName);
        //void Set2dListenerAndOrientation(const Vector2& vPos = Vector2{ 0, 0 }, float fVolumedB = 0.0f);
        int PlaySound(const std::string& strSoundName, const Vector2& vPos = Vector2{ 0, 0 }, float fVolumedB = 0.0f);
        void StopChannel(int nChannelId);
        void StopAllChannels();
        void SetChannelVolume(int nChannelId, float fVolumedB);
        bool IsPlaying(int nChannelId) const;
        float dbToVolume(float db);
        float VolumeTodb(float volume);
        FMOD_VECTOR VectorToFmod(const Vector2& vPosition);
    };
}

#endif



