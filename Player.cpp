#include "stdafx.h"
#include "Player.h"

CPlayer::CPlayer(LPCTSTR path, CSoundManager *sm, int w, int h, float fFrequency) : CUnit(sm, fFrequency), m_sprite(path, w, h)
{
	m_cimgHP_Bar = new CSpriteImage(TEXT("Resource\\Graphics\\UI\\HP_BAR_R.png"), 1, 1);
	m_cimgBulletNum = new CSpriteImage(TEXT("Resource\\Graphics\\UI\\Bullet.png"), 12, 1);
	m_cimgPotionNum = new CSpriteImage(TEXT("Resource\\Graphics\\UI\\Potion.png"), 8, 1);
	m_cimgShot = new CSpriteImage(TEXT("Resource\\Graphics\\Sprite\\Bullet.png"), 1, 1);
	m_cimgEnemyCount.Load(TEXT("Resource\\Graphics\\UI\\몹_카운트.png"));
	m_colTransparent = ::GetPixel(m_cimgEnemyCount.GetDC(), 0, 0);

	m_HP_Bar = { 10, 10, 500, 40 };
	m_Bullet_Num = { 20, 60, 120, 100 };
	m_Potion_Num = { 20, 110, 120, 150 };

	m_MaxHP = PLAYER_HP;
	m_prev_HP = PLAYER_HP;
	m_HP	= PLAYER_HP;
	m_ATK	= PLAYER_ATK;
	m_DEF	= PLAYER_DEF;
	m_fSpeed = PLAYER_SPEED;

	m_NumOfBullet = PLAYER_START_BULLET;
	m_NumOfPotion = PLAYER_START_POTION;
}
CPlayer::~CPlayer()
{
	m_sprite.~CAnimationSprite();
	m_cimgHP_Bar->~CSpriteImage();
	m_cimgBulletNum->~CSpriteImage();
	m_cimgPotionNum->~CSpriteImage();
	m_cimgShot->~CSpriteImage();
}

void CPlayer::Input(InputKey input, bool bPushLShift)
{
	m_bPushShift = bPushLShift;

	switch (input)
	{
		// 키 놓음 상태 : idle 상태로 변경
	case InputKey::KeyUp:
		if (m_sprite.GetCurrentState() == CAnimationSprite::SpriteState::sprite_state_attack_0
			|| m_sprite.GetCurrentState() == CAnimationSprite::SpriteState::sprite_state_attack_1
			|| m_sprite.GetCurrentState() == CAnimationSprite::SpriteState::sprite_state_attack_2
			|| m_sprite.GetCurrentState() == CAnimationSprite::SpriteState::sprite_state_attack_3
			)
		{
			break;
		}
		m_Move = false;
		m_JumpAttack = false;
		m_sprite.UpdateState(CAnimationSprite::SpriteState::sprite_state_idle);
		break;
	case InputKey::MoveLeft:
		// 왼쪽으로 방향 변경
		if (m_sprite.GetCurrentState() == CAnimationSprite::SpriteState::sprite_state_idle ||
			m_sprite.GetCurrentState() == CAnimationSprite::SpriteState::sprite_state_jump ||
			m_sprite.GetCurrentState() == CAnimationSprite::SpriteState::sprite_state_move)
		{
			m_bLeft = true;
			m_sprite.SetDirection(m_bLeft);
		}
		// 이동
		if (!m_bPushShift)
		{
			m_Move = true;
		}
		// 대쉬
		else if (m_bPushShift && !m_bJump)
		{
			Dash();
			m_Move = false;
		}
		break;
	case InputKey::MoveRight:
		// 오른쪽으로 방향 변경
		if (m_sprite.GetCurrentState() == CAnimationSprite::SpriteState::sprite_state_idle ||
			m_sprite.GetCurrentState() == CAnimationSprite::SpriteState::sprite_state_jump ||
			m_sprite.GetCurrentState() == CAnimationSprite::SpriteState::sprite_state_move)
		{
			m_bLeft = false;
			m_sprite.SetDirection(m_bLeft);
		}
		// 이동
		if (!m_bPushShift)
		{
			m_Move = true;
		}
		// 대쉬
		else if (m_bPushShift && !m_bJump)
		{
			Dash();
			m_Move = false;
		}
		break;
	case InputKey::Jump:
		if (!m_bJump)
		{
			m_bJump = true;
		}
		break;
	case InputKey::Z:
		// 페링
		Parrying();
		m_Move = false;
		break;
	case InputKey::X:
		// 공격
		if (m_bJump)
		{
			// 점프 공격
			JumpAttack();
		}
		else
		{
			// 기본 공격
			Attack();
		}
		break;
	case InputKey::C:
		// 방어
		GuardAttack();
		m_Move = false;
		break;
	case InputKey::V:
		if (m_NumOfBullet > 0 && m_fCurrentUpdate > PLAYER_ATTACK_CYCLE)
		{
			// 사격 (Shift + 공격)
			Shot();
			m_Move = false;
		}
		break;
	case InputKey::A:
		if (m_NumOfPotion > 0 && m_fCurrentUpdate > PLAYER_ATTACK_CYCLE)
		{
			m_pSound->PlaySnd_UI(CSoundManager::UI_SOUND::Potion_UI);
			m_HP += PLAYER_POTION_SIZE;
			m_prev_HP = m_HP;
			m_NumOfPotion--;
			m_fCurrentUpdate = 0;
		}
		break;
	//case InputKey::O:
	//	if (m_Togle) m_Togle = false;
	//	else m_Togle = true;
	//	break;
	}
}

