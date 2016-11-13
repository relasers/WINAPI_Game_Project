#pragma once
#include "Unit.h"
#include "Enemy.h"
#include "Player.h"

#define BOSS_HP 110
#define BOSS_ATK 15
#define BOSS_DEF 2
#define BOSS_SPEED 300
#define BOSS_ACTIVE_CYCLE 3
#define BOSS_SUMMON_CYCLE 1.5
#define BOSS_SEARCH_RANGE 1500
#define BOSS_DASH_SPEED 12
#define BOSS_CAMERA_SHAKE 10

class CPlayer;
class CEnemy;
template<typename T>
class WarpList;

class CBoss :public CUnit
{
public:
	enum class BossPhase : short
	{
		Phase1 = 1
		, Phase2
		, Phase3
	};
protected:

	CBOSSAnimationSprite m_sprite;
	CSpriteImage	*m_cimgHP_Bar;

	CPlayer *m_Tageted_Player{ nullptr };
	WarpList<CEnemy*>*m_EnemyList{ nullptr };

	BossPhase m_phase{ BossPhase::Phase1 };

	POINT* m_ScenePos{ nullptr };

	RECT m_HP_Bar{ 0, 0, 0, 0 };
	RECT m_MagicBox{ 0, 0, 0, 0 };

	float m_fCurrentActive{ 0 };
public:
	CBoss(LPCTSTR path, CSoundManager *sm, int w, int h, float fFrequency = 1.0f / 60.0f);
	virtual ~CBoss();

	virtual void OnDraw(HDC hDC);
	virtual void OnUpdate(float fTimeElapsed, POINT* ScenePos);
	virtual void Tageting(WarpList<CEnemy*> &Enemy_List, CPlayer *&Player) { m_EnemyList = &Enemy_List; m_Tageted_Player = Player; }

	virtual void SetState(CBOSSAnimationSprite::SpriteState state, int val) { m_sprite.SetState(state, val); }
	virtual void SetState(CBOSSAnimationSprite::SpriteState state, int val, int excute_start, int excute_end, int exit_start, int exit_end) { m_sprite.SetState(state, val, excute_start, excute_end, exit_start, exit_end); }
	virtual void SetStateDelay(CBOSSAnimationSprite::SpriteState state, int excute, int action, int exit) { m_sprite.SetStateDelay(state, excute, action, exit); }
	virtual void SetStateDelay(CBOSSAnimationSprite::SpriteState state, int action) { m_sprite.SetStateDelay(state, 0, action, 0); }

	virtual void ReSet();

	void SetPos(POINT Pos) { m_ptCurrent = Pos; }

	void Update_Interaction(float fTimeElapsed);
	
	void Summon();
	void Summon_Interaction(float fTimeElapsed);
	void Attack();
	void Attack_Interaction();
	void Dash();
	void Dash_Interaction(float fTimeElapsed);
	void Idle_Interaction(float fTimeElapsed);
	void Hit();

	bool CrashCheck();
};

