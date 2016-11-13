#include "stdafx.h"
#include "Unit.h"

CUnit::CUnit(CSoundManager *sm , float fFrequency)
{
	m_pSound = sm;
	// 1회 호출 당 나가는 속력
	m_fSpeed = 4.0f;
	
	m_fFrequency = fFrequency;
	m_fTick = 0.f;
}
CUnit::~CUnit()
{
}

void CUnit::OnDraw(HDC hDC)
{
}

void CUnit::OnUpdate(float fTimeElapsed)
{
	m_fTick += fTimeElapsed;
	if (m_fTick < m_fFrequency) return;
	m_fTick -= m_fFrequency;

}

void CUnit::MoveToTaget(float fTimeElapsed)
{
	// move 상태로 변경
	//m_sprite.UpdateState(CAnimationSprite::SpriteState::sprite_state_move);
	// 진입 상태일 경우에는 정지(준비 자세이기 때문)
	//if (m_sprite.GetCurrentStateState() == CAnimationSprite::StateState::state_state_excute) return;

	// 이동
	//POINT Enemy_Pos = m_ClosedEnemy->Get_UnitStatus_Pos();

	if (true)// 타겟과의 거리
	{
		m_fMoveLength += m_fSpeed * fTimeElapsed;

		if (m_fMoveLength >= 1)
		{
			m_ptCurrent.x++;
			m_fMoveLength = 0;
		}
		if (m_fMoveLength <= -1)
		{
			m_ptCurrent.x--;
			m_fMoveLength = 0;
		}
	}
	if (m_ptCurrent.x < 0) m_ptCurrent.x = 0;
	if (m_ptCurrent.y < 0) m_ptCurrent.y = 0;
}