void CPlayer::OnDraw(HDC hDC)
{
	m_sprite.Draw(hDC, m_ptCurrent.x, int(m_ptCurrent.y - m_fCurrentJump));
	m_cimgShot->Draw(hDC, m_ShotBox.left, m_ShotBox.top, m_ShotBox.right - m_ShotBox.left, m_ShotBox.bottom - m_ShotBox.top);
	m_cimgHP_Bar->Draw(hDC, m_HP_Bar.left, m_HP_Bar.top, m_HP_Bar.left + m_HP , m_HP_Bar.bottom - m_HP_Bar.top);
	m_cimgBulletNum->Draw(hDC, m_Bullet_Num.left, m_Bullet_Num.top, 
		m_Bullet_Num.right - m_Bullet_Num.left, m_Bullet_Num.bottom - m_Bullet_Num.top);
	m_cimgPotionNum->Draw(hDC, m_Potion_Num.left, m_Potion_Num.top,
		m_Potion_Num.right - m_Potion_Num.left, m_Potion_Num.bottom - m_Potion_Num.top);

	if (m_Togle)
	{
		FrameRect(hDC, &m_bodybox, (HBRUSH)GetStockObject(WHITE_BRUSH));
		FrameRect(hDC, &m_atkbox, (HBRUSH)GetStockObject(WHITE_BRUSH));
		FrameRect(hDC, &m_JumpAttackBox, (HBRUSH)GetStockObject(WHITE_BRUSH));
		FrameRect(hDC, &m_ParryingBox, (HBRUSH)GetStockObject(WHITE_BRUSH));
		FrameRect(hDC, &m_GuardAttackBox, (HBRUSH)GetStockObject(WHITE_BRUSH));
	}

	HFONT myFont = CreateFont(40, 0, 0, 0, 0, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, 0, L"궁서체");
	HFONT oldFont = (HFONT)SelectObject(hDC, myFont);
	TCHAR temp[20];
	SetTextColor(hDC, RGB(51, 0, 153));
	wsprintf(temp, L"%d", m_NumOfBullet);
	TextOut(hDC, 125, 60, temp, lstrlen(temp));
	wsprintf(temp, L"%d", m_NumOfPotion);
	TextOut(hDC, 125, 110, temp, lstrlen(temp));
	SetTextColor(hDC, RGB(255, 255, 255));
	m_cimgEnemyCount.TransparentBlt(hDC, 460, 10, 70, 70, 
		0, 0, m_cimgEnemyCount.GetWidth(), m_cimgEnemyCount.GetHeight(), m_colTransparent);
	SelectObject(hDC, oldFont);
	DeleteObject(myFont);
	myFont = CreateFont(60, 0, 0, 0, 0, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, 0, L"궁서체");
	oldFont = (HFONT)SelectObject(hDC, myFont);
	SetTextColor(hDC, RGB(255, 0, 0));
	wsprintf(temp, L"%d", m_EnemyCount);
	if (!m_Boss) TextOut(hDC, 550, 20, temp, lstrlen(temp));
	else TextOut(hDC, 550, 20, L"BOSS", lstrlen(L"BOSS"));
	SelectObject(hDC, oldFont);
	DeleteObject(myFont);
}

