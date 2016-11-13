#include "stdafx.h"
#include "SoundManager.h"

CSoundManager::CSoundManager()
{
}
CSoundManager::~CSoundManager()
{
}

void CSoundManager::Initialize()
{
	// 사운드 시스템을 생성하는 코드
	FMOD_System_Create(&g_System);
	// 채널수와 사운드 모드 설정
	FMOD_System_Init(g_System, Max_Channel, FMOD_INIT_NORMAL, NULL);             
	// 채널그룹 생성
	FMOD_System_CreateChannelGroup(g_System, "BGM", &g_BGM_ChannelGroup);
	FMOD_System_CreateChannelGroup(g_System, "UI", &g_UI_ChannelGroup);
	FMOD_System_CreateChannelGroup(g_System, "Effect", &g_Effect_ChannelGroup);
	// 채널그룹에 채널셋팅
	FMOD_Channel_SetChannelGroup(g_BGM_Channel, g_BGM_ChannelGroup);
	FMOD_Channel_SetChannelGroup(g_UI_Channel, g_UI_ChannelGroup);
	FMOD_Channel_SetChannelGroup(g_Enemy_Channel, g_Effect_ChannelGroup);
	FMOD_Channel_SetChannelGroup(g_Player_Channel, g_Effect_ChannelGroup);
	FMOD_Channel_SetChannelGroup(g_Boss_Channel, g_Effect_ChannelGroup);
	// 사운드 생성
	CreateSound_BGM();
	CreateSound_UI();
	CreateSound_Player();
	CreateSound_Enemy();
	CreateSound_Boss();
	// 볼륨 크기조절
	FMOD_Channel_SetVolume(g_Enemy_Channel, 0.2);
}

