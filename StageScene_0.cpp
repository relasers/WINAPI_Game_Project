#include "stdafx.h"
#include "Framework.h"
#include "WarpList.h"
#include "Player.h"
#include "Enemy.h"
#include "StageScene_0.h"

using namespace std;

CStageScene_0::CStageScene_0()
{
}
CStageScene_0::~CStageScene_0()
{
	ReleaseObjects();
}

void CStageScene_0::Update(float fTimeElapsed)
{
	if (!m_Build)
	{
		m_ScenePos = { 100, 500 };
		(*m_Player)->ReSet();
		(*m_Player)->SetPos({ 100, 350 });
		for (auto p : m_EnemyList)
		{
			p->ReSet();
			p->SetPos({ rand() % (m_cimgBackground.GetWidth() - 700) + 500, 350 });
			p->Tageting(*m_Player);
		}
		(*m_Player)->Tageting(m_EnemyList);
		m_Build = true;
	}
	m_fCurrentUpdate += fTimeElapsed;
	// 키 입력
	if (m_fCurrentUpdate > INPUT_KEY_CYCLE)
	{
		ProcessInput();
		m_fCurrentUpdate = 0;
	}

	// Object Updating
	(*m_Player)->OnUpdate(fTimeElapsed);
	if (m_EnemyList.begin() != m_EnemyList.end())
		for (auto p : m_EnemyList)
		{
			p->OnUpdate(fTimeElapsed, m_ScenePos);
		}
	for (auto p : m_EnemyList)
	{
		if (p->IsDie()) m_Clear = true;
		else
		{
			m_Clear = false;
			break;
		}
	}

	// Scene Updating
	Update_Scene();

	// 유닛 사망시
	//if (m_EnemyList.begin() != m_EnemyList.end())
	//	m_EnemyList.remove_if([](const decltype(m_EnemyList)::type& data)-> bool {
	//		return (data->IsDie());
	//	});
	if ((*m_Player)->IsDie())
	{
		m_pSound->PlaySnd_Player(CSoundManager::PLAYER_SOUND::Die_P);
		m_pSound->PlaySnd_BGM(CSoundManager::BGM_SOUND::Stop_BGM);
		m_pSound->PlaySnd_BGM(CSoundManager::BGM_SOUND::GameOver_BGM);
		m_pFramework->ChangeScene(CurrentScene::GameOver);
	}
}

void CStageScene_0::Draw(HDC hdc)
{
	m_cimgBackground.BitBlt(hdc, m_rcScene.left, m_rcScene.top, m_rcScene.right - m_rcScene.left, m_rcScene.bottom - m_rcScene.top
		,m_ScenePos.x , m_ScenePos.y, SRCCOPY);

	// Unit Draw
	if (m_EnemyList.begin() != m_EnemyList.end())
		for (auto p : m_EnemyList)
		{
			p->OnDraw(hdc);
		}

	(*m_Player)->OnDraw(hdc);

	if (m_bF1) m_cimgControl.Draw(hdc, m_rcScene.right / 5, m_rcScene.bottom / 5, m_rcScene.right * 3 / 5, m_rcScene.bottom * 3 / 5,
		0, 0, m_cimgControl.GetWidth(), m_cimgControl.GetHeight());
}

bool CStageScene_0::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_KEYDOWN:
		break;
	case WM_KEYUP:
		if (wParam == VK_F1)
			if (!m_bF1) m_bF1 = true;
			else m_bF1 = false;
		break;
	}
	return false;
}
bool CStageScene_0::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_RBUTTONDOWN:
		return true;
	case WM_MOUSEMOVE:
		return true;
	}
	return false;
}
bool CStageScene_0::OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_SIZE:
		break;
	case WM_DESTROY:
		break;
	}
	return false;
}

void CStageScene_0::BuildObjects(CFramework * myFramework, CSoundManager *pSoundManager, HWND hWnd, CurrentScene tag, CPlayer **Player)
{
	CScene::BuildObjects(myFramework, pSoundManager, hWnd, tag, Player);
	m_cimgBackground.Load(TEXT("Resource\\Graphics\\Background\\Stage_0_BK.png"));
	m_cimgControl.Load(TEXT("Resource\\Graphics\\UI\\조작법.png"));

	m_ScenePos = { 100, 500 };
	m_Player = Player;
	// Unit Initialize
	UnitInitialize();

	// Set Unit Pos
	(*m_Player)->SetPos({ 100, 350 });

	// Unit Tageting
	(*m_Player)->Tageting(m_EnemyList);
	for (auto p : m_EnemyList)
	{
		p->Tageting(*m_Player);
	}

	// Set Togle
	(*m_Player)->SetTogle(TOGLE);
	for (auto p : m_EnemyList)
	{
		p->SetTogle(TOGLE);
	}

	m_Build = false;
	m_Clear = false;
}

void CStageScene_0::ReSetObjects()
{
	m_Build = false;
	m_Clear = false;

	for (auto p : m_EnemyList)
	{
		p->ReSet();
	}
}

void CStageScene_0::ReleaseObjects()
{
	m_EnemyList.remove_if([](const decltype(m_EnemyList)::type& data)->bool { data->~CEnemy(); return true; });
	m_cimgBackground.Destroy();
}

void CStageScene_0::Update_Scene()
{
	m_PlayerPos = (*m_Player)->GetPos();

	if (m_PlayerPos.x > (m_rcScene.right * 2) / SCENE_MOVE_PIVOT)
	{
		m_ScenePos.x += m_PlayerPos.x - (m_rcScene.right * 2) / SCENE_MOVE_PIVOT;
		if (m_ScenePos.x > m_cimgBackground.GetWidth() - (m_rcScene.right - m_rcScene.left))
		{
			m_ScenePos.x = m_cimgBackground.GetWidth() - (m_rcScene.right - m_rcScene.left);
			if (m_PlayerPos.x > m_rcScene.right - 100)
			{
				m_PlayerPos.x = m_rcScene.right - 100;
				if (m_Clear) m_pFramework->ChangeScene(CurrentScene::Stage_1);
			}
		}
		else
		{
			m_PlayerPos.x = (m_rcScene.right * 2) / SCENE_MOVE_PIVOT;
		}
	}
	if (m_PlayerPos.x < m_rcScene.right / SCENE_MOVE_PIVOT - 100)
	{
		m_ScenePos.x -= m_rcScene.right / SCENE_MOVE_PIVOT - 100 - m_PlayerPos.x;
		if (m_ScenePos.x < 0)
		{
			m_ScenePos.x = 0;
			if (m_PlayerPos.x < 0)
				m_PlayerPos.x = 0;
		}
		else
		{
			m_PlayerPos.x = m_rcScene.right / SCENE_MOVE_PIVOT - 100;
		}
	}

	(*m_Player)->SetPos(m_PlayerPos);
}

inline void CStageScene_0::UnitInitialize()
{
	for (int i = 0; i < STAGE_0_ENEMY_NUM; i++)
	{
		switch ((CEnemy::TypeOfEnemy)(rand() % (int)CEnemy::TypeOfEnemy::Count))
		{
		case CEnemy::TypeOfEnemy::SwordMan:
			m_EnemyList.push_front(new CEnemy(TEXT("Resource\\Graphics\\Sprite\\몬스터.png"), m_pSound, 23, 12, CEnemy::TypeOfEnemy::SwordMan));
			break;
		case CEnemy::TypeOfEnemy::Archer:
			m_EnemyList.push_front(new CEnemy(TEXT("Resource\\Graphics\\Sprite\\몬스터_아쳐.png"), m_pSound, 22, 12, CEnemy::TypeOfEnemy::Archer));
			break;
		}
		auto p = m_EnemyList.begin();

		switch ((*p)->GetType())
		{
		case CEnemy::TypeOfEnemy::SwordMan:
			(*p)->SetState(CEnemyAnimationSprite::ESpriteState::sprite_state_idle, 8);
			(*p)->SetState(CEnemyAnimationSprite::ESpriteState::sprite_state_move, 6, 0, 0, 0, 6);
			(*p)->SetState(CEnemyAnimationSprite::ESpriteState::sprite_state_attack, 7, 0, 3, 5, 7);
			(*p)->SetState(CEnemyAnimationSprite::ESpriteState::sprite_state_guard, 6, 0, 2, 2, 6);
			(*p)->SetState(CEnemyAnimationSprite::ESpriteState::sprite_state_hit, 10, 0, 5, 7, 10);
			(*p)->SetState(CEnemyAnimationSprite::ESpriteState::sprite_state_pop, 23, 0, 7, 15, 23);
		
			(*p)->SetStateDelay(CEnemyAnimationSprite::ESpriteState::sprite_state_idle, 5);
			(*p)->SetStateDelay(CEnemyAnimationSprite::ESpriteState::sprite_state_move, 5, 5, 3);
			(*p)->SetStateDelay(CEnemyAnimationSprite::ESpriteState::sprite_state_attack, 15, 1, 15);
			(*p)->SetStateDelay(CEnemyAnimationSprite::ESpriteState::sprite_state_guard, 1, 1, 10);
			(*p)->SetStateDelay(CEnemyAnimationSprite::ESpriteState::sprite_state_hit, 5, 5, 5);
			(*p)->SetStateDelay(CEnemyAnimationSprite::ESpriteState::sprite_state_pop, 3, 3, 3);
			break;
		case CEnemy::TypeOfEnemy::Archer:
			(*p)->SetState(CEnemyArcher_AnimationSprite::ESpriteState::sprite_state_idle, 5);
			(*p)->SetState(CEnemyArcher_AnimationSprite::ESpriteState::sprite_state_move, 6, 0, 0, 0, 6);
			(*p)->SetState(CEnemyArcher_AnimationSprite::ESpriteState::sprite_state_attack, 7, 0, 4, 6, 7);
			(*p)->SetState(CEnemyArcher_AnimationSprite::ESpriteState::sprite_state_shot, 9, 0, 5, 8, 9);
			(*p)->SetState(CEnemyArcher_AnimationSprite::ESpriteState::sprite_state_hit, 7, 0, 2, 3, 7);
			(*p)->SetState(CEnemyArcher_AnimationSprite::ESpriteState::sprite_state_pop, 22, 0, 7, 15, 22);
			
			(*p)->SetStateDelay(CEnemyArcher_AnimationSprite::ESpriteState::sprite_state_idle, 5);
			(*p)->SetStateDelay(CEnemyArcher_AnimationSprite::ESpriteState::sprite_state_move, 5, 5, 3);
			(*p)->SetStateDelay(CEnemyArcher_AnimationSprite::ESpriteState::sprite_state_attack, 15, 1, 15);
			(*p)->SetStateDelay(CEnemyArcher_AnimationSprite::ESpriteState::sprite_state_shot, 15, 1, 10);
			(*p)->SetStateDelay(CEnemyArcher_AnimationSprite::ESpriteState::sprite_state_hit, 5, 5, 5);
			(*p)->SetStateDelay(CEnemyArcher_AnimationSprite::ESpriteState::sprite_state_pop, 3, 3, 3);
			break;
		}
		(*p)->SetPos({ rand() % (m_cimgBackground.GetWidth() - 700) + 500, 350 });
	}
}

bool CStageScene_0::ProcessInput()
{
	bool bProcessedByScene = false;

	if (!bProcessedByScene)
	{
		static UCHAR pKeyBuffer[256];
		DWORD dwDirection = 0;

		bool bPushShift = false;
		bool bControll = false;

		if (GetKeyboardState(pKeyBuffer))
		{
			//			if (pKeyBuffer[VK_UP	] & 0xF0)	dwDirection	|= DIR_FORWARD;
			//			if (pKeyBuffer[VK_DOWN	] & 0xF0)	dwDirection |= DIR_BACKWARD;
			//			if (pKeyBuffer[VK_LEFT	] & 0xF0)	dwDirection |= DIR_LEFT;
			//			if (pKeyBuffer[VK_RIGHT	] & 0xF0)	dwDirection |= DIR_RIGHT;
			if (pKeyBuffer[VK_LSHIFT] & 0xF0)	  bPushShift = true;
			if (pKeyBuffer[VK_LEFT] & 0xF0)  	  dwDirection |= DIR_LEFT;
			if (pKeyBuffer[VK_RIGHT] & 0xF0)	  dwDirection |= DIR_RIGHT;

			if (pKeyBuffer['A'] & 0xF0) { bControll = true; (*m_Player)->Input(CPlayer::InputKey::A, bPushShift); }

			if ((pKeyBuffer['X'] & 0xF0) && (pKeyBuffer['C'] & 0xF0)) { bControll = true; (*m_Player)->Input(CPlayer::InputKey::X); }
			else if ((pKeyBuffer['X'] & 0xF0) || (pKeyBuffer['Z'] & 0xF0) || (pKeyBuffer['C'] & 0xF0) || (pKeyBuffer['V'] & 0xF0))
			{
				if (pKeyBuffer['Z'] & 0xF0) { bControll = true; (*m_Player)->Input(CPlayer::InputKey::Z, bPushShift); }
				if (pKeyBuffer['X'] & 0xF0) { bControll = true; (*m_Player)->Input(CPlayer::InputKey::X, bPushShift); }
				if (pKeyBuffer['C'] & 0xF0) { bControll = true; (*m_Player)->Input(CPlayer::InputKey::C, bPushShift); }
				if (pKeyBuffer['V'] & 0xF0) { bControll = true; (*m_Player)->Input(CPlayer::InputKey::V, bPushShift); }
			}
			else if ((pKeyBuffer[VK_LEFT] & 0xF0) || (pKeyBuffer[VK_RIGHT] & 0xF0))
			{
				if (dwDirection & DIR_LEFT) { bControll = true; (*m_Player)->Input(CPlayer::InputKey::MoveLeft, bPushShift); }
				if (dwDirection & DIR_RIGHT) { bControll = true; (*m_Player)->Input(CPlayer::InputKey::MoveRight, bPushShift); }
			}

			if ((pKeyBuffer[VK_SPACE] & 0xF0) || (pKeyBuffer[VK_UP] & 0xF0))
			{
				bControll = true; (*m_Player)->Input(CPlayer::InputKey::Jump, bPushShift);
			}
		}

		if (bControll == false)
			(*m_Player)->Input(CPlayer::InputKey::KeyUp);

		return bControll;
	}
	return false;
}