void CPlayer::OnUpdate(float fTimeElapsed)
{
	if (m_Die) return;
	m_fTick += fTimeElapsed;
	if (m_fTick < m_fFrequency) return;
	m_fTick -= m_fFrequency;

	m_fCurrentUpdate += fTimeElapsed;
	// 스프라이트 갱신
	m_sprite.OnUpdate();
	m_cimgBulletNum->OnUpdate();
	m_cimgPotionNum->OnUpdate();

	Hit();

	if (m_sprite.GetCurrentState() == CAnimationSprite::SpriteState::sprite_state_hit && m_HP <= 0)
		m_fCurrentDie += fTimeElapsed;
	if (m_fCurrentDie > 0.5 && m_sprite.GetCurrentState() != CAnimationSprite::SpriteState::sprite_state_hit)
		m_Die = true;

	if (!m_Die)
	{
		m_EnemyCount = 0;
		for (auto p : *m_EnemyList)
		{
			if (!(p->IsDie())) m_EnemyCount++;
		}
		// 활성화 중이면 행동
		if (m_Move) Move(fTimeElapsed);
		if (m_bJump) Jump(fTimeElapsed);

		// 상호작용 갱신
		Update_Interaction(fTimeElapsed);
	}
}

void CPlayer::ReSet()
{
	m_HP_Bar = { 10, 10, 500, 40 };
	m_Bullet_Num = { 20, 60, 120, 100 };
	m_Potion_Num = { 20, 110, 120, 150 };

	m_MaxHP = PLAYER_HP;
	m_prev_HP = PLAYER_HP;
	m_HP = PLAYER_HP;
	m_ATK = PLAYER_ATK;
	m_DEF = PLAYER_DEF;
	m_fSpeed = PLAYER_SPEED;

	m_NumOfBullet = PLAYER_START_BULLET;
	m_NumOfPotion = PLAYER_START_POTION;

	m_fCurrentDie = 0;
	m_Die = false;
}

void CPlayer::Update_Interaction(float fTimeElapsed)
{
	// 캐릭터 몸체 판정박스 갱신
	if (m_sprite.GetCurrentState() == CAnimationSprite::SpriteState::sprite_state_hit ||
		m_sprite.GetCurrentState() == CAnimationSprite::SpriteState::sprite_state_dash)
		m_bodybox = { 0, 0, 0, 0 };
	else 
		m_bodybox = { 128 / 3 + m_ptCurrent.x, 48 + m_ptCurrent.y - (LONG)m_fCurrentJump, 
		256 / 3 + m_ptCurrent.x, 128 + m_ptCurrent.y - (LONG)m_fCurrentJump };

	// 공격 판정박스 갱신 및 충돌체크 후 상호작용
	Attack_Interaction();
	JumpAttack_Interaction();
	Shot_Interaction();
	GuardAttack_Interaction();
	Parrying_Interaction();
	Dash_Interaction(fTimeElapsed);
	Hit_Interaction(fTimeElapsed);

	if ((m_sprite.GetCurrentStateState() != CAnimationSprite::StateState::state_state_action && (
		m_sprite.GetCurrentState() == CAnimationSprite::SpriteState::sprite_state_guard ||
		m_sprite.GetCurrentState() == CAnimationSprite::SpriteState::sprite_state_jump_attack ||
		//m_sprite.GetCurrentState() == CAnimationSprite::SpriteState::sprite_state_shot ||
		m_sprite.GetCurrentState() == CAnimationSprite::SpriteState::sprite_state_parrying ||
		m_sprite.GetCurrentState() == CAnimationSprite::SpriteState::sprite_state_attack_0 ||
		m_sprite.GetCurrentState() == CAnimationSprite::SpriteState::sprite_state_attack_1 ||
		m_sprite.GetCurrentState() == CAnimationSprite::SpriteState::sprite_state_attack_2 ||
		m_sprite.GetCurrentState() == CAnimationSprite::SpriteState::sprite_state_attack_3)) ||
		m_sprite.GetCurrentState() == CAnimationSprite::SpriteState::sprite_state_idle)
	{
		m_Attack = false;
		m_Sound = false;
	}
}

