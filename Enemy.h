#pragma once
#include "Unit.h"
#include "Player.h"

#define ENEMY_HP_SWORDMAN 50
#define ENEMY_ATK_SWORDMAN 7
#define ENEMY_DEF_SWORDMAN 0
#define ENEMY_SPEED_SWORDMAN 100
#define ENEMY_GUARD_DEF_SWORDMAN 2

#define ENEMY_HP_ARCHER 30
#define ENEMY_ATK_ARCHER 5
#define ENEMY_DEF_ARCHER 0
#define ENEMY_SPEED_ARCHER 100
#define ENEMY_ARROW_SPEED_ARCHER 500
#define ENEMY_SHOT_RANGE 800
#define ENEMY_SHOT_CYCLE 5

#define ENEMY_PUSHED_SPEED 50
#define ENEMY_SEARCH_RANGE 1000
#define ENEMY_MOVE_SOUND_CYCLE 0.5

#define GetLength(x1, y1, x2, y2) sqrt( ( x2 - x1 )*( x2 - x1 ) + ( y2- y1 )*( y2 - y1 ) )

class CPlayer;

class CEnemy :public CUnit
{
public:
	enum class TypeOfEnemy {
		SwordMan = 0
		, Archer
		, Count
	};
protected:
	// 공통변수
	TypeOfEnemy m_Type;

	CPlayer *m_Tageted_Player{ nullptr };

	POINT m_ScenePos{ 0, 0 };

	RECT m_HP_Bar{ 0, 0, 0, 0 };

	// SwordMan
	CEnemyAnimationSprite m_sprite;

	bool m_guard{ false };

	// Archer
	CEnemyArcher_AnimationSprite m_sprite_A;
	CImage m_Arrow;

	COLORREF m_colTransparent;

	RECT m_ArrowBox{ 0, 0, 0, 0 };

	POINT m_ArrowStartPos{ 0, 0 };
	
	float m_fCurrentShot{ 0.0f };
	float m_fArrowMove{ 0.0f };

	bool m_shot{ false };
	bool m_bLeft_Arrow{ false };

public:
	CEnemy(LPCTSTR path, CSoundManager *sm, int w, int h, TypeOfEnemy k, float fFrequency = 1.0f / 60.0f);
	virtual ~CEnemy();

	virtual void OnDraw(HDC hDC);
	virtual void OnUpdate(float fTimeElapsed, POINT ScenePos);
	virtual void Tageting(CPlayer *&Player) { m_Tageted_Player = Player; }

	virtual void SetState(CEnemyAnimationSprite::ESpriteState state, int val) { m_sprite.SetState(state, val); }
	virtual void SetState(CEnemyArcher_AnimationSprite::ESpriteState state, int val) { m_sprite_A.SetState(state, val); }
	virtual void SetState(CEnemyAnimationSprite::ESpriteState state, int val, int excute_start, int excute_end, int exit_start, int exit_end) { m_sprite.SetState(state, val, excute_start, excute_end, exit_start, exit_end); }
	virtual void SetState(CEnemyArcher_AnimationSprite::ESpriteState state, int val, int excute_start, int excute_end, int exit_start, int exit_end) { m_sprite_A.SetState(state, val, excute_start, excute_end, exit_start, exit_end); }
	virtual void SetStateDelay(CEnemyAnimationSprite::ESpriteState state, int excute, int action, int exit) { m_sprite.SetStateDelay(state, excute, action, exit); }
	virtual void SetStateDelay(CEnemyArcher_AnimationSprite::ESpriteState state, int excute, int action, int exit) { m_sprite_A.SetStateDelay(state, excute, action, exit); }
	virtual void SetStateDelay(CEnemyAnimationSprite::ESpriteState state, int action) { m_sprite.SetStateDelay(state, 0, action, 0); }
	virtual void SetStateDelay(CEnemyArcher_AnimationSprite::ESpriteState state, int action) { m_sprite_A.SetStateDelay(state, 0, action, 0); }

	virtual void ReSet();

	TypeOfEnemy GetType() { return m_Type; }

	void SetPos(POINT Pos) { m_ptCurrent = Pos; }

	void Update_Interaction(float fTimeElapsed);

	void Attack();
	void Guard();
	void Shot();
	void Hit();

	bool CrashCheck();
	
protected:

	virtual void MoveToTaget(float fTimeElapsed);
};

