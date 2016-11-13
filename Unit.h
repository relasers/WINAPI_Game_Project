#pragma once
#include <Windows.h>
#include "SpriteImage.h"
#include "SoundManager.h"
#include "Vec2f.h"
#include "WarpList.h"

class CUnit
{
protected:

	CSoundManager		*m_pSound;

	POINT				m_ptCurrent{0, 0}; // 현재위치
	float				m_fCurrentUpdate{ 0 }; // 업뎃카운터
	float				m_fCurrentAttack{ 0 }; // 공격카운터
	float				m_fCurrentJump{ 0 }; // 점프카운터
	float				m_fCurrentHit{ 0 }; // 피격카운터
	float				m_fCurrentDie{ 0 }; // 죽은 후 삭제까지의 카운터

	float				m_fMoveLength{ 0 }; // 누적이동거리 < 1
	float				m_Gravity_AC{ 0 }; // 누적가속도

	bool				m_Togle{ false }; // 판정박스 on / off
	bool				m_Die{ false };
	// 유닛 상태판정 박스
	RECT m_bodybox{ 0, 0, 0, 0 };
	RECT m_atkbox{ 0, 0, 0, 0 };

	// 유닛 상태 플래그
	bool m_Attack{ false };
	bool m_bJump{ false };
	bool m_bLeft{ false };
	bool m_Sound{ false };

	// 유닛 공통 스테이터스
	int m_MaxHP{ 0 };
	int m_prev_HP{ 0 };
	int m_HP{ 0 };
	int m_ATK{ 0 };
	int m_DEF{ 0 };
	float m_fSpeed{ 0 };

	// 유닛 갱신 단위(millisec)
	float m_fFrequency = 1.0f / 60.0f;
	float m_fTick = 0.f;
public:

	CUnit(CSoundManager *sm, float fFrequency = 1.0f / 60.0f);
	virtual ~CUnit();

	virtual void OnDraw(HDC hDC);
	virtual void OnUpdate(float fTimeElapsed);
	virtual void Tageting() {}

	virtual void ReSet() = 0;

	void SetTogle(bool togle = false) { m_Togle = togle; }

	// 유닛 스테이터스 반환
	const int GetMaxHP() const { return m_MaxHP; }
	const int GetHP() const { return m_HP; }
	int& GetHP() { return m_HP; }
	const int GetATK() const { return m_ATK; }
	const int GetDEF() const { return m_DEF; }

	// 유닛 위치정보 반환
	const POINT GetPos() const { return m_ptCurrent; }
	RECT GetBodybox() const { return m_bodybox; }
	RECT GetAtkbox() const { return m_atkbox; }

	const bool IsDie() const { return m_Die; }
protected:

	virtual void MoveToTaget(float fTimeElapsed);
};


