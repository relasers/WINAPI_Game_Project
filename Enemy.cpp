#include "stdafx.h"
#include "Enemy.h"

CEnemy::CEnemy(LPCTSTR path, CSoundManager *sm, int w, int h, TypeOfEnemy k, float fFrequency)
	: CUnit(sm, fFrequency), m_sprite(path, w, h), m_sprite_A(path, w, h)
{
	m_Arrow.Load(TEXT("Resource\\Graphics\\Sprite\\화살.png"));
	m_colTransparent = ::GetPixel(m_Arrow.GetDC(), 0, 0);

	m_Type = k;
	switch (m_Type)
	{
	case TypeOfEnemy::SwordMan:
		m_MaxHP = ENEMY_HP_SWORDMAN;
		m_prev_HP = ENEMY_HP_SWORDMAN;
		m_HP = ENEMY_HP_SWORDMAN;
		m_ATK = ENEMY_ATK_SWORDMAN;
		m_DEF = ENEMY_DEF_SWORDMAN;
		m_fSpeed = ENEMY_SPEED_SWORDMAN;
		break;
	case TypeOfEnemy::Archer:
		m_MaxHP = ENEMY_HP_ARCHER;
		m_prev_HP = ENEMY_HP_ARCHER;
		m_HP = ENEMY_HP_ARCHER;
		m_ATK = ENEMY_ATK_ARCHER;
		m_DEF = ENEMY_DEF_ARCHER;
		m_fSpeed = ENEMY_SPEED_ARCHER;
		break;
	}
	m_HP_Bar = {0, 0, 128, 5};
}
CEnemy::~CEnemy()
{
	m_Arrow.Destroy();
	m_sprite.~CEnemyAnimationSprite();
	m_sprite_A.~CEnemyArcher_AnimationSprite();
}

void CEnemy::OnDraw(HDC hDC)
{
	if (m_Die) return;
	switch (m_Type)
	{
	case TypeOfEnemy::SwordMan:
		if (!m_Die) m_sprite.Draw(hDC, m_ptCurrent.x - m_ScenePos.x, m_ptCurrent.y - m_ScenePos.y + 500);
		break;
	case TypeOfEnemy::Archer:
		if (!m_Die) {
			m_sprite_A.Draw(hDC, m_ptCurrent.x - m_ScenePos.x, m_ptCurrent.y - m_ScenePos.y + 500);
			if(m_shot) 
				m_Arrow.TransparentBlt(hDC, m_ArrowBox.left, m_ArrowBox.top,
				m_ArrowBox.right - m_ArrowBox.left, m_ArrowBox.bottom - m_ArrowBox.top, 
				(m_Arrow.GetWidth() / 2) * m_bLeft_Arrow, 0, m_Arrow.GetWidth() / 2, m_Arrow.GetHeight(), m_colTransparent);
		}
		break;
	}
	if (m_Togle)
	{
		FrameRect(hDC, &m_bodybox, (HBRUSH)GetStockObject(WHITE_BRUSH));
		if (m_Attack) FrameRect(hDC, &m_atkbox, (HBRUSH)GetStockObject(WHITE_BRUSH));
	}
	Rectangle(hDC, m_HP_Bar.left, m_HP_Bar.top, m_HP_Bar.left + m_HP, m_HP_Bar.bottom);
}