void CPlayer::Attack()
{
	if (m_fCurrentUpdate > PLAYER_ATTACK_CYCLE) {
		if (m_AttackCombo == 1 )
		{
			m_sprite.UpdateState(CAnimationSprite::SpriteState::sprite_state_attack_1);
			m_AttackCombo = 2;
		}
		else if (m_AttackCombo == 2 )
		{
			m_sprite.UpdateState(CAnimationSprite::SpriteState::sprite_state_attack_2);
			m_AttackCombo = 3;
		}
		else if (m_AttackCombo == 3 )
		{
			m_sprite.UpdateState(CAnimationSprite::SpriteState::sprite_state_attack_3);
			m_AttackCombo = 0;
		}
		else if (m_AttackCombo == 0)
		{
			m_sprite.UpdateState(CAnimationSprite::SpriteState::sprite_state_attack_0);
			m_AttackCombo = 1;
		}
		m_fCurrentUpdate = 0;
	}
}
void CPlayer::Attack_Interaction()
{
	if (m_sprite.GetCurrentStateState() == CAnimationSprite::StateState::state_state_action && (
		m_sprite.GetCurrentState() == CAnimationSprite::SpriteState::sprite_state_attack_0 ||
		m_sprite.GetCurrentState() == CAnimationSprite::SpriteState::sprite_state_attack_1 ||
		m_sprite.GetCurrentState() == CAnimationSprite::SpriteState::sprite_state_attack_2 ||
		m_sprite.GetCurrentState() == CAnimationSprite::SpriteState::sprite_state_attack_3))
	{
		if (!m_Sound)
		{
			m_pSound->PlaySnd_Player(CSoundManager::PLAYER_SOUND::Attack_P);
			m_Sound = true;
		}
		if (m_bLeft)
		{
			m_atkbox = { m_ptCurrent.x , m_ptCurrent.y + 30 - (LONG)m_fCurrentJump, m_ptCurrent.x + 256 / 3 , m_ptCurrent.y + 110 - (LONG)m_fCurrentJump };
		}
		else if (!m_bLeft)
		{
			m_atkbox = { m_ptCurrent.x + 128 / 3 , m_ptCurrent.y + 30 - (LONG)m_fCurrentJump, m_ptCurrent.x + 128 , m_ptCurrent.y + 110 - (LONG)m_fCurrentJump };
		}
		if (!m_Attack)
		{
			if (m_EnemyList->begin() != m_EnemyList->end())
			for (auto p : *m_EnemyList)
			{
				if (CrashCheck(p, KindOfAttack::Normal_Attack))
				{
					int Damege{ 0 };
					Damege = m_ATK - p->GetDEF();
					if (Damege < 0) Damege = 0;
					p->GetHP() -= Damege;
				}
			}
			if (m_Boss != nullptr)
			{
				if (CrashCheck(m_Boss, KindOfAttack::Normal_Attack))
				{
					int Damege{ 0 };
					Damege = m_ATK - m_Boss->GetDEF();
					if (Damege < 0) Damege = 0;
					m_Boss->GetHP() -= Damege;
				}
			}
			m_Attack = true;
		}
	}
	else
	{
		m_atkbox = { 0, 0, 0, 0 };
	}
}

void CPlayer::JumpAttack()
{
	if (m_sprite.GetCurrentState() != CAnimationSprite::SpriteState::sprite_state_jump) return;
	m_JumpAttack = true;
	m_sprite.UpdateState(CAnimationSprite::SpriteState::sprite_state_jump_attack);
}
void CPlayer::JumpAttack_Interaction()
{
	if (m_sprite.GetCurrentStateState() == CAnimationSprite::StateState::state_state_action &&
		m_sprite.GetCurrentState() == CAnimationSprite::SpriteState::sprite_state_jump_attack)
	{
		if (!m_Sound)
		{
			m_pSound->PlaySnd_Player(CSoundManager::PLAYER_SOUND::Attack_P);
			m_Sound = true;
		}
		if (m_bLeft)
		{
			m_JumpAttackBox = { m_ptCurrent.x , m_ptCurrent.y + 10 - (LONG)m_fCurrentJump, m_ptCurrent.x + 256 / 3 , m_ptCurrent.y + 74 - (LONG)m_fCurrentJump };
		}
		else if (!m_bLeft)
		{
			m_JumpAttackBox = { m_ptCurrent.x + 128 / 3 , m_ptCurrent.y + 10 - (LONG)m_fCurrentJump, m_ptCurrent.x + 128 , m_ptCurrent.y + 74 - (LONG)m_fCurrentJump };
		}
		if (!m_Attack)
		{
			for (auto p : *m_EnemyList)
			{
				if (CrashCheck(p, KindOfAttack::Jump_Attack))
				{
					int Damege{ 0 };
					Damege = m_ATK - p->GetDEF();
					if (Damege < 0) Damege = 0;
					p->GetHP() -= Damege;
				}
			}
			if (m_Boss != nullptr)
			{
				if (CrashCheck(m_Boss, KindOfAttack::Jump_Attack))
				{
					int Damege{ 0 };
					Damege = m_ATK - m_Boss->GetDEF();
					if (Damege < 0) Damege = 0;
					m_Boss->GetHP() -= Damege;
				}
			}
			m_Attack = true;
		}
	}
	else
	{
		m_JumpAttackBox = { 0, 0, 0, 0 };
	}
}