void CSoundManager::CreateSound_BGM() {

	FMOD_System_CreateSound(g_System, "Resource\\sound\\Logo.wav", FMOD_LOOP_NORMAL, 0, &g_BGM_Sound[BGM_SOUND::Logo_BGM]);
	FMOD_System_CreateSound(g_System, "Resource\\sound\\Title_BGM.mp3", FMOD_LOOP_NORMAL, 0, &g_BGM_Sound[BGM_SOUND::Title_BGM]);
	FMOD_System_CreateSound(g_System, "Resource\\sound\\Lobby_BGM.mp3", FMOD_LOOP_NORMAL, 0, &g_BGM_Sound[BGM_SOUND::Lobby_BGM]);
	FMOD_System_CreateSound(g_System, "Resource\\sound\\Stage_0-1_BGM.mp3", FMOD_LOOP_NORMAL, 0, &g_BGM_Sound[BGM_SOUND::Stage_BGM]);
	FMOD_System_CreateSound(g_System, "Resource\\sound\\BOSSStage_BGM.mp3", FMOD_LOOP_NORMAL, 0, &g_BGM_Sound[BGM_SOUND::Boss_Stage_BGM]);
	FMOD_System_CreateSound(g_System, "Resource\\sound\\Game_Over.mp3", FMOD_DEFAULT, 0, &g_BGM_Sound[BGM_SOUND::GameOver_BGM]);
	FMOD_System_CreateSound(g_System, "Resource\\sound\\Game_Clear.wav", FMOD_DEFAULT, 0, &g_BGM_Sound[BGM_SOUND::GameClear_BGM]);
}
void CSoundManager::CreateSound_UI() {

	FMOD_System_CreateSound(g_System, "Resource\\sound\\Lobby_Menu_Select.wav", FMOD_DEFAULT, 0, &g_UI_Sound[UI_SOUND::Lobby_Menu_Select_UI]);
	FMOD_System_CreateSound(g_System, "Resource\\sound\\Potion.wav", FMOD_DEFAULT, 0, &g_UI_Sound[UI_SOUND::Potion_UI]);
}
void CSoundManager::CreateSound_Player() {

	FMOD_System_CreateSound(g_System, "Resource\\sound\\Attack.wav", FMOD_DEFAULT, 0, &g_Player_Sound[PLAYER_SOUND::Attack_P]);
	FMOD_System_CreateSound(g_System, "Resource\\sound\\walk.wav", FMOD_DEFAULT, 0, &g_Player_Sound[PLAYER_SOUND::Walk_P]);
	FMOD_System_CreateSound(g_System, "Resource\\sound\\Reload.wav", FMOD_DEFAULT, 0, &g_Player_Sound[PLAYER_SOUND::Reload_P]);
	FMOD_System_CreateSound(g_System, "Resource\\sound\\Shot.wav", FMOD_DEFAULT, 0, &g_Player_Sound[PLAYER_SOUND::Shot_P]);
	FMOD_System_CreateSound(g_System, "Resource\\sound\\Die.wav", FMOD_DEFAULT, 0, &g_Player_Sound[PLAYER_SOUND::Die_P]);
	FMOD_System_CreateSound(g_System, "Resource\\sound\\Jump.mp3", FMOD_DEFAULT, 0, &g_Player_Sound[PLAYER_SOUND::Jump_P]);
	FMOD_System_CreateSound(g_System, "Resource\\sound\\Deadly.wav", FMOD_DEFAULT, 0, &g_Player_Sound[PLAYER_SOUND::Deadly_P]);
	FMOD_System_CreateSound(g_System, "Resource\\sound\\Hit.wav", FMOD_DEFAULT, 0, &g_Player_Sound[PLAYER_SOUND::Hit_P]);
	FMOD_System_CreateSound(g_System, "Resource\\sound\\Parrying.wav", FMOD_DEFAULT, 0, &g_Player_Sound[PLAYER_SOUND::Parrying_P]);
}
void CSoundManager::CreateSound_Enemy() {

	FMOD_System_CreateSound(g_System, "Resource\\sound\\Enemy_Attack.mp3", FMOD_DEFAULT, 0, &g_Enemy_Sound[ENEMY_SOUND::Enemy_Attack]);
	FMOD_System_CreateSound(g_System, "Resource\\sound\\Enemy_Shot.wav", FMOD_DEFAULT, 0, &g_Enemy_Sound[ENEMY_SOUND::Enemy_Shot]);
	FMOD_System_CreateSound(g_System, "Resource\\sound\\Enemy_Move.mp3", FMOD_DEFAULT, 0, &g_Enemy_Sound[ENEMY_SOUND::Enemy_Move]);
	FMOD_System_CreateSound(g_System, "Resource\\sound\\Enemy_Hit.mp3", FMOD_DEFAULT, 0, &g_Enemy_Sound[ENEMY_SOUND::Enemy_Hit]);
	FMOD_System_CreateSound(g_System, "Resource\\sound\\Enemy_Guard.mp3", FMOD_DEFAULT, 0, &g_Enemy_Sound[ENEMY_SOUND::Enemy_Guard]);
	FMOD_System_CreateSound(g_System, "Resource\\sound\\Enemy_Die.mp3", FMOD_DEFAULT, 0, &g_Enemy_Sound[ENEMY_SOUND::Enemy_Die]);
}
void CSoundManager::CreateSound_Boss() {

	FMOD_System_CreateSound(g_System, "Resource\\sound\\Boss_Phase_2_Shift.wav", FMOD_DEFAULT, 0, &g_Boss_Sound[BOSS_SOUND::Boss_Phase2_Shift]);
	FMOD_System_CreateSound(g_System, "Resource\\sound\\Boss_Phase_2_Attack.wav", FMOD_DEFAULT, 0, &g_Boss_Sound[BOSS_SOUND::Boss_Phase2_Attack]);
	FMOD_System_CreateSound(g_System, "Resource\\sound\\Boss_Phase_2_Boom.wav", FMOD_DEFAULT, 0, &g_Boss_Sound[BOSS_SOUND::Boss_Phase2_Boom]);
	FMOD_System_CreateSound(g_System, "Resource\\sound\\Boss_Phase_2_Summon.wav", FMOD_DEFAULT, 0, &g_Boss_Sound[BOSS_SOUND::Boss_Phase2_Summon]);
	FMOD_System_CreateSound(g_System, "Resource\\sound\\Boss_Phase_3_Shift.wav", FMOD_DEFAULT, 0, &g_Boss_Sound[BOSS_SOUND::Boss_Phase3_Shift]);
	FMOD_System_CreateSound(g_System, "Resource\\sound\\Boss_Phase_3_Idle.wav", FMOD_DEFAULT, 0, &g_Boss_Sound[BOSS_SOUND::Boss_Phase3_Idle]);
	FMOD_System_CreateSound(g_System, "Resource\\sound\\Boss_Die.wav", FMOD_DEFAULT, 0, &g_Boss_Sound[BOSS_SOUND::Boss_Die]);
}

void CSoundManager::Update(float fTimeElapsed)
{
	FMOD_System_Update(g_System);
}