void CEnemy::OnUpdate(float fTimeElapsed, POINT ScenePos)
{
	if (m_Die) return;
	m_fTick += fTimeElapsed;
	if (m_fTick < m_fFrequency) return;
	m_fTick -= m_fFrequency;

	m_fCurrentUpdate += fTimeElapsed;

	m_ScenePos = ScenePos;

	// 스프라이트 갱신8
	switch (m_Type)
	{
	case TypeOfEnemy::SwordMan:
		m_sprite.OnUpdate();
		break;
	case TypeOfEnemy::Archer:
		m_sprite_A.OnUpdate();
		break;
	}

	Hit();

	switch (m_Type)
	{
	case TypeOfEnemy::SwordMan:
		if (m_sprite.GetCurrentState() == CEnemyAnimationSprite::ESpriteState::sprite_state_hit && m_HP <= 0)
			m_fCurrentDie += fTimeElapsed;
		if (m_fCurrentDie > 0.5 && m_sprite.GetCurrentState() != CEnemyAnimationSprite::ESpriteState::sprite_state_hit)
			m_Die = true;

		if (!m_Die)
		{
			if (m_Tageted_Player->GetPos().x - (m_ptCurrent.x - m_ScenePos.x) < 80 &&
				m_Tageted_Player->GetPos().x - (m_ptCurrent.x - m_ScenePos.x) > -80)
				Attack();
			else if (m_sprite.GetCurrentState() == CEnemyAnimationSprite::ESpriteState::sprite_state_idle ||
				m_sprite.GetCurrentState() == CEnemyAnimationSprite::ESpriteState::sprite_state_move)
				MoveToTaget(fTimeElapsed);
			
			Update_Interaction(fTimeElapsed);
		}
		break;
	case TypeOfEnemy::Archer:
		if (m_sprite_A.GetCurrentState() == CEnemyArcher_AnimationSprite::ESpriteState::sprite_state_hit && m_HP <= 0)
			m_fCurrentDie += fTimeElapsed;
		if (m_fCurrentDie > 0.5 && m_sprite.GetCurrentState() != CEnemyArcher_AnimationSprite::ESpriteState::sprite_state_hit)
			m_Die = true;

		if (!m_Die)
		{
			m_fCurrentShot += fTimeElapsed;

			if (m_Tageted_Player->GetPos().x - (m_ptCurrent.x - m_ScenePos.x) < 80 &&
				m_Tageted_Player->GetPos().x - (m_ptCurrent.x - m_ScenePos.x) > -80)
				Attack();
			else if (m_Tageted_Player->GetPos().x - (m_ptCurrent.x - m_ScenePos.x) < ENEMY_SHOT_RANGE &&
				m_Tageted_Player->GetPos().x - (m_ptCurrent.x - m_ScenePos.x) > -ENEMY_SHOT_RANGE &&
				m_fCurrentShot > ENEMY_SHOT_CYCLE)
			{
				Shot();
				m_fCurrentShot = 0;
			}
			else if (m_sprite.GetCurrentState() == CEnemyArcher_AnimationSprite::ESpriteState::sprite_state_idle ||
				m_sprite.GetCurrentState() == CEnemyArcher_AnimationSprite::ESpriteState::sprite_state_move)
				MoveToTaget(fTimeElapsed);

			Update_Interaction(fTimeElapsed);
		}
		break;
	}
}

void CEnemy::ReSet()
{
	switch (m_Type)
	{
	case TypeOfEnemy::SwordMan:
		m_MaxHP = ENEMY_HP_SWORDMAN;
		m_prev_HP = ENEMY_HP_SWORDMAN;
		m_HP = ENEMY_HP_SWORDMAN;
		m_ATK = ENEMY_ATK_SWORDMAN;
		m_DEF = ENEMY_DEF_SWORDMAN;
		m_fSpeed = ENEMY_SPEED_SWORDMAN;
		break;
	case TypeOfEnemy::Archer:
		m_MaxHP = ENEMY_HP_ARCHER;
		m_prev_HP = ENEMY_HP_ARCHER;
		m_HP = ENEMY_HP_ARCHER;
		m_ATK = ENEMY_ATK_ARCHER;
		m_DEF = ENEMY_DEF_ARCHER;
		m_fSpeed = ENEMY_SPEED_ARCHER;
		break;
	}
	m_HP_Bar = { 0, 0, 128, 5 };

	m_fCurrentDie = 0;
	m_Die = false;

	switch (m_Type)
	{
	case TypeOfEnemy::SwordMan:
		m_sprite.OnUpdate(true);
		break;
	case TypeOfEnemy::Archer:
		m_sprite_A.OnUpdate(true);
		break;
	}
}