void CPlayer::GuardAttack()
{
	m_sprite.UpdateState(CAnimationSprite::SpriteState::sprite_state_guard);
}
void CPlayer::GuardAttack_Interaction()
{
	if (m_sprite.GetCurrentStateState() == CAnimationSprite::StateState::state_state_action &&
		m_sprite.GetCurrentState() == CAnimationSprite::SpriteState::sprite_state_guard)
	{
		if (m_bLeft)
		{
			m_GuardAttackBox = { m_ptCurrent.x + 20, m_ptCurrent.y + 50 , m_ptCurrent.x + 128 , m_ptCurrent.y + 110 };
		}
		else if (!m_bLeft)
		{
			m_GuardAttackBox = { m_ptCurrent.x , m_ptCurrent.y + 50, m_ptCurrent.x + 110 , m_ptCurrent.y + 110 };
		}
		if (!m_Attack)
		{
			for (auto p : *m_EnemyList)
			{
				if (CrashCheck(p, KindOfAttack::Guard_Attack))
				{
					int Damege{ 0 };
					Damege = m_ATK - p->GetDEF();
					if (Damege < 0) Damege = 0;
					p->GetHP() -= Damege;
				}
			}
			if (m_Boss != nullptr)
			{
				if (CrashCheck(m_Boss, KindOfAttack::Guard_Attack))
				{
					int Damege{ 0 };
					Damege = m_ATK - m_Boss->GetDEF();
					if (Damege < 0) Damege = 0;
					m_Boss->GetHP() -= Damege;
				}
			}
			m_Attack = true;
		}
	}
	else
	{
		m_GuardAttackBox = { 0, 0, 0, 0 };
	}
	if (m_sprite.GetCurrentState() == CAnimationSprite::SpriteState::sprite_state_guard)
	{
		if (!m_Guard)
		{
			m_DEF += PLAYER_GUARD_DEF;
			m_pSound->PlaySnd_Player(CSoundManager::PLAYER_SOUND::Attack_P);
		}
		m_Guard = true;
	}
	else if (m_Guard)
	{
		m_DEF -= PLAYER_GUARD_DEF;
		m_Guard = false;
	}
}

void CPlayer::Parrying()
{
	m_sprite.UpdateState(CAnimationSprite::SpriteState::sprite_state_parrying);
}
void CPlayer::Parrying_Interaction()
{
	if (m_sprite.GetCurrentStateState() == CAnimationSprite::StateState::state_state_action &&
		m_sprite.GetCurrentState() == CAnimationSprite::SpriteState::sprite_state_parrying)
	{
		if (!m_Sound)
		{
			m_pSound->PlaySnd_Player(CSoundManager::PLAYER_SOUND::Parrying_P);
			m_Sound = true;
		}
		if (m_bLeft)
		{
			m_ParryingBox = { m_ptCurrent.x , m_ptCurrent.y + 40 , m_ptCurrent.x + 256 / 3 , m_ptCurrent.y + 100 };
		}
		else if (!m_bLeft)
		{
			m_ParryingBox = { m_ptCurrent.x + 128 / 3 , m_ptCurrent.y + 40 , m_ptCurrent.x + 128 , m_ptCurrent.y + 100 };
		}
		if (!m_Attack)
		{
			for (auto p : *m_EnemyList)
			{
				if (CrashCheck(p, KindOfAttack::Parrying_Attack))
				{
					int Damege{ 0 };
					Damege = (PLAYER_PARRYING_DAMAGE * m_ATK) - p->GetDEF();
					if (Damege < 0) Damege = 0;
					p->GetHP() -= Damege;
				}
			}
			if (m_Boss != nullptr)
			{
				if (CrashCheck(m_Boss, KindOfAttack::Parrying_Attack))
				{
					int Damege{ 0 };
					Damege = (PLAYER_PARRYING_DAMAGE * m_ATK) - m_Boss->GetDEF();
					if (Damege < 0) Damege = 0;
					m_Boss->GetHP() -= Damege;
				}
			}
			m_Attack = true;
		}
	}
	else
	{
		m_ParryingBox = { 0, 0, 0, 0 };
	}
}

void CPlayer::Shot()
{
	if (m_sprite.GetCurrentState() != CAnimationSprite::SpriteState::sprite_state_idle) return;
	m_sprite.UpdateState(CAnimationSprite::SpriteState::sprite_state_shot);
}
void CPlayer::Shot_Interaction()
{
	if (m_sprite.GetCurrentStateState() == CAnimationSprite::StateState::state_state_action &&
		m_sprite.GetCurrentState() == CAnimationSprite::SpriteState::sprite_state_shot)
	{
		if (m_bLeft)
		{
			m_ShotBox = { m_ptCurrent.x - PLAYER_SHOT_RANGE , m_ptCurrent.y + 75 , m_ptCurrent.x + 64 , m_ptCurrent.y + 80 };
		}
		else if (!m_bLeft)
		{
			m_ShotBox = { m_ptCurrent.x + 64 , m_ptCurrent.y + 75 , m_ptCurrent.x + PLAYER_SHOT_RANGE , m_ptCurrent.y + 80 };
		}
		if (!m_Attack)
		{
			for (auto p : *m_EnemyList)
			{
				if (CrashCheck(p, KindOfAttack::Shot_Attack))
				{
					int Damege{ 0 };
					Damege = (PLAYER_SHOT_DAMAGE * m_ATK) - p->GetDEF();
					if (Damege < 0) Damege = 0;
					p->GetHP() -= Damege;
				}
			}
			if (m_Boss != nullptr)
			{
				if (CrashCheck(m_Boss, KindOfAttack::Shot_Attack))
				{
					int Damege{ 0 };
					Damege = (PLAYER_SHOT_DAMAGE * m_ATK) - m_Boss->GetDEF();
					if (Damege < 0) Damege = 0;
					m_Boss->GetHP() -= Damege;
				}
			}
			m_pSound->PlaySnd_Player(CSoundManager::PLAYER_SOUND::Shot_P);
			m_Attack = true;
			m_NumOfBullet--;
		}
	}
	else
	{
		if (m_sprite.GetCurrentStateState() == CAnimationSprite::StateState::state_state_exit &&
			m_sprite.GetCurrentState() == CAnimationSprite::SpriteState::sprite_state_shot && !m_Sound)
		{
			m_pSound->PlaySnd_Player(CSoundManager::PLAYER_SOUND::Reload_P);
			m_Sound = true;
		}
		m_ShotBox = { 0, 0, 0, 0 };
	}
}

void CPlayer::Dash()
{
	if (m_sprite.GetCurrentState() == CAnimationSprite::SpriteState::sprite_state_attack_0 ||
		m_sprite.GetCurrentState() == CAnimationSprite::SpriteState::sprite_state_attack_1 ||
		m_sprite.GetCurrentState() == CAnimationSprite::SpriteState::sprite_state_attack_2 ||
		m_sprite.GetCurrentState() == CAnimationSprite::SpriteState::sprite_state_attack_3 ||
		m_sprite.GetCurrentState() == CAnimationSprite::SpriteState::sprite_state_parrying ||
		m_sprite.GetCurrentState() == CAnimationSprite::SpriteState::sprite_state_shot ||
		m_sprite.GetCurrentState() == CAnimationSprite::SpriteState::sprite_state_hit ||
		m_sprite.GetCurrentState() == CAnimationSprite::SpriteState::sprite_state_guard)
	{
		return;
	}
	if (m_sprite.GetCurrentState() != CAnimationSprite::SpriteState::sprite_state_dash)
	{
		m_pSound->PlaySnd_Player(CSoundManager::PLAYER_SOUND::Walk_P);
		m_pSound->PlaySnd_Player(CSoundManager::PLAYER_SOUND::Walk_P);
		m_pSound->PlaySnd_Player(CSoundManager::PLAYER_SOUND::Walk_P);
	}
	
	m_sprite.UpdateState(CAnimationSprite::SpriteState::sprite_state_dash);
}
void CPlayer::Dash_Interaction(float fTimeElapsed)
{
	if (m_sprite.GetCurrentStateState() != CAnimationSprite::StateState::state_state_exit &&
		m_sprite.GetCurrentState() == CAnimationSprite::SpriteState::sprite_state_dash)
	{
		m_fMoveLength += PLAYER_DASH_FORCE * m_fSpeed * fTimeElapsed;

		if (m_fMoveLength >= 1)
		{
			if (m_bLeft)
			{
				m_ptCurrent.x -= m_fMoveLength;
				m_fMoveLength = 0;
			}
			else if (!m_bLeft)
			{
				m_ptCurrent.x += m_fMoveLength;
				m_fMoveLength = 0;
			}
		}
		if (m_ptCurrent.x < 0) m_ptCurrent.x = 0;
		if (m_ptCurrent.y < 0) m_ptCurrent.y = 0;
	}
}

bool CPlayer::CrashCheck(CEnemy *enemy, KindOfAttack KA)
{
	RECT temp;
	switch (KA)
	{
	case KindOfAttack::Normal_Attack:
		if (IntersectRect(&temp, &m_atkbox, &(enemy->GetBodybox())))
		{
			return true;
		}
		break;
	case KindOfAttack::Guard_Attack:
		if (IntersectRect(&temp, &m_GuardAttackBox, &(enemy->GetBodybox())))
		{
			return true;
		}
		break;
	case KindOfAttack::Jump_Attack:
		if (IntersectRect(&temp, &m_JumpAttackBox, &(enemy->GetBodybox())))
		{
			return true;
		}
		break;
	case KindOfAttack::Parrying_Attack:
		if (IntersectRect(&temp, &m_ParryingBox, &(enemy->GetBodybox())))
		{
			return true;
		}
		break;
	case KindOfAttack::Shot_Attack:
		if (IntersectRect(&temp, &m_ShotBox, &(enemy->GetBodybox())))
		{
			return true;
		}
		break;
	}
	return false;
}
bool CPlayer::CrashCheck(CBoss * boss, KindOfAttack KA)
{
	RECT temp;
	switch (KA)
	{
	case KindOfAttack::Normal_Attack:
		if (IntersectRect(&temp, &m_atkbox, &(boss->GetBodybox())))
		{
			return true;
		}
		break;
	case KindOfAttack::Guard_Attack:
		if (IntersectRect(&temp, &m_GuardAttackBox, &(boss->GetBodybox())))
		{
			return true;
		}
		break;
	case KindOfAttack::Jump_Attack:
		if (IntersectRect(&temp, &m_JumpAttackBox, &(boss->GetBodybox())))
		{
			return true;
		}
		break;
	case KindOfAttack::Parrying_Attack:
		if (IntersectRect(&temp, &m_ParryingBox, &(boss->GetBodybox())))
		{
			return true;
		}
		break;
	case KindOfAttack::Shot_Attack:
		if (IntersectRect(&temp, &m_ShotBox, &(boss->GetBodybox())))
		{
			return true;
		}
		break;
	}
	return false;
}

void CPlayer::Hit()
{
	if (m_HP != m_prev_HP)
	{
		m_prev_HP = m_HP;
		m_sprite.UpdateState(CAnimationSprite::SpriteState::sprite_state_hit);
		m_pSound->PlaySnd_Player(CSoundManager::PLAYER_SOUND::Hit_P);
	}
	else if (!m_bJump) m_Gravity_AC = 0;
}
void CPlayer::Hit_Interaction(float fTimeElapsed)
{
	if (m_sprite.GetCurrentState() == CAnimationSprite::SpriteState::sprite_state_hit &&
		m_sprite.GetCurrentStateState() != CAnimationSprite::StateState::state_state_exit)
	{
		m_Gravity_AC += GRAVITY_FRAME_COEF;

		m_fMoveLength += (PLAYER_KNOCKBACK_FORCE - m_Gravity_AC) * fTimeElapsed;

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
		if (m_ptCurrent.x < 0) m_ptCurrent.x = 0;
		if (m_ptCurrent.y < 0) m_ptCurrent.y = 0;

		m_fCurrentJump +=  (- m_Gravity_AC) * fTimeElapsed;

		// 점프 상태가 아니라면 점프 비활성화
		if (m_fCurrentJump < 0)
		{
			m_fCurrentJump = 0;
			m_bJump = false;
			m_Gravity_AC = 0;
		}
	}
}