void CSoundManager::PlaySnd_BGM(int set) {
	switch (set) {
	case BGM_SOUND::Logo_BGM:
		FMOD_System_PlaySound(g_System, g_BGM_Sound[BGM_SOUND::Logo_BGM], NULL, false, &g_BGM_Channel);
		break;
	case BGM_SOUND::Title_BGM:
		FMOD_System_PlaySound(g_System, g_BGM_Sound[BGM_SOUND::Title_BGM], NULL, false, &g_BGM_Channel);
		break;
	case BGM_SOUND::Lobby_BGM:
		FMOD_System_PlaySound(g_System, g_BGM_Sound[BGM_SOUND::Lobby_BGM], NULL, false, &g_BGM_Channel);
		break;
	case BGM_SOUND::Stage_BGM:
		FMOD_System_PlaySound(g_System, g_BGM_Sound[BGM_SOUND::Stage_BGM], NULL, false, &g_BGM_Channel);
		break;
	case BGM_SOUND::Boss_Stage_BGM:
		FMOD_System_PlaySound(g_System, g_BGM_Sound[BGM_SOUND::Boss_Stage_BGM], NULL, false, &g_BGM_Channel);
		break;
	case BGM_SOUND::GameOver_BGM:
		FMOD_System_PlaySound(g_System, g_BGM_Sound[BGM_SOUND::GameOver_BGM], NULL, false, &g_BGM_Channel);
		break;
	case BGM_SOUND::GameClear_BGM:
		FMOD_System_PlaySound(g_System, g_BGM_Sound[BGM_SOUND::GameClear_BGM], NULL, false, &g_BGM_Channel);
		break;
	case BGM_SOUND::Stop_BGM:
		FMOD_Channel_Stop(g_BGM_Channel);
		break;
	}
}
void CSoundManager::PlaySnd_UI(int set) {
	switch (set) {
	case UI_SOUND::Lobby_Menu_Select_UI:
		FMOD_System_PlaySound(g_System, g_UI_Sound[UI_SOUND::Lobby_Menu_Select_UI], NULL, false, &g_UI_Channel);
		break;
	case UI_SOUND::Potion_UI:
		FMOD_System_PlaySound(g_System, g_UI_Sound[UI_SOUND::Potion_UI], NULL, false, &g_UI_Channel);
		break;
	case UI_SOUND::Stop_UI:
		FMOD_Channel_Stop(g_UI_Channel);
		break;
	}
}
void CSoundManager::PlaySnd_Player(int set) {
	switch (set) {
	case PLAYER_SOUND::Attack_P:
		FMOD_System_PlaySound(g_System, g_Player_Sound[PLAYER_SOUND::Attack_P], NULL, false, &g_Player_Channel);
		break;
	case PLAYER_SOUND::Walk_P:
		FMOD_System_PlaySound(g_System, g_Player_Sound[PLAYER_SOUND::Walk_P], NULL, false, &g_Player_Channel);
		break;
	case PLAYER_SOUND::Reload_P:
		FMOD_System_PlaySound(g_System, g_Player_Sound[PLAYER_SOUND::Reload_P], NULL, false, &g_Player_Channel);
		break;
	case PLAYER_SOUND::Shot_P:
		FMOD_System_PlaySound(g_System, g_Player_Sound[PLAYER_SOUND::Shot_P], NULL, false, &g_Player_Channel);
		break;
	case PLAYER_SOUND::Deadly_P:
		FMOD_System_PlaySound(g_System, g_Player_Sound[PLAYER_SOUND::Deadly_P], NULL, false, &g_Player_Channel);
		break;
	case PLAYER_SOUND::Hit_P:
		FMOD_System_PlaySound(g_System, g_Player_Sound[PLAYER_SOUND::Hit_P], NULL, false, &g_Player_Channel);
		break;
	case PLAYER_SOUND::Parrying_P:
		FMOD_System_PlaySound(g_System, g_Player_Sound[PLAYER_SOUND::Parrying_P], NULL, false, &g_Player_Channel);
		break;
	case PLAYER_SOUND::Die_P:
		FMOD_System_PlaySound(g_System, g_Player_Sound[PLAYER_SOUND::Die_P], NULL, false, &g_Player_Channel);
		break;
	case PLAYER_SOUND::Jump_P:
		FMOD_System_PlaySound(g_System, g_Player_Sound[PLAYER_SOUND::Jump_P], NULL, false, &g_Player_Channel);
		break;
	case PLAYER_SOUND::Stop_P:
		FMOD_Channel_Stop(g_Player_Channel);
		break;
	}
}
void CSoundManager::PlaySnd_Enemy(int set) {
	switch (set) {
	case ENEMY_SOUND::Enemy_Attack:
		FMOD_System_PlaySound(g_System, g_Enemy_Sound[ENEMY_SOUND::Enemy_Attack], NULL, false, &g_Enemy_Channel);
		break;
	case ENEMY_SOUND::Enemy_Shot:
		FMOD_System_PlaySound(g_System, g_Enemy_Sound[ENEMY_SOUND::Enemy_Shot], NULL, false, &g_Enemy_Channel);
		break;
	case ENEMY_SOUND::Enemy_Move:
		FMOD_System_PlaySound(g_System, g_Enemy_Sound[ENEMY_SOUND::Enemy_Move], NULL, false, &g_Enemy_Channel);
		break;
	case ENEMY_SOUND::Enemy_Hit:
		FMOD_System_PlaySound(g_System, g_Enemy_Sound[ENEMY_SOUND::Enemy_Hit], NULL, false, &g_Enemy_Channel);
		break;
	case ENEMY_SOUND::Enemy_Guard:
		FMOD_System_PlaySound(g_System, g_Enemy_Sound[ENEMY_SOUND::Enemy_Guard], NULL, false, &g_Enemy_Channel);
		break;
	case ENEMY_SOUND::Enemy_Die:
		FMOD_System_PlaySound(g_System, g_Enemy_Sound[ENEMY_SOUND::Enemy_Die], NULL, false, &g_Enemy_Channel);
		break;
	case ENEMY_SOUND::Enemy_Stop:
		FMOD_Channel_Stop(g_Enemy_Channel);
		break;
	}
}
void CSoundManager::PlaySnd_Boss(int set) {
	switch (set) {
	case BOSS_SOUND::Boss_Phase2_Shift:
		FMOD_System_PlaySound(g_System, g_Boss_Sound[BOSS_SOUND::Boss_Phase2_Shift], NULL, false, &g_Boss_Channel);
		break;
	case BOSS_SOUND::Boss_Phase2_Attack:
		FMOD_System_PlaySound(g_System, g_Boss_Sound[BOSS_SOUND::Boss_Phase2_Attack], NULL, false, &g_Boss_Channel);
		break;
	case BOSS_SOUND::Boss_Phase2_Boom:
		FMOD_System_PlaySound(g_System, g_Boss_Sound[BOSS_SOUND::Boss_Phase2_Boom], NULL, false, &g_Boss_Channel);
		break;
	case BOSS_SOUND::Boss_Phase2_Summon:
		FMOD_System_PlaySound(g_System, g_Boss_Sound[BOSS_SOUND::Boss_Phase2_Summon], NULL, false, &g_Boss_Channel);
		break;
	case BOSS_SOUND::Boss_Phase3_Shift:
		FMOD_System_PlaySound(g_System, g_Boss_Sound[BOSS_SOUND::Boss_Phase3_Shift], NULL, false, &g_Boss_Channel);
		break;
	case BOSS_SOUND::Boss_Phase3_Idle :
		FMOD_System_PlaySound(g_System, g_Boss_Sound[BOSS_SOUND::Boss_Phase3_Idle], NULL, false, &g_Boss_Channel);
		break;
	case BOSS_SOUND::Boss_Die		  :
		FMOD_System_PlaySound(g_System, g_Boss_Sound[BOSS_SOUND::Boss_Die], NULL, false, &g_Boss_Channel);
		break;
	case BOSS_SOUND::Boss_Stop:
		FMOD_Channel_Stop(g_Boss_Channel);
		break;
	}
}

void CSoundManager::Destroy() {
	for (int i = 0; i < BGM_SOUND::Count_BGM; ++i) {
		FMOD_Sound_Release(g_BGM_Sound[i]);
	}
	for (int i = 0; i < UI_SOUND::Count_UI; ++i) {
		FMOD_Sound_Release(g_UI_Sound[i]);
	}
	for (int i = 0; i < PLAYER_SOUND::Count_P; ++i) {
		FMOD_Sound_Release(g_Player_Sound[i]);
	}
	for (int i = 0; i < ENEMY_SOUND::Enemy_Count; ++i) {
		FMOD_Sound_Release(g_Enemy_Sound[i]);
	}
	for (int i = 0; i < BOSS_SOUND::Boss_Count; ++i) {
		FMOD_Sound_Release(g_Boss_Sound[i]);
	}
	FMOD_System_Close(g_System);
	FMOD_System_Release(g_System);
}