void CEnemy::Update_Interaction(float fTimeElapsed)
{
	m_bodybox = { 128 / 3 + m_ptCurrent.x - m_ScenePos.x, 48 + m_ptCurrent.y - m_ScenePos.y + 500,
		256 / 3 + m_ptCurrent.x - m_ScenePos.x, 128 + m_ptCurrent.y - m_ScenePos.y + 500 };
	m_HP_Bar = { 128 / 3 + m_ptCurrent.x - m_ScenePos.x, m_ptCurrent.y - m_ScenePos.y + 543 ,
		256 / 3 + m_ptCurrent.x - m_ScenePos.x, 48 + m_ptCurrent.y - m_ScenePos.y + 500 };

	POINT m_PlayerPos = m_Tageted_Player->GetPos();

	if (m_PlayerPos.x + 64 > m_ptCurrent.x - m_ScenePos.x + 64)
	{
		m_bLeft = false;
		m_sprite_A.SetDirection(m_bLeft);
	}
	else if (m_PlayerPos.x + 64 < m_ptCurrent.x - m_ScenePos.x + 64)
	{
		m_bLeft = true;
		m_sprite_A.SetDirection(m_bLeft);
	}

	switch (m_Type)
	{
	case TypeOfEnemy::SwordMan:
		if (m_sprite.GetCurrentStateState() == CEnemyAnimationSprite::StateState::state_state_action &&
			m_sprite.GetCurrentState() == CEnemyAnimationSprite::ESpriteState::sprite_state_hit && m_HP > 0)
		{
			m_sprite.UpdateState(CEnemyAnimationSprite::ESpriteState::sprite_state_idle);
		}
		if (m_sprite.GetCurrentStateState() == CEnemyAnimationSprite::StateState::state_state_action &&
			m_sprite.GetCurrentState() == CEnemyAnimationSprite::ESpriteState::sprite_state_attack)
		{
			if (m_bLeft)
			{
				m_atkbox = { m_ptCurrent.x - m_ScenePos.x, m_ptCurrent.y - m_ScenePos.y + 530 ,
					m_ptCurrent.x - m_ScenePos.x + 256 / 3 , m_ptCurrent.y - m_ScenePos.y + 610 };
			}
			else if (!m_bLeft)
			{
				m_atkbox = { m_ptCurrent.x - m_ScenePos.x + 128 / 3 , m_ptCurrent.y - m_ScenePos.y + 530 ,
					m_ptCurrent.x - m_ScenePos.x + 128 , m_ptCurrent.y - m_ScenePos.y + 610 };
			}

			if (CrashCheck() && !m_Attack)
			{
				int Damege{ 0 };
				m_Attack = true;
				Damege = m_ATK - m_Tageted_Player->GetDEF();
				if (Damege < 0) Damege = 0;
				m_Tageted_Player->GetHP() -= Damege;
			}
		}
		else
		{
			m_atkbox = { 0, 0, 0, 0 };
			m_Attack = false;
		}

		if (m_sprite.GetCurrentState() == CEnemyAnimationSprite::ESpriteState::sprite_state_guard)
		{
			m_Gravity_AC += GRAVITY_FRAME_COEF;
			m_fMoveLength += (ENEMY_PUSHED_SPEED - m_Gravity_AC) * fTimeElapsed;
			if (m_fMoveLength >= 1)
			{
				if (m_bLeft)
				{
					m_ptCurrent.x += m_fMoveLength;
					m_fMoveLength = 0;
				}
				else if (!m_bLeft)
				{
					m_ptCurrent.x -= m_fMoveLength;
					m_fMoveLength = 0;
				}
			}
		}
		else
		{
			if (m_guard)
			{
				m_DEF -= ENEMY_GUARD_DEF_SWORDMAN;
				m_guard = false;
			}
			m_Gravity_AC = 0.0;
			m_fMoveLength = 0.0;
		}
		break;
	case TypeOfEnemy::Archer:
		if (m_sprite_A.GetCurrentStateState() == CEnemyArcher_AnimationSprite::StateState::state_state_action &&
			m_sprite_A.GetCurrentState() == CEnemyArcher_AnimationSprite::ESpriteState::sprite_state_hit && m_HP > 0)
		{
			m_sprite_A.UpdateState(CEnemyArcher_AnimationSprite::ESpriteState::sprite_state_idle);
		}
		if (m_sprite_A.GetCurrentStateState() == CEnemyArcher_AnimationSprite::StateState::state_state_action &&
			m_sprite_A.GetCurrentState() == CEnemyArcher_AnimationSprite::ESpriteState::sprite_state_attack)
		{
			if (m_bLeft)
			{
				m_atkbox = { m_ptCurrent.x - m_ScenePos.x, m_ptCurrent.y - m_ScenePos.y + 570 ,
					m_ptCurrent.x - m_ScenePos.x + 256 / 3 , m_ptCurrent.y - m_ScenePos.y + 580 };
			}
			else if (!m_bLeft)
			{
				m_atkbox = { m_ptCurrent.x - m_ScenePos.x + 128 / 3 , m_ptCurrent.y - m_ScenePos.y + 570 ,
					m_ptCurrent.x - m_ScenePos.x + 128 , m_ptCurrent.y - m_ScenePos.y + 580 };
			}

			if (CrashCheck() && !m_Attack)
			{
				int Damege{ 0 };
				m_Attack = true;
				Damege = m_ATK - m_Tageted_Player->GetDEF();
				if (Damege < 0) Damege = 0;
				m_Tageted_Player->GetHP() -= Damege;
			}
		}
		else
		{
			m_atkbox = { 0, 0, 0, 0 };
			m_Attack = false;
		}

		if (m_sprite_A.GetCurrentState() == CEnemyArcher_AnimationSprite::ESpriteState::sprite_state_shot && 
			m_sprite_A.GetCurrentStateState() == CEnemyArcher_AnimationSprite::StateState::state_state_action )
		{
			m_fArrowMove = 0.0f;
			
			POINT m_PlayerPos = m_Tageted_Player->GetPos();
			
			m_ArrowStartPos.x = m_ptCurrent.x;
			m_ArrowStartPos.y = m_ptCurrent.y;

			if(m_PlayerPos.x - m_ptCurrent.x + m_ScenePos.x > 0) m_bLeft_Arrow = false;
			else m_bLeft_Arrow = true;

			if (!m_Sound) m_pSound->PlaySnd_Enemy(CSoundManager::ENEMY_SOUND::Enemy_Shot);
			m_shot = true;
			m_Sound = true;
		}
		else m_Sound = false;
		if (m_shot)
		{
			if(!m_bLeft_Arrow) m_fArrowMove += ENEMY_ARROW_SPEED_ARCHER * fTimeElapsed;
			else m_fArrowMove -= ENEMY_ARROW_SPEED_ARCHER * fTimeElapsed;

			m_ArrowBox = 
			{ 158 / 3 + m_ArrowStartPos.x - m_ScenePos.x + (LONG)m_fArrowMove,
				m_ArrowStartPos.y - m_ScenePos.y + 568,
				226 / 3 + m_ArrowStartPos.x - m_ScenePos.x + (LONG)m_fArrowMove,
				m_ArrowStartPos.y - m_ScenePos.y + 571 };

			if (CrashCheck())
			{
				int Damege{ 0 };
				Damege = m_ATK - m_Tageted_Player->GetDEF();
				if (Damege < 0) Damege = 0;
				m_Tageted_Player->GetHP() -= Damege;
				m_shot = false;
			}
		}
		else if(!m_shot)
		{
			m_ArrowBox = { 0, 0, 0, 0 };
			m_fArrowMove = 0;
		}
		break;
	}
}