void CPlayer::Move(float fTimeElapsed)
{
	// 행동하는 상태일 경우에는 정지
	if (m_sprite.GetCurrentState() == CAnimationSprite::SpriteState::sprite_state_attack_0 ||
		m_sprite.GetCurrentState() == CAnimationSprite::SpriteState::sprite_state_attack_1 ||
		m_sprite.GetCurrentState() == CAnimationSprite::SpriteState::sprite_state_attack_2 ||
		m_sprite.GetCurrentState() == CAnimationSprite::SpriteState::sprite_state_attack_3 ||
		m_sprite.GetCurrentState() == CAnimationSprite::SpriteState::sprite_state_parrying ||
		m_sprite.GetCurrentState() == CAnimationSprite::SpriteState::sprite_state_shot ||
		m_sprite.GetCurrentState() == CAnimationSprite::SpriteState::sprite_state_dash ||
		m_sprite.GetCurrentState() == CAnimationSprite::SpriteState::sprite_state_hit ||
		m_sprite.GetCurrentState() == CAnimationSprite::SpriteState::sprite_state_guard)
	{
		m_Move = false;
		return;
	}

	if (m_sprite.GetCurrentState() == CAnimationSprite::SpriteState::sprite_state_idle)
		m_pSound->PlaySnd_Player(CSoundManager::PLAYER_SOUND::Walk_P);
	// move 상태로 변경
	m_sprite.UpdateState(CAnimationSprite::SpriteState::sprite_state_move);
	// 이동
	m_fMoveLength += m_fSpeed * fTimeElapsed;

	if (m_fMoveLength >= 1)
	{
		if (m_bLeft)
		{
			m_ptCurrent.x -= m_fMoveLength;
			m_fMoveLength = 0;
		}
		else if (!m_bLeft)
		{
			m_ptCurrent.x+= m_fMoveLength;
			m_fMoveLength = 0;
		}
	}
	if (m_ptCurrent.x < 0) m_ptCurrent.x = 0;
	if (m_ptCurrent.y < 0) m_ptCurrent.y = 0;
}

void CPlayer::Jump(float fTimeElapsed, float maxJump)
{
	// 행동하는 상태일 경우에는 정지
	if (m_sprite.GetCurrentState() == CAnimationSprite::SpriteState::sprite_state_attack_0 ||
		m_sprite.GetCurrentState() == CAnimationSprite::SpriteState::sprite_state_attack_1 ||
		m_sprite.GetCurrentState() == CAnimationSprite::SpriteState::sprite_state_attack_2 ||
		m_sprite.GetCurrentState() == CAnimationSprite::SpriteState::sprite_state_attack_3 ||
		m_sprite.GetCurrentState() == CAnimationSprite::SpriteState::sprite_state_parrying ||
		m_sprite.GetCurrentState() == CAnimationSprite::SpriteState::sprite_state_shot ||
		m_sprite.GetCurrentState() == CAnimationSprite::SpriteState::sprite_state_dash ||
		m_sprite.GetCurrentState() == CAnimationSprite::SpriteState::sprite_state_hit ||
		m_sprite.GetCurrentState() == CAnimationSprite::SpriteState::sprite_state_guard)
	{
		m_bJump = false;
		m_fCurrentJump = 0;
		return;
	}
	m_sprite.UpdateState(CAnimationSprite::SpriteState::sprite_state_jump);
	if (m_fCurrentJump == 0) m_pSound->PlaySnd_Player(CSoundManager::PLAYER_SOUND::Jump_P);

	m_Gravity_AC += 100 * m_fSpeed * GRAVITY_FRAME_COEF;

	m_fCurrentJump += (maxJump - m_Gravity_AC) * fTimeElapsed;

	// 점프 상태가 아니라면 점프 비활성화
	if (m_fCurrentJump < 0)
	{
		m_fCurrentJump = 0;
		m_bJump = false;
		m_Gravity_AC = 0;
		m_Move = false;
	}
}