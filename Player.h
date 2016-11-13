#pragma once
#include "Unit.h"
#include "Enemy.h"
#include "Boss.h"

#define PLAYER_ATTACK_CYCLE 0.25f
#define PLAYER_HP 100
#define PLAYER_ATK 5
#define PLAYER_DEF 2
#define PLAYER_SPEED 300
#define PLAYER_START_POTION 3
#define PLAYER_POTION_SIZE 30
#define PLAYER_START_BULLET 5
#define PLAYER_GUARD_DEF 10
#define PLAYER_SHOT_RANGE 2000
#define PLAYER_DASH_FORCE 5
#define PLAYER_KNOCKBACK_FORCE 100
#define PLAYER_MAXJUMP 2000.0f
// 각 공격 데미지 상수 정의
#define PLAYER_SHOT_DAMAGE 10
#define PLAYER_PARRYING_DAMAGE 3

class CBoss;
class CEnemy;
template<typename T>
class WarpList;

class CPlayer :public CUnit
{
public:
	enum class InputKey : short
	{
		KeyUp
		, MoveLeft
		, MoveRight
		, Jump
		, Z // parrying
		, X // 공격
		, C // 방어
		, V // shot
		, A // 포션
	};

	enum class KindOfAttack {
		Normal_Attack = 0
		, Guard_Attack
		, Jump_Attack
		, Parrying_Attack
		, Shot_Attack
	};

protected:
	// Charactor Sprite
	CAnimationSprite m_sprite;

	// UI Image
	CSpriteImage	*m_cimgHP_Bar{ nullptr };
	CSpriteImage	*m_cimgBulletNum{ nullptr };
	CSpriteImage	*m_cimgPotionNum{ nullptr };
	CSpriteImage	*m_cimgShot{ nullptr };

	CImage m_cimgEnemyCount;
	COLORREF m_colTransparent;
	// UI Box
	RECT m_HP_Bar{ 0, 0, 0, 0 };
	RECT m_Bullet_Num{ 0, 0, 0, 0 };
	RECT m_Potion_Num{ 0, 0, 0, 0 };

	RECT m_GuardAttackBox{ 0, 0, 0, 0 };
	RECT m_ParryingBox{ 0, 0, 0, 0 };
	RECT m_JumpAttackBox{ 0, 0, 0, 0 };
	RECT m_ShotBox{ 0, 0, 0, 0 };

	// Taget
	WarpList<CEnemy*>*m_EnemyList{ nullptr };
	CBoss			*m_Boss{ nullptr };

	// State flag
	int m_AttackCombo{ 0 };
	bool m_JumpAttack{ false };
	bool m_Move{ false };
	bool m_Dash{ false };
	bool m_Guard{ false };
	bool m_bPushShift{ false };

	// Parameter
	int m_NumOfBullet{ 0 };
	int m_NumOfPotion{ 0 };

	int m_EnemyCount{ 0 };

public:
	CPlayer(LPCTSTR path, CSoundManager *sm, int w, int h, float fFrequency = 1.0f / 60.0f);
	virtual ~CPlayer();

	virtual void Input(InputKey input, bool bPushLShift = false);
	virtual void OnDraw(HDC hDC);
	virtual void OnUpdate(float fTimeElapsed);
	virtual void Tageting(WarpList<CEnemy*> &Enemy_List, CBoss *Boss = nullptr) { m_EnemyList = &Enemy_List; m_Boss = Boss; }

	virtual void SetState(CAnimationSprite::SpriteState state, int val) { m_sprite.SetState(state, val); }
	virtual void SetState(CAnimationSprite::SpriteState state, int val, int excute_start, int excute_end, int exit_start, int exit_end) { m_sprite.SetState(state, val, excute_start, excute_end, exit_start, exit_end); }
	virtual void SetStateDelay(CAnimationSprite::SpriteState state, int excute, int action, int exit) { m_sprite.SetStateDelay(state, excute, action, exit); }
	virtual void SetStateDelay(CAnimationSprite::SpriteState state, int action) { m_sprite.SetStateDelay(state, 0, action, 0); }
	
	virtual void ReSet();

	void SetPos(POINT Pos) { m_ptCurrent = Pos; }

	void Update_Interaction(float fTimeElapsed);

	void Attack();
	void Attack_Interaction();
	void JumpAttack();
	void JumpAttack_Interaction();
	void GuardAttack();
	void GuardAttack_Interaction();
	void Parrying();
	void Parrying_Interaction();
	void Shot();
	void Shot_Interaction();

	void Dash();
	void Dash_Interaction(float fTimeElapsed);

	bool CrashCheck(CEnemy *enemy, KindOfAttack KA);
	bool CrashCheck(CBoss *boss, KindOfAttack KA);

	void Hit();
	void Hit_Interaction(float fTimeElapsed);
protected:

	void Move(float fTimeElapsed);
	// 초기값은 기본 점프 높이
	void Jump(float fTimeElapsed, float maxJump = PLAYER_MAXJUMP);
};