void CEnemy::Attack()
{
	switch (m_Type)
	{
	case TypeOfEnemy::SwordMan:
		if (m_sprite.GetCurrentState() != CEnemyAnimationSprite::ESpriteState::sprite_state_idle) return;
		m_sprite.UpdateState(CEnemyAnimationSprite::ESpriteState::sprite_state_attack);
		m_pSound->PlaySnd_Enemy(CSoundManager::ENEMY_SOUND::Enemy_Attack);
		break;
	case TypeOfEnemy::Archer:
		if (m_sprite_A.GetCurrentState() != CEnemyArcher_AnimationSprite::ESpriteState::sprite_state_idle) return;
		m_sprite_A.UpdateState(CEnemyArcher_AnimationSprite::ESpriteState::sprite_state_attack);
		m_pSound->PlaySnd_Enemy(CSoundManager::ENEMY_SOUND::Enemy_Attack);
		break;
	}
}

void CEnemy::Guard()
{
	m_sprite.UpdateState(CEnemyAnimationSprite::ESpriteState::sprite_state_guard);
	m_pSound->PlaySnd_Enemy(CSoundManager::ENEMY_SOUND::Enemy_Guard);
	m_Gravity_AC = 0.0;
	if (!m_guard)
	{
		m_DEF += ENEMY_GUARD_DEF_SWORDMAN;
		m_guard = true;
	}
}

void CEnemy::Shot()
{
	if (m_sprite_A.GetCurrentState() == CEnemyArcher_AnimationSprite::ESpriteState::sprite_state_idle) 
		m_sprite_A.UpdateState(CEnemyArcher_AnimationSprite::ESpriteState::sprite_state_shot);
}

void CEnemy::Hit()
{
	switch (m_Type)
	{
	case TypeOfEnemy::SwordMan:
		if (m_HP != m_prev_HP)
		{
			if (m_sprite.GetCurrentState() != CEnemyAnimationSprite::ESpriteState::sprite_state_guard)
			{
				m_prev_HP = m_HP;

				if (m_HP > 0)
				{
					if(rand()%2) Guard();
					else
					{
						m_sprite.UpdateState(CEnemyAnimationSprite::ESpriteState::sprite_state_hit);
						m_pSound->PlaySnd_Enemy(CSoundManager::ENEMY_SOUND::Enemy_Hit);
					}
				}
				else
				{
					m_HP = m_prev_HP = 0;
					m_sprite.UpdateState(CEnemyAnimationSprite::ESpriteState::sprite_state_hit);
					m_pSound->PlaySnd_Enemy(CSoundManager::ENEMY_SOUND::Enemy_Die);
				}
			}
			else if (m_sprite.GetCurrentState() == CEnemyAnimationSprite::ESpriteState::sprite_state_guard)
			{
				m_HP = m_prev_HP;
			}
		}
		break;
	case TypeOfEnemy::Archer:
		if (m_HP != m_prev_HP)
		{
			m_prev_HP = m_HP;

			if (m_HP > 0)
			{
				m_sprite_A.UpdateState(CEnemyArcher_AnimationSprite::ESpriteState::sprite_state_hit);
				m_pSound->PlaySnd_Enemy(CSoundManager::ENEMY_SOUND::Enemy_Hit);
			}
			else
			{
				m_HP = 0;
				m_prev_HP = 0;
				m_sprite_A.UpdateState(CEnemyArcher_AnimationSprite::ESpriteState::sprite_state_hit);
				m_pSound->PlaySnd_Enemy(CSoundManager::ENEMY_SOUND::Enemy_Die);
			}
		}
		break;
	}
	
}

bool CEnemy::CrashCheck()
{
	RECT temp;
	if (m_shot && IntersectRect(&temp, &m_ArrowBox, &(m_Tageted_Player->GetBodybox())))
	{
		return true;
	}
	if (IntersectRect(&temp, &m_atkbox, &(m_Tageted_Player->GetBodybox())))
	{
		return true;
	}
	return false;
}

void CEnemy::MoveToTaget(float fTimeElapsed)
{
	POINT m_PlayerPos = m_Tageted_Player->GetPos();

	m_fMoveLength += m_fSpeed * fTimeElapsed;

	if ((m_PlayerPos.x + 64 > m_ptCurrent.x - m_ScenePos.x + 64) && (m_fMoveLength >= 1))
	{
		m_ptCurrent.x += m_fMoveLength;
		m_bLeft = false;
		switch (m_Type)
		{
		case TypeOfEnemy::SwordMan:
			m_sprite.SetDirection(m_bLeft);
			m_sprite.UpdateState(CEnemyAnimationSprite::ESpriteState::sprite_state_move);
			break;
		case TypeOfEnemy::Archer:
			m_sprite_A.SetDirection(m_bLeft);
			m_sprite_A.UpdateState(CEnemyArcher_AnimationSprite::ESpriteState::sprite_state_move);
			break;
		}
		if (m_fCurrentUpdate > ENEMY_MOVE_SOUND_CYCLE)
		{
			m_pSound->PlaySnd_Enemy(CSoundManager::ENEMY_SOUND::Enemy_Move);
			m_fCurrentUpdate = 0;
		}
		m_fMoveLength = 0;
	}
	if ((m_PlayerPos.x + 64 < m_ptCurrent.x - m_ScenePos.x + 64) && (m_fMoveLength >= 1))
	{
		m_ptCurrent.x -= m_fMoveLength;
		m_bLeft = true;
		switch (m_Type)
		{
		case TypeOfEnemy::SwordMan:
			m_sprite.SetDirection(m_bLeft);
			m_sprite.UpdateState(CEnemyAnimationSprite::ESpriteState::sprite_state_move);
			break;
		case TypeOfEnemy::Archer:
			m_sprite_A.SetDirection(m_bLeft);
			m_sprite_A.UpdateState(CEnemyArcher_AnimationSprite::ESpriteState::sprite_state_move);
			break;
		}
		if (m_fCurrentUpdate > ENEMY_MOVE_SOUND_CYCLE)
		{
			m_pSound->PlaySnd_Enemy(CSoundManager::ENEMY_SOUND::Enemy_Move);
			m_fCurrentUpdate = 0;
		}
		m_fMoveLength = 0;
	}
}
