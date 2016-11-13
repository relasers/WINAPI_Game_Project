#pragma once
#pragma comment (lib, "fmod_vc.lib")
#include "inc\fmod.hpp"
using namespace FMOD;

// 동시실행가능한 사운드(채널) 갯수.
#define Max_Channel 3000

class CSoundManager
{
public:
	///////////////// BGM ////////////////////
	typedef enum 
	{
		Logo_BGM,
		Title_BGM,
		Lobby_BGM,
		Stage_BGM,
		Boss_Stage_BGM,
		GameOver_BGM,
		GameClear_BGM,
		Count_BGM,
		Stop_BGM
	}BGM_SOUND;
	///////////////// UI /////////////////////
	typedef enum
	{
		Lobby_Menu_Select_UI,
		Potion_UI,
		Count_UI,
		Stop_UI
	}UI_SOUND;
	///////////////// PALYER /////////////////
	typedef enum
	{
		Attack_P,
		Walk_P,
		Reload_P,
		Shot_P,
		Deadly_P,
		Hit_P,
		Jump_P,
		Die_P,
		Parrying_P,
		Count_P,
		Stop_P
	}PLAYER_SOUND;
	///////////////// ENEMY //////////////////
	typedef enum
	{
		Enemy_Attack,
		Enemy_Shot,
		Enemy_Move,
		Enemy_Hit,
		Enemy_Guard,
		Enemy_Die,
		Enemy_Count,
		Enemy_Stop
	}ENEMY_SOUND;
	///////////////// BOSS ///////////////////
	typedef enum
	{
		Boss_Phase2_Shift,
		Boss_Phase2_Attack,
		Boss_Phase2_Boom,
		Boss_Phase2_Summon,
		Boss_Phase3_Shift,
		Boss_Phase3_Idle,
		Boss_Die,
		Boss_Count,
		Boss_Stop
	}BOSS_SOUND;

private:
	FMOD_SYSTEM*		g_System{ nullptr };

	FMOD_SOUND*			g_BGM_Sound[BGM_SOUND::Count_BGM];
	FMOD_SOUND*			g_UI_Sound[UI_SOUND::Count_UI];
	FMOD_SOUND*			g_Player_Sound[PLAYER_SOUND::Count_P];
	FMOD_SOUND*			g_Enemy_Sound[ENEMY_SOUND::Enemy_Count];
	FMOD_SOUND*			g_Boss_Sound[BOSS_SOUND::Boss_Count];

	FMOD_CHANNELGROUP*	g_BGM_ChannelGroup{ nullptr };
	FMOD_CHANNELGROUP*	g_UI_ChannelGroup{ nullptr };
	FMOD_CHANNELGROUP*	g_Effect_ChannelGroup{ nullptr };

	FMOD_CHANNEL*		g_BGM_Channel{ nullptr };
	FMOD_CHANNEL*		g_UI_Channel{ nullptr };
	FMOD_CHANNEL*		g_Player_Channel{ nullptr };
	FMOD_CHANNEL*		g_Enemy_Channel{ nullptr };
	FMOD_CHANNEL*		g_Boss_Channel{ nullptr };
public:
	CSoundManager();
	~CSoundManager();

	void Initialize();
	
	void CreateSound_BGM();
	void CreateSound_UI();
	void CreateSound_Player();
	void CreateSound_Enemy();
	void CreateSound_Boss();

	void Update(float fTimeElapsed);

	void PlaySnd_BGM(int);
	void PlaySnd_UI(int);
	void PlaySnd_Player(int);
	void PlaySnd_Enemy(int);
	void PlaySnd_Boss(int);

	void Destroy();
};

/*
FMOD::Sound        : 소리가 담긴 리소스
FMOD::Channel      : 소리는 하나의 채널에서 소리가 난다.
FMOD::ChannelGroup : 각각의 채널(소리)를 묶어서 제어할때 사용한다.
마스터 채널그룹이 이에 속한다.

채널그룹 사용방법
1. result = system->createChannelGroup("Group A", &groupA);
2. result = system->getMasterChannelGroup(&masterGroup);
3. result = masterGroup->addGroup(groupA);
4. result = system->createSound("Sleep Away.mp3", FMOD_LOOP_NORMAL, 0, &sound);
5. result = system->playSound(FMOD_CHANNEL_FREE, &sound, true, &channel);
6. result = channel->setChannelGroup(groupA);
1. system으로부터 채널그룹A생성
2. system으로부터 마스터 채널그룹 얻음
3. 마스터 채널그룹에 채널그룹A추가
4. sound음원생성
5. 재생하며 채널설정
6. 채널에 채널그룹A설정
7. 채널그룹A를 통하여 등록된 소리를 통제한다.
배경음, 효과음 각각 따로
- 효과음그룹 : 총소리, 발자국소리, 등등
- 배경음악그룹 : 각 배경음악들
- UI그룹 : UI를 사용할때 난다
- 마스터 채널 그룹 : 게임을 일시정지시

*/