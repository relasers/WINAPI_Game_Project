#include "stdafx.h"
#include "Boss.h"

CBoss::CBoss(LPCTSTR path, CSoundManager *sm, int w, int h, float fFrequency) : CUnit(sm, fFrequency), m_sprite(path, w, h)
{
	m_cimgHP_Bar = new CSpriteImage(TEXT("Resource\\Graphics\\UI\\HP_BAR_R.png"), 1, 1);
	m_MaxHP = BOSS_HP;
	m_prev_HP = BOSS_HP / 3;
	m_HP = BOSS_HP / 3;
	m_ATK = BOSS_ATK;
	m_DEF = BOSS_DEF;
	m_fSpeed = BOSS_SPEED;
	m_phase = BossPhase::Phase1;
}
CBoss::~CBoss()
{
	m_cimgHP_Bar->~CSpriteImage();
	m_sprite.~CBOSSAnimationSprite();
}

void CBoss::OnDraw(HDC hDC)
{
	if (m_Die) return;
	if (m_ScenePos) {
		if (!m_Die) m_sprite.Draw(hDC, m_ptCurrent.x - m_ScenePos->x, m_ptCurrent.y - m_ScenePos->y + 500);
		if (m_Togle)
		{
			FrameRect(hDC, &m_bodybox, (HBRUSH)GetStockObject(WHITE_BRUSH));
			if (m_Attack) FrameRect(hDC, &m_atkbox, (HBRUSH)GetStockObject(WHITE_BRUSH));
		}
		m_cimgHP_Bar->Draw(hDC, m_HP_Bar.left, m_HP_Bar.top, m_HP_Bar.right - m_HP_Bar.left, m_HP_Bar.bottom - m_HP_Bar.top);
	}
}

void CBoss::OnUpdate(float fTimeElapsed, POINT* ScenePos)
{
	m_fTick += fTimeElapsed;
	if (m_fTick < m_fFrequency) return;
	m_fTick -= m_fFrequency;

	m_ScenePos = ScenePos;

	// 스프라이트 갱신
	if (!m_Die) m_sprite.OnUpdate();

	if (m_ScenePos) {
		if (!m_Die) Hit();

		if (m_sprite.GetCurrentState() == CBOSSAnimationSprite::SpriteState::sprite_state_phase3_die && m_HP <= 0)
			m_fCurrentDie += fTimeElapsed;
		if (m_fCurrentDie > 0.5 && m_sprite.GetCurrentState() != CBOSSAnimationSprite::SpriteState::sprite_state_phase3_die)\
			m_Die = true;\

		if (!m_Die)
		{
			if (m_Tageted_Player->GetPos().x - (m_ptCurrent.x - m_ScenePos->x) < BOSS_SEARCH_RANGE &&
				m_Tageted_Player->GetPos().x - (m_ptCurrent.x - m_ScenePos->x) > -BOSS_SEARCH_RANGE)
				switch (m_phase)
				{
				case BossPhase::Phase1:
					break;
				case BossPhase::Phase2:
					if ((m_fCurrentActive += fTimeElapsed) > BOSS_ACTIVE_CYCLE)
					{
						m_fCurrentActive = 0;
						if (rand() % 2) Summon();
						else Attack();
					}
					break;
				case BossPhase::Phase3:
					if (m_sprite.GetCurrentState() == CBOSSAnimationSprite::SpriteState::sprite_state_phase3_idle &&
						m_sprite.GetCurrentStateState() == CBOSSAnimationSprite::StateState::state_state_exit)
					{
						if (rand() % 2) Attack();
						else Dash();
					}

					break;
				}
			Update_Interaction(fTimeElapsed);
		}
	}
}

void CBoss::ReSet()
{
	m_MaxHP = BOSS_HP;
	m_prev_HP = BOSS_HP / 3;
	m_HP = BOSS_HP / 3;
	m_ATK = BOSS_ATK;
	m_DEF = BOSS_DEF;
	m_fSpeed = BOSS_SPEED;
	m_phase = BossPhase::Phase1;
	m_sprite.OnUpdate(true);
	m_fCurrentDie = 0;
	m_Die = false;
}

void CBoss::Update_Interaction(float fTimeElapsed)
{
	POINT m_PlayerPos = m_Tageted_Player->GetPos();
	if (m_phase == BossPhase::Phase3)
		m_bodybox = { m_ptCurrent.x - m_ScenePos->x + 100, m_ptCurrent.y - m_ScenePos->y + 570,
			m_ptCurrent.x - m_ScenePos->x + 156, 186 + m_ptCurrent.y - m_ScenePos->y + 500 };
	else
		m_bodybox = { m_ptCurrent.x - m_ScenePos->x + 70, m_ptCurrent.y - m_ScenePos->y + 510,
		m_ptCurrent.x - m_ScenePos->x + 186, 186 + m_ptCurrent.y - m_ScenePos->y + 500 };
	m_HP_Bar = { m_ptCurrent.x - m_ScenePos->x + 70, m_ptCurrent.y - m_ScenePos->y + 490,
		m_ptCurrent.x - m_ScenePos->x + 70 + m_HP,  m_ptCurrent.y - m_ScenePos->y + 500 };

	if (m_sprite.GetCurrentState() == CBOSSAnimationSprite::SpriteState::sprite_state_phase1_idle ||
		m_sprite.GetCurrentState() == CBOSSAnimationSprite::SpriteState::sprite_state_phase2_idle ||
		m_sprite.GetCurrentState() == CBOSSAnimationSprite::SpriteState::sprite_state_phase3_idle)
	{
		if (m_PlayerPos.x + 64 > m_ptCurrent.x - m_ScenePos->x + 128)
		{
			m_bLeft = false;
			m_sprite.SetDirection(m_bLeft);
		}
		if (m_PlayerPos.x + 64 < m_ptCurrent.x - m_ScenePos->x + 128)
		{
			m_bLeft = true;
			m_sprite.SetDirection(m_bLeft);
		}
	}
	switch (m_phase)
	{
	case BossPhase::Phase1:
		break;
	case BossPhase::Phase2:
		Summon_Interaction(fTimeElapsed);
		break;
	case BossPhase::Phase3:
		Attack_Interaction();
		Dash_Interaction(fTimeElapsed);
		Idle_Interaction(fTimeElapsed);
		break;
	}
}

void CBoss::Summon()
{
	if (m_sprite.GetCurrentState() != CBOSSAnimationSprite::SpriteState::sprite_state_phase2_idle &&
		m_sprite.GetCurrentState() != CBOSSAnimationSprite::SpriteState::sprite_state_phase2_hit)
		return;
	m_sprite.UpdateState(CBOSSAnimationSprite::SpriteState::sprite_state_phase2_summon);
}

void CBoss::Summon_Interaction(float fTimeElapsed)
{
	if (m_sprite.GetCurrentStateState() == CBOSSAnimationSprite::StateState::state_state_action && (
		m_sprite.GetCurrentState() == CBOSSAnimationSprite::SpriteState::sprite_state_phase2_summon ||
		m_sprite.GetCurrentState() == CBOSSAnimationSprite::SpriteState::sprite_state_phase2_attack))
	{
		if ((m_fCurrentUpdate += fTimeElapsed) > BOSS_SUMMON_CYCLE)
		{
			m_pSound->PlaySnd_Boss(CSoundManager::BOSS_SOUND::Boss_Phase2_Summon);
			m_fCurrentUpdate = 0;

			for (auto p : *m_EnemyList)
			{
				if (p->IsDie())
				{
					p->ReSet();
					p->SetPos({ m_ptCurrent.x - 300 + rand() % 600, 500 });
					p->Tageting(m_Tageted_Player);
					return;
				}
			}

			//switch ((CEnemy::TypeOfEnemy)(rand() % (int)CEnemy::TypeOfEnemy::Count))
			//{
			//case CEnemy::TypeOfEnemy::SwordMan:
			//	m_EnemyList->push_front(new CEnemy(TEXT("Resource\\Graphics\\Sprite\\몬스터.png"), m_pSound, 23, 12, CEnemy::TypeOfEnemy::SwordMan));
			//	break;
			//case CEnemy::TypeOfEnemy::Archer:
			//	m_EnemyList->push_front(new CEnemy(TEXT("Resource\\Graphics\\Sprite\\몬스터_아쳐.png"), m_pSound, 22, 12, CEnemy::TypeOfEnemy::Archer));
			//	break;
			//}
			//auto p = m_EnemyList->begin();
			//
			//switch ((*p)->GetType())
			//{
			//case CEnemy::TypeOfEnemy::SwordMan:
			//	(*p)->SetState(CEnemyAnimationSprite::ESpriteState::sprite_state_idle, 8);
			//	(*p)->SetState(CEnemyAnimationSprite::ESpriteState::sprite_state_move, 6, 0, 0, 0, 6);
			//	(*p)->SetState(CEnemyAnimationSprite::ESpriteState::sprite_state_attack, 7, 0, 3, 5, 7);
			//	(*p)->SetState(CEnemyAnimationSprite::ESpriteState::sprite_state_guard, 6, 0, 2, 2, 6);
			//	(*p)->SetState(CEnemyAnimationSprite::ESpriteState::sprite_state_hit, 10, 0, 7, 7, 10);
			//	(*p)->SetState(CEnemyAnimationSprite::ESpriteState::sprite_state_pop, 23, 0, 7, 15, 23);
			//
			//	(*p)->SetStateDelay(CEnemyAnimationSprite::ESpriteState::sprite_state_idle, 5);
			//	(*p)->SetStateDelay(CEnemyAnimationSprite::ESpriteState::sprite_state_move, 5, 5, 3);
			//	(*p)->SetStateDelay(CEnemyAnimationSprite::ESpriteState::sprite_state_attack, 15, 1, 15);
			//	(*p)->SetStateDelay(CEnemyAnimationSprite::ESpriteState::sprite_state_guard, 1, 1, 10);
			//	(*p)->SetStateDelay(CEnemyAnimationSprite::ESpriteState::sprite_state_hit, 10, 5, 5);
			//	(*p)->SetStateDelay(CEnemyAnimationSprite::ESpriteState::sprite_state_pop, 3, 3, 3);
			//	break;
			//case CEnemy::TypeOfEnemy::Archer:
			//	(*p)->SetState(CEnemyArcher_AnimationSprite::ESpriteState::sprite_state_idle, 5);
			//	(*p)->SetState(CEnemyArcher_AnimationSprite::ESpriteState::sprite_state_move, 6, 0, 0, 0, 6);
			//	(*p)->SetState(CEnemyArcher_AnimationSprite::ESpriteState::sprite_state_attack, 7, 0, 4, 6, 7);
			//	(*p)->SetState(CEnemyArcher_AnimationSprite::ESpriteState::sprite_state_shot, 9, 0, 5, 8, 9);
			//	(*p)->SetState(CEnemyArcher_AnimationSprite::ESpriteState::sprite_state_hit, 7, 0, 0, 0, 7);
			//	(*p)->SetState(CEnemyArcher_AnimationSprite::ESpriteState::sprite_state_pop, 22, 0, 7, 15, 22);
			//
			//	(*p)->SetStateDelay(CEnemyArcher_AnimationSprite::ESpriteState::sprite_state_idle, 5);
			//	(*p)->SetStateDelay(CEnemyArcher_AnimationSprite::ESpriteState::sprite_state_move, 5, 5, 3);
			//	(*p)->SetStateDelay(CEnemyArcher_AnimationSprite::ESpriteState::sprite_state_attack, 15, 1, 15);
			//	(*p)->SetStateDelay(CEnemyArcher_AnimationSprite::ESpriteState::sprite_state_shot, 15, 1, 10);
			//	(*p)->SetStateDelay(CEnemyArcher_AnimationSprite::ESpriteState::sprite_state_hit, 10, 5, 5);
			//	(*p)->SetStateDelay(CEnemyArcher_AnimationSprite::ESpriteState::sprite_state_pop, 3, 3, 3);
			//	break;
			//}
			//
			//(*p)->SetPos({ m_ptCurrent.x - 300 + rand() % 600, 500 });
			//(*p)->Tageting(m_Tageted_Player);
		}
	}
}

void CBoss::Attack()
{
	switch (m_phase)
	{
	case BossPhase::Phase2:
		if (m_sprite.GetCurrentState() != CBOSSAnimationSprite::SpriteState::sprite_state_phase2_idle &&
			m_sprite.GetCurrentState() != CBOSSAnimationSprite::SpriteState::sprite_state_phase2_hit)
			return;
		m_sprite.UpdateState(CBOSSAnimationSprite::SpriteState::sprite_state_phase2_attack);

		break;
	case BossPhase::Phase3:
		if (m_sprite.GetCurrentState() != CBOSSAnimationSprite::SpriteState::sprite_state_phase3_idle) return;
		m_sprite.UpdateState(CBOSSAnimationSprite::SpriteState::sprite_state_phase3_attack);
		m_pSound->PlaySnd_Enemy(CSoundManager::ENEMY_SOUND::Enemy_Attack);
		break;
	}
}

void CBoss::Attack_Interaction()
{
	if (m_sprite.GetCurrentStateState() == CBOSSAnimationSprite::StateState::state_state_action &&
		m_sprite.GetCurrentState() == CBOSSAnimationSprite::SpriteState::sprite_state_phase3_attack)
	{
		if (m_bLeft)
		{
			m_atkbox = { m_ptCurrent.x - m_ScenePos->x , m_ptCurrent.y - m_ScenePos->y + 530,
				206 + m_ptCurrent.x - m_ScenePos->x, 186 + m_ptCurrent.y - m_ScenePos->y + 500 };
		}
		else if (!m_bLeft)
		{
			m_atkbox = { m_ptCurrent.x - m_ScenePos->x + 50, m_ptCurrent.y - m_ScenePos->y + 530,
				256 + m_ptCurrent.x - m_ScenePos->x, 186 + m_ptCurrent.y - m_ScenePos->y + 500 };
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
		if (m_Attack && m_sprite.GetCurrentStateState() == CAnimationSprite::StateState::state_state_exit)
			m_pSound->PlaySnd_Player(CSoundManager::PLAYER_SOUND::Reload_P);
		m_atkbox = { 0, 0, 0, 0 };
		m_Attack = false;
	}
}

void CBoss::Dash()
{
	if (m_sprite.GetCurrentState() == CBOSSAnimationSprite::SpriteState::sprite_state_phase3_idle)
	{
		m_sprite.UpdateState(CBOSSAnimationSprite::SpriteState::sprite_state_phase3_dash_ready);
		m_pSound->PlaySnd_Player(CSoundManager::PLAYER_SOUND::Shot_P);
	}
}

void CBoss::Dash_Interaction(float fTimeElapsed)
{
	if (m_sprite.GetCurrentStateState() == CBOSSAnimationSprite::StateState::state_state_action &&
		m_sprite.GetCurrentState() == CBOSSAnimationSprite::SpriteState::sprite_state_phase3_dash)
	{
		m_fMoveLength += BOSS_DASH_SPEED * m_fSpeed * fTimeElapsed;
		m_pSound->PlaySnd_Player(CSoundManager::PLAYER_SOUND::Shot_P);

		if (m_bLeft)
		{
			m_atkbox = { m_ptCurrent.x - m_ScenePos->x + 100, m_ptCurrent.y - m_ScenePos->y + 570,
				m_ptCurrent.x - m_ScenePos->x + 156, 186 + m_ptCurrent.y - m_ScenePos->y + 500 };
			if (m_fMoveLength >= 1)
			{
				m_ptCurrent.x -= m_fMoveLength;
				m_fMoveLength = 0;
			}
		}
		else if (!m_bLeft)
		{
			m_atkbox = { m_ptCurrent.x - m_ScenePos->x + 100, m_ptCurrent.y - m_ScenePos->y + 570,
				m_ptCurrent.x - m_ScenePos->x + 156, 186 + m_ptCurrent.y - m_ScenePos->y + 500 };
			if (m_fMoveLength >= 1)
			{
				m_ptCurrent.x += m_fMoveLength;
				m_fMoveLength = 0;
			}
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
}

void CBoss::Idle_Interaction(float fTimeElapsed)
{
	if (m_sprite.GetCurrentStateState() == CBOSSAnimationSprite::StateState::state_state_action &&
		m_sprite.GetCurrentState() == CBOSSAnimationSprite::SpriteState::sprite_state_phase3_idle)
	{
		if (!m_Sound)
		{
			m_pSound->PlaySnd_Boss(CSoundManager::BOSS_SOUND::Boss_Phase3_Idle);
			m_Sound = true;
		}
		m_fCurrentUpdate += BOSS_CAMERA_SHAKE * fTimeElapsed;
		m_ScenePos->y = 500 + BOSS_CAMERA_SHAKE * sin(m_fCurrentUpdate*10.0f) * powf(0.5f, m_fCurrentUpdate);
	}
	else
	{
		m_ScenePos->y = 500;
		m_fCurrentUpdate = 0;
		m_Sound = false;
	}
}

void CBoss::Hit()
{
	if (m_HP != m_prev_HP)
	{
		m_prev_HP = m_HP;
		if (m_HP > 0)
		{
			if (m_sprite.GetCurrentState() == CBOSSAnimationSprite::SpriteState::sprite_state_phase2_shift ||
				m_sprite.GetCurrentState() == CBOSSAnimationSprite::SpriteState::sprite_state_phase3_shift)
			{
				m_HP = m_MaxHP;
				m_prev_HP = m_MaxHP;
			}
			switch (m_phase)
			{
			case BossPhase::Phase1:
				m_sprite.UpdateState(CBOSSAnimationSprite::SpriteState::sprite_state_phase1_hit);
				m_pSound->PlaySnd_Enemy(CSoundManager::ENEMY_SOUND::Enemy_Hit);
				break;
			case BossPhase::Phase2:
				m_pSound->PlaySnd_Enemy(CSoundManager::ENEMY_SOUND::Enemy_Hit);
				if (m_sprite.GetCurrentState() == CBOSSAnimationSprite::SpriteState::sprite_state_phase2_attack ||
					m_sprite.GetCurrentState() == CBOSSAnimationSprite::SpriteState::sprite_state_phase2_summon)
					break;
				m_sprite.UpdateState(CBOSSAnimationSprite::SpriteState::sprite_state_phase2_hit);
				break;
			case BossPhase::Phase3:
				m_pSound->PlaySnd_Enemy(CSoundManager::ENEMY_SOUND::Enemy_Hit);
				break;
			}
		}
		else
		{
			switch(m_phase)
			{
			case BossPhase::Phase1:
				m_sprite.UpdateState(CBOSSAnimationSprite::SpriteState::sprite_state_phase2_shift);
				m_pSound->PlaySnd_Boss(CSoundManager::BOSS_SOUND::Boss_Phase2_Shift);
				m_phase = BossPhase::Phase2;
				m_HP = m_MaxHP;
				break;
			case BossPhase::Phase2:
				m_sprite.UpdateState(CBOSSAnimationSprite::SpriteState::sprite_state_phase3_shift);
				m_pSound->PlaySnd_Boss(CSoundManager::BOSS_SOUND::Boss_Phase3_Shift);
				m_phase = BossPhase::Phase3;
				m_HP = m_MaxHP;
				break;
			case BossPhase::Phase3:
				m_sprite.UpdateState(CBOSSAnimationSprite::SpriteState::sprite_state_phase3_die);
				m_pSound->PlaySnd_Boss(CSoundManager::BOSS_SOUND::Boss_Die);
				break;
			}
		}
	}
}

bool CBoss::CrashCheck()
{
	RECT temp;
	if (IntersectRect(&temp, &m_atkbox, &(m_Tageted_Player->GetBodybox())))
	{
		return true;
	}
	return false;
}
