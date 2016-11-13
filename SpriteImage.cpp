#include "stdafx.h"
#include "SpriteImage.h"
#include <iostream>
#include <cassert>

using namespace std;

CSpriteImage::CSpriteImage(LPCTSTR path, int w, int h)
{

	m_nFrameXNum = w;
	m_nFrameYNum = h;

	// 절대 발생하면 안 되는 코드이기 때문에 발생 시 강제로 프로그램을 종료
	if (FAILED(m_ciSpriteImage.Load(path)))
		assert(!"이미지가 정상적으로 로드되지 않았습니다!");


	m_nFrameWidthSize = m_ciSpriteImage.GetWidth() / m_nFrameXNum;
	m_nFrameHeightSize = m_ciSpriteImage.GetHeight() / m_nFrameYNum;

	m_colTransparent = ::GetPixel(m_ciSpriteImage.GetDC(), 0, 0);
}
CSpriteImage::~CSpriteImage()
{
	if (!m_ciSpriteImage.IsNull())
		m_ciSpriteImage.Destroy();
}

void CSpriteImage::OnUpdate()
{
	m_ptCurrentAnimationState.x++;

	if (m_ptCurrentAnimationState.x >= m_nFrameXNum)
	{
		m_ptCurrentAnimationState.x = 0;
		m_ptCurrentAnimationState.y++;
	}

	if (m_ptCurrentAnimationState.y >= m_nFrameYNum)
	{
		m_ptCurrentAnimationState.y = 0;
	}
}
void CSpriteImage::Draw(HDC hDC, int x, int y)
{
	m_ciSpriteImage.TransparentBlt(hDC,
		x, y,
		m_nFrameWidthSize, m_nFrameHeightSize,
		m_ptCurrentAnimationState.x * m_nFrameWidthSize,
		m_ptCurrentAnimationState.y * m_nFrameHeightSize,
		m_nFrameWidthSize, m_nFrameHeightSize,
		m_colTransparent
	);
}
void CSpriteImage::Draw(HDC hDC, int x, int y, int w, int h)
{
	m_ciSpriteImage.TransparentBlt(hDC,
		x, y, w, h,
		m_ptCurrentAnimationState.x * m_nFrameWidthSize,
		m_ptCurrentAnimationState.y * m_nFrameHeightSize,
		m_nFrameWidthSize, m_nFrameHeightSize,
		RGB(255, 255, 255)
	);
}


CAnimationSprite::CAnimationSprite(LPCTSTR path, int w, int h) : CSpriteImage(path, w, h)
{
}
CAnimationSprite::~CAnimationSprite()
{
	for (int i = 0; i < SpriteState::sprite_state_count; i++)
	{
		m_iMaxState[i];
		int j;
		for (j = 0; j < 2; j++)
		{
			m_iStateExcute[i][j] = 0;
			m_iStateAction[i][j] = 0;
			m_iStateExit[i][j] = 0;
			m_iStateDelay[i][j] = 0;
		}
		m_iStateDelay[i][j] = 0;
	}
	if (!m_ciSpriteImage.IsNull())
		m_ciSpriteImage.Destroy();
}

void CAnimationSprite::SetState(SpriteState s, int val)
{
	m_iMaxState[s] = val;

	m_iStateExcute[s][0] = 0;
	m_iStateExcute[s][1] = 0;
	m_iStateExit[s][0] = val;
	m_iStateExit[s][1] = val;
	m_iStateAction[s][0] = 0;
	m_iStateAction[s][1] = val;
}
void CAnimationSprite::SetState(SpriteState s, int val, int excute_start, int excute_end, int exit_start, int exit_end)
{
	m_iMaxState[s] = val;

	m_iStateExcute[s][0] = excute_start;
	m_iStateExcute[s][1] = excute_end;
	m_iStateExit[s][0] = exit_start;
	m_iStateExit[s][1] = exit_end;
	m_iStateAction[s][0] = excute_end;
	m_iStateAction[s][1] = exit_start;
}

void CAnimationSprite::SetDirection(bool bLeft)
{
	// 방향이 변경됐는지 확인
	bool VaildChange = (m_bLeft != bLeft);
	m_bLeft = bLeft;

	// 점프 중에는 변경 생략
	if (SpriteState::sprite_state_jump == m_stCurrent) return;
	// 공격 중에는 변경 생략
	if (SpriteState::sprite_state_attack_0 == m_stCurrent) return;
	if (SpriteState::sprite_state_attack_1 == m_stCurrent) return;
	if (SpriteState::sprite_state_attack_2 == m_stCurrent) return;
	if (SpriteState::sprite_state_attack_3 == m_stCurrent) return;
	if (SpriteState::sprite_state_parrying == m_stCurrent) return;
	if (SpriteState::sprite_state_shot == m_stCurrent) return;
	// 피격 중에는 상태 갱신 생략
	if (SpriteState::sprite_state_hit == m_stCurrent) return;
	// 가드 중에는 상태 갱신 생략
	if (SpriteState::sprite_state_guard == m_stCurrent) return;
	// 대쉬 중에는 상태 갱신 생략
	if (SpriteState::sprite_state_dash == m_stCurrent) return;

	// 변경되지 않은 경우 후처리 생략
	if (!VaildChange) return;

	// 딜레이 카운터 초기화
	m_nCount = 0;
	// 진입 상태로 변경
	m_ptCurrentAnimationState.x = m_iStateExcute[m_stCurrent][0];
	m_ptCurrentAnimationState.y = m_stCurrent * 2 + (m_bLeft ? 1 : 0);
}

void CAnimationSprite::UpdateState(SpriteState s)
{
	// 현재 상태와 동일하면 생략
	if (s == m_stCurrent) return;

	// 피격 중에는 상태 갱신 생략
	if (SpriteState::sprite_state_hit == m_stCurrent ) return;
	// 대쉬 중에는 상태 갱신 생략
	if (SpriteState::sprite_state_dash == m_stCurrent ) return;
	// 점프 중에는 상태 갱신 생략
	if ((SpriteState::sprite_state_jump == m_stCurrent && s != SpriteState::sprite_state_jump_attack) && s != SpriteState::sprite_state_hit) return;
	// 공격 중에는 상태 갱신 생략
	if (SpriteState::sprite_state_jump_attack == m_stCurrent && s != SpriteState::sprite_state_hit) return;
	if (SpriteState::sprite_state_parrying == m_stCurrent && s != SpriteState::sprite_state_hit) return;
	if (SpriteState::sprite_state_shot == m_stCurrent && s != SpriteState::sprite_state_hit ) return;
	if ((SpriteState::sprite_state_attack_0 == m_stCurrent && s != SpriteState::sprite_state_attack_1) && s != SpriteState::sprite_state_hit ) return;
	if ((SpriteState::sprite_state_attack_1 == m_stCurrent && s != SpriteState::sprite_state_attack_2) && s != SpriteState::sprite_state_hit ) return;
	if ((SpriteState::sprite_state_attack_2 == m_stCurrent && s != SpriteState::sprite_state_attack_3) && s != SpriteState::sprite_state_hit ) return;
	if ((SpriteState::sprite_state_attack_3 == m_stCurrent && s != SpriteState::sprite_state_attack_0) && s != SpriteState::sprite_state_hit ) return;

	// 현재 상태가 idle 이 아니거나
	// 현재 진행중인 상태가 진입 상태일 경우
	// 퇴장 상태로 변경하지 않고 갱신
	if (s != SpriteState::sprite_state_idle || GetCurrentStateState() == StateState::state_state_excute)
	{
		m_stCurrent = s;
		m_ptCurrentAnimationState.x = m_iStateExcute[m_stCurrent][0];
		m_ptCurrentAnimationState.y = m_stCurrent * 2 + (m_bLeft ? 1 : 0);
	}
	// 퇴장 상태로 갱신
	else
	{
		// 이미 퇴장중이면 취소
		if (GetCurrentStateState() == StateState::state_state_exit) return;
		m_stNext = s;
		m_ptCurrentAnimationState.x = m_iStateExit[m_stCurrent][0];
	}

	// 딜레이 카운터 초기화
	m_nCount = 0;

}

CAnimationSprite::StateState CAnimationSprite::GetCurrentStateState() const
{
	// 진입 상태로 초기화
	StateState State_state = state_state_excute;
	// 진행 상태보다 많이 진행하고 있는 경우
	if (m_ptCurrentAnimationState.x >= m_iStateAction[m_stCurrent][1]) State_state = state_state_exit;
	// 진행상태일 경우
	else if (m_ptCurrentAnimationState.x >= m_iStateAction[m_stCurrent][0]) State_state = state_state_action;
	return State_state;
}
void CAnimationSprite::OnUpdate()
{
	// excute - action - exit
	StateState State_state = GetCurrentStateState();

	// 딜레이 카운터 갱신
	(++m_nCount) %= (m_iStateDelay[m_stCurrent][State_state]);
	// 딜레이 카운터로 갱신 여부 확인
	if (m_nCount) return;


	// 이미지 갱신
	(++m_ptCurrentAnimationState.x) %= m_iStateExit[m_stCurrent][1];

	//초기화는
	if (0 == m_ptCurrentAnimationState.x)
	{
		// 다음 상태로 변경
		if (m_stCurrent == SpriteState::sprite_state_jump_attack)
		{
			m_stCurrent = SpriteState::sprite_state_jump;
			m_ptCurrentAnimationState.x = m_iStateExcute[m_stCurrent][0];
			m_ptCurrentAnimationState.y = (SpriteState::sprite_state_jump * 2) + (m_bLeft ? 1 : 0);
		}
		else
		{
			m_stCurrent = m_stNext;
			m_ptCurrentAnimationState.x = m_iStateExcute[m_stCurrent][0];
			m_ptCurrentAnimationState.y = m_stCurrent * 2 + (m_bLeft ? 1 : 0);
		}
	}

	// 이미지가 초기화 상태라면
	if (m_ptCurrentAnimationState.x)					return;
	
	// idle 상태로 초기화
	m_stCurrent = SpriteState::sprite_state_idle;
	m_ptCurrentAnimationState.y = (SpriteState::sprite_state_idle * 2) + (m_bLeft ? 1 : 0);
}

void CAnimationSprite::Draw(HDC hDC, int x, int y)
{
	m_ciSpriteImage.TransparentBlt(hDC,
		// Draw Start pt
		x, y,
		// Draw Size
		m_nFrameWidthSize, m_nFrameHeightSize,
		// Image Start pt.x
		m_ptCurrentAnimationState.x * m_nFrameWidthSize,
		// Image Start pt.y
		m_ptCurrentAnimationState.y * m_nFrameHeightSize,
		// Image Size
		m_nFrameWidthSize, m_nFrameHeightSize,
		// Transparent RGB
		m_colTransparent
	);

}


CEnemyAnimationSprite::CEnemyAnimationSprite(LPCTSTR path, int w, int h) : CSpriteImage(path, w, h)
{
}

CEnemyAnimationSprite::~CEnemyAnimationSprite()
{
	for (int i = 0; i < ESpriteState::sprite_state_count; i++)
	{
		m_iMaxState[i];
		int j;
		for (j = 0; j < 2; j++)
		{
			m_iStateExcute[i][j] = 0;
			m_iStateAction[i][j] = 0;
			m_iStateExit[i][j] = 0;
			m_iStateDelay[i][j] = 0;
		}
		m_iStateDelay[i][j] = 0;
	}
	if (!m_ciSpriteImage.IsNull())
		m_ciSpriteImage.Destroy();
}
void CEnemyAnimationSprite::SetState(ESpriteState s, int val)
{
	m_iMaxState[s] = val;

	m_iStateExcute[s][0] = 0;
	m_iStateExcute[s][1] = 0;
	m_iStateExit[s][0] = val;
	m_iStateExit[s][1] = val;
	m_iStateAction[s][0] = 0;
	m_iStateAction[s][1] = val;
}
void CEnemyAnimationSprite::SetState(ESpriteState s, int val, int excute_start, int excute_end, int exit_start, int exit_end)
{
	m_iMaxState[s] = val;

	m_iStateExcute[s][0] = excute_start;
	m_iStateExcute[s][1] = excute_end;
	m_iStateExit[s][0] = exit_start;
	m_iStateExit[s][1] = exit_end;
	m_iStateAction[s][0] = excute_end;
	m_iStateAction[s][1] = exit_start;
}
void CEnemyAnimationSprite::SetDirection(bool bLeft)
{
	// 공격 중에는 방향 갱신 생략
	if (ESpriteState::sprite_state_attack == m_stCurrent) return;

	if (ESpriteState::sprite_state_hit == m_stCurrent) return;
	if (ESpriteState::sprite_state_pop == m_stCurrent) return;

	// 방향이 변경됐는지 확인
	bool VaildChange = (m_bLeft != bLeft);
	m_bLeft = bLeft;

	// 변경되지 않은 경우 후처리 생략
	if (!VaildChange) return;

	// 딜레이 카운터 초기화
	m_nCount = 0;
	// 진입 상태로 변경
	m_ptCurrentAnimationState.x = m_iStateExcute[m_stCurrent][0];
	m_ptCurrentAnimationState.y = m_stCurrent * 2 + (m_bLeft ? 1 : 0);
}

void CEnemyAnimationSprite::UpdateState(ESpriteState s)
{
	// 현재 상태와 동일하면 생략
	if (s == m_stCurrent) return;

	// 공격 중에는 상태 갱신 생략
	if (ESpriteState::sprite_state_attack == m_stCurrent && ESpriteState::sprite_state_guard != s && ESpriteState::sprite_state_hit != s) return;
	// 피격 중에는 상태 갱신 생략
	if (ESpriteState::sprite_state_guard == m_stCurrent && ESpriteState::sprite_state_hit != s ) return;
	if (ESpriteState::sprite_state_hit == m_stCurrent && ESpriteState::sprite_state_idle == s)
	{
		m_stCurrent = s;
		m_ptCurrentAnimationState.x = m_iStateExcute[m_stCurrent][0];
		m_ptCurrentAnimationState.y = m_stCurrent * 2 + (m_bLeft ? 1 : 0);
		return;
	}
	if (ESpriteState::sprite_state_hit == m_stCurrent) return;

	// 현재 상태가 idle 이 아니고
	// 현재 진행중인 상태가 진입 상태일 경우
	// 퇴장 상태로 변경하지 않고 갱신
	if (ESpriteState::sprite_state_idle != s || ESpriteState::sprite_state_hit == s)
	{
		m_stCurrent = s;
		m_ptCurrentAnimationState.x = m_iStateExcute[m_stCurrent][0];
		m_ptCurrentAnimationState.y = m_stCurrent * 2 + (m_bLeft ? 1 : 0);
	}
	// 퇴장 상태로 갱신
	else
	{
		// 이미 퇴장중이면 취소
		if (GetCurrentStateState() == StateState::state_state_exit) return;
		m_stNext = s;
		m_ptCurrentAnimationState.x = m_iStateExit[m_stCurrent][0];
	}

	// 딜레이 카운터 초기화
	m_nCount = 0;

}

CEnemyAnimationSprite::StateState CEnemyAnimationSprite::GetCurrentStateState() const
{
	// 진입 상태로 초기화
	StateState State_state = state_state_excute;
	// 진행 상태보다 많이 진행하고 있는 경우
	if (m_ptCurrentAnimationState.x >= m_iStateAction[m_stCurrent][1]) State_state = state_state_exit;
	// 진행상태일 경우
	else if (m_ptCurrentAnimationState.x >= m_iStateAction[m_stCurrent][0]) State_state = state_state_action;
	return State_state;
}
void CEnemyAnimationSprite::OnUpdate(bool reset)
{
	if (reset)
	{
		m_stCurrent = ESpriteState::sprite_state_pop;
		m_ptCurrentAnimationState.x = m_iStateExcute[m_stCurrent][0];
		m_ptCurrentAnimationState.y = (ESpriteState::sprite_state_pop * 2) + (m_bLeft ? 1 : 0);
		return;
	}
	// excute - action - exit
	StateState State_state = GetCurrentStateState();

	// 딜레이 카운터 갱신
	(++m_nCount) %= (m_iStateDelay[m_stCurrent][State_state]);
	// 딜레이 카운터로 갱신 여부 확인
	if (m_nCount) return;
	if (!m_Build)
	{
		m_bLeft = true;
		m_stCurrent = ESpriteState::sprite_state_pop;
		m_ptCurrentAnimationState.x = 0;
		m_ptCurrentAnimationState.y = m_stCurrent * 2 + (m_bLeft ? 1 : 0);;
		m_Build = true;
	}
	// 이미지 갱신
	(++m_ptCurrentAnimationState.x) %= m_iStateExit[m_stCurrent][1];
	//초기화는
	if (0 == m_ptCurrentAnimationState.x)
	{
		// 다음 상태로 변경
		m_stCurrent = m_stNext;
		m_ptCurrentAnimationState.x = m_iStateExcute[m_stCurrent][0];
		m_ptCurrentAnimationState.y = m_stCurrent * 2 + (m_bLeft ? 1 : 0);
	}

	// 이미지가 초기화 상태라면
	if (m_ptCurrentAnimationState.x)					return;
	// idle 상태로 초기화
	if (m_Build) {
		m_stCurrent = ESpriteState::sprite_state_idle;
		m_ptCurrentAnimationState.y = (ESpriteState::sprite_state_idle * 2) + (m_bLeft ? 1 : 0);
	}
}
void CEnemyAnimationSprite::Draw(HDC hDC, int x, int y)
{
	m_ciSpriteImage.TransparentBlt(hDC,
		// Draw Start pt
		x, y,
		// Draw Size
		m_nFrameWidthSize, m_nFrameHeightSize,
		// Image Start pt.x
		m_ptCurrentAnimationState.x * m_nFrameWidthSize,
		// Image Start pt.y
		m_ptCurrentAnimationState.y * m_nFrameHeightSize,
		// Image Size
		m_nFrameWidthSize, m_nFrameHeightSize,
		// Transparent RGB
		m_colTransparent
	);

}
CEnemyArcher_AnimationSprite::CEnemyArcher_AnimationSprite(LPCTSTR path, int w, int h) : CSpriteImage(path, w, h)
{
}

CEnemyArcher_AnimationSprite::~CEnemyArcher_AnimationSprite()
{
	for (int i = 0; i < ESpriteState::sprite_state_count; i++)
	{
		m_iMaxState[i];
		int j;
		for (j = 0; j < 2; j++)
		{
			m_iStateExcute[i][j] = 0;
			m_iStateAction[i][j] = 0;
			m_iStateExit[i][j] = 0;
			m_iStateDelay[i][j] = 0;
		}
		m_iStateDelay[i][j] = 0;
	}
	if (!m_ciSpriteImage.IsNull())
		m_ciSpriteImage.Destroy();
}
void CEnemyArcher_AnimationSprite::SetState(ESpriteState s, int val)
{
	m_iMaxState[s] = val;

	m_iStateExcute[s][0] = 0;
	m_iStateExcute[s][1] = 0;
	m_iStateExit[s][0] = val;
	m_iStateExit[s][1] = val;
	m_iStateAction[s][0] = 0;
	m_iStateAction[s][1] = val;
}
void CEnemyArcher_AnimationSprite::SetState(ESpriteState s, int val, int excute_start, int excute_end, int exit_start, int exit_end)
{
	m_iMaxState[s] = val;

	m_iStateExcute[s][0] = excute_start;
	m_iStateExcute[s][1] = excute_end;
	m_iStateExit[s][0] = exit_start;
	m_iStateExit[s][1] = exit_end;
	m_iStateAction[s][0] = excute_end;
	m_iStateAction[s][1] = exit_start;
}
void CEnemyArcher_AnimationSprite::SetDirection(bool bLeft)
{
	if (ESpriteState::sprite_state_hit == m_stCurrent) return;
	if (ESpriteState::sprite_state_pop == m_stCurrent) return;

	// 방향이 변경됐는지 확인
	bool VaildChange = (m_bLeft != bLeft);
	m_bLeft = bLeft;

	// 변경되지 않은 경우 후처리 생략
	if (!VaildChange) return;

	// 딜레이 카운터 초기화
	m_nCount = 0;
	// 진입 상태로 변경
	m_ptCurrentAnimationState.x = m_iStateExcute[m_stCurrent][0];
	m_ptCurrentAnimationState.y = m_stCurrent * 2 + (m_bLeft ? 1 : 0);
}

void CEnemyArcher_AnimationSprite::UpdateState(ESpriteState s)
{
	// 현재 상태와 동일하면 생략
	if (s == m_stCurrent && ESpriteState::sprite_state_hit != s) return;

	// 공격 중에는 상태 갱신 생략
	if (ESpriteState::sprite_state_attack == m_stCurrent && ESpriteState::sprite_state_hit != s) return;
	// 피격 중에는 상태 갱신 생략
	if (ESpriteState::sprite_state_shot == m_stCurrent && ESpriteState::sprite_state_hit != s) return;
	if (ESpriteState::sprite_state_hit == m_stCurrent && ESpriteState::sprite_state_idle == s)
	{
		m_stCurrent = s;
		m_ptCurrentAnimationState.x = m_iStateExcute[m_stCurrent][0];
		m_ptCurrentAnimationState.y = m_stCurrent * 2 + (m_bLeft ? 1 : 0);
		return;
	}
	if (ESpriteState::sprite_state_hit == m_stCurrent) return;

	// 현재 상태가 idle 이 아니고
	// 현재 진행중인 상태가 진입 상태일 경우
	// 퇴장 상태로 변경하지 않고 갱신
	if (ESpriteState::sprite_state_idle != s || ESpriteState::sprite_state_hit == s)
	{
		m_stCurrent = s;
		m_ptCurrentAnimationState.x = m_iStateExcute[m_stCurrent][0];
		m_ptCurrentAnimationState.y = m_stCurrent * 2 + (m_bLeft ? 1 : 0);
	}
	// 퇴장 상태로 갱신
	else
	{
		// 이미 퇴장중이면 취소
		if (GetCurrentStateState() == StateState::state_state_exit) return;
		m_stNext = s;
		m_ptCurrentAnimationState.x = m_iStateExit[m_stCurrent][0];
	}

	// 딜레이 카운터 초기화
	m_nCount = 0;

}

CEnemyArcher_AnimationSprite::StateState CEnemyArcher_AnimationSprite::GetCurrentStateState() const
{
	// 진입 상태로 초기화
	StateState State_state = state_state_excute;
	// 진행 상태보다 많이 진행하고 있는 경우
	if (m_ptCurrentAnimationState.x >= m_iStateAction[m_stCurrent][1]) State_state = state_state_exit;
	// 진행상태일 경우
	else if (m_ptCurrentAnimationState.x >= m_iStateAction[m_stCurrent][0]) State_state = state_state_action;
	return State_state;
}
void CEnemyArcher_AnimationSprite::OnUpdate(bool reset)
{
	if (reset)
	{
		m_stCurrent = ESpriteState::sprite_state_pop;
		m_ptCurrentAnimationState.x = m_iStateExcute[m_stCurrent][0];
		m_ptCurrentAnimationState.y = (ESpriteState::sprite_state_pop * 2) + (m_bLeft ? 1 : 0);
		return;
	}
	// excute - action - exit
	StateState State_state = GetCurrentStateState();

	// 딜레이 카운터 갱신
	(++m_nCount) %= (m_iStateDelay[m_stCurrent][State_state]);
	// 딜레이 카운터로 갱신 여부 확인
	if (m_nCount) return;
	if (!m_Build)
	{
		m_bLeft = true;
		m_stCurrent = ESpriteState::sprite_state_pop;
		m_ptCurrentAnimationState.x = 0;
		m_ptCurrentAnimationState.y = m_stCurrent * 2 + (m_bLeft ? 1 : 0);;
		m_Build = true;
	}
	// 이미지 갱신
	(++m_ptCurrentAnimationState.x) %= m_iStateExit[m_stCurrent][1];
	//초기화는
	if (0 == m_ptCurrentAnimationState.x)
	{
		// 다음 상태로 변경
		m_stCurrent = m_stNext;
		m_ptCurrentAnimationState.x = m_iStateExcute[m_stCurrent][0];
		m_ptCurrentAnimationState.y = m_stCurrent * 2 + (m_bLeft ? 1 : 0);
	}

	// 이미지가 초기화 상태라면
	if (m_ptCurrentAnimationState.x)					return;
	// idle 상태로 초기화
	if (m_Build) {
		m_stCurrent = ESpriteState::sprite_state_idle;
		m_ptCurrentAnimationState.y = (ESpriteState::sprite_state_idle * 2) + (m_bLeft ? 1 : 0);
	}
}
void CEnemyArcher_AnimationSprite::Draw(HDC hDC, int x, int y)
{
	m_ciSpriteImage.TransparentBlt(hDC,
		// Draw Start pt
		x, y,
		// Draw Size
		m_nFrameWidthSize, m_nFrameHeightSize,
		// Image Start pt.x
		m_ptCurrentAnimationState.x * m_nFrameWidthSize,
		// Image Start pt.y
		m_ptCurrentAnimationState.y * m_nFrameHeightSize,
		// Image Size
		m_nFrameWidthSize, m_nFrameHeightSize,
		// Transparent RGB
		m_colTransparent
	);
}


CBOSSAnimationSprite::CBOSSAnimationSprite(LPCTSTR path, int w, int h) : CSpriteImage(path, w, h)
{
}
CBOSSAnimationSprite::~CBOSSAnimationSprite()
{
	for (int i = 0; i < SpriteState::sprite_state_count; i++)
	{
		m_iMaxState[i];
		int j;
		for (j = 0; j < 2; j++)
		{
			m_iStateExcute[i][j] = 0;
			m_iStateAction[i][j] = 0;
			m_iStateExit[i][j] = 0;
			m_iStateDelay[i][j] = 0;
		}
		m_iStateDelay[i][j] = 0;
	}
	if (!m_ciSpriteImage.IsNull())
		m_ciSpriteImage.Destroy();
}

void CBOSSAnimationSprite::SetState(SpriteState s, int val)
{
	m_phase = 1;
	m_iMaxState[s] = val;

	m_iStateExcute[s][0] = 0;
	m_iStateExcute[s][1] = 0;
	m_iStateExit[s][0] = val;
	m_iStateExit[s][1] = val;
	m_iStateAction[s][0] = 0;
	m_iStateAction[s][1] = val;
}
void CBOSSAnimationSprite::SetState(SpriteState s, int val, int excute_start, int excute_end, int exit_start, int exit_end)
{
	m_phase = 1;
	m_iMaxState[s] = val;

	m_iStateExcute[s][0] = excute_start;
	m_iStateExcute[s][1] = excute_end;
	m_iStateExit[s][0] = exit_start;
	m_iStateExit[s][1] = exit_end;
	m_iStateAction[s][0] = excute_end;
	m_iStateAction[s][1] = exit_start;
}
void CBOSSAnimationSprite::SetDirection(bool bLeft)
{
	// 방향이 변경됐는지 확인
	bool VaildChange = (m_bLeft != bLeft);
	m_bLeft = bLeft;

	// 행동 중에는 변경 생략
	if ((SpriteState::sprite_state_phase1_idle != m_stCurrent)&&
		(SpriteState::sprite_state_phase2_idle != m_stCurrent)&&
		(SpriteState::sprite_state_phase3_idle != m_stCurrent)) return;
	// 변경되지 않은 경우 후처리 생략
	if (!VaildChange) return;

	// 딜레이 카운터 초기화
	m_nCount = 0;
	// 진입 상태로 변경
	m_ptCurrentAnimationState.x = m_iStateExcute[m_stCurrent][0];
	m_ptCurrentAnimationState.y = m_stCurrent * 2 + (m_bLeft ? 1 : 0);
}

void CBOSSAnimationSprite::UpdateState(SpriteState s)
{
	// 행동 상태라면 생략
	if (SpriteState::sprite_state_phase2_shift == m_stCurrent) return;
	if (SpriteState::sprite_state_phase3_shift == m_stCurrent) return;
	if ((SpriteState::sprite_state_phase3_dash_ready == m_stCurrent || SpriteState::sprite_state_phase3_dash == m_stCurrent)
		&& s != SpriteState::sprite_state_phase3_die) return;
	if (SpriteState::sprite_state_phase3_attack == m_stCurrent && s != SpriteState::sprite_state_phase3_idle 
		&& s != SpriteState::sprite_state_phase3_die) return;
	if (SpriteState::sprite_state_phase3_die == m_stCurrent) return;


	// 현재 상태와 동일하면 생략
	if (s == m_stCurrent) return;

	// 현재 상태가 idle 이 아니고
	// 현재 진행중인 상태가 진입 상태일 경우
	// 퇴장 상태로 변경하지 않고 갱신
	if ((
		//SpriteState::sprite_state_phase1_idle != s && 
		SpriteState::sprite_state_phase2_idle != s && 
		SpriteState::sprite_state_phase3_idle != s)
		|| GetCurrentStateState() == StateState::state_state_excute)
	{
		m_stCurrent = s;
		m_ptCurrentAnimationState.x = m_iStateExcute[m_stCurrent][0];
		m_ptCurrentAnimationState.y = m_stCurrent * 2 + (m_bLeft ? 1 : 0);
	}
	// 퇴장 상태로 갱신
	else
	{
		// 이미 퇴장중이면 취소
		if (GetCurrentStateState() == StateState::state_state_exit) return;
		m_stNext = s;
		m_ptCurrentAnimationState.x = m_iStateExit[m_stCurrent][0];
	}

	// 딜레이 카운터 초기화
	m_nCount = 0;

}

CBOSSAnimationSprite::StateState CBOSSAnimationSprite::GetCurrentStateState() const
{
	// 진입 상태로 초기화
	StateState State_state = state_state_excute;
	// 진행 상태보다 많이 진행하고 있는 경우
	if (m_ptCurrentAnimationState.x >= m_iStateAction[m_stCurrent][1]) State_state = state_state_exit;
	// 진행상태일 경우
	else if (m_ptCurrentAnimationState.x >= m_iStateAction[m_stCurrent][0]) State_state = state_state_action;
	return State_state;
}
void CBOSSAnimationSprite::OnUpdate(bool reset)
{
	if (reset)
	{
		m_stCurrent = SpriteState::sprite_state_phase1_idle;
		m_ptCurrentAnimationState.x = m_iStateExcute[m_stCurrent][0];
		m_ptCurrentAnimationState.y = (SpriteState::sprite_state_phase1_idle * 2) + (m_bLeft ? 1 : 0);
		return;
	}
	// excute - action - exit
	StateState State_state = GetCurrentStateState();

	// 딜레이 카운터 갱신
	(++m_nCount) %= (m_iStateDelay[m_stCurrent][State_state]);
	// 딜레이 카운터로 갱신 여부 확인
	if (m_nCount) return;

		// 이미지 갱신
	(++m_ptCurrentAnimationState.x) %= m_iStateExit[m_stCurrent][1];
	//초기화는
	//if (0 == m_ptCurrentAnimationState.x)
	//{
	//	// 다음 상태로 변경
	//	m_stCurrent = m_stNext;
	//	m_ptCurrentAnimationState.x = m_iStateExcute[m_stCurrent][0];
	//	m_ptCurrentAnimationState.y = m_stCurrent * 2 + (m_bLeft ? 1 : 0);
	//}

	// 이미지가 초기화 상태라면
	if (m_ptCurrentAnimationState.x)					return;

	// 대쉬일 경우 다음 동작으로
	if (m_stCurrent == SpriteState::sprite_state_phase3_dash_ready)
	{
		m_stCurrent = SpriteState::sprite_state_phase3_dash;
		m_ptCurrentAnimationState.y = (SpriteState::sprite_state_phase3_dash * 2) + (m_bLeft ? 1 : 0);
		return;
	}

	// idle 상태로 초기화
	if (m_stCurrent < SpriteState::sprite_state_phase2_shift)
	{
		m_stCurrent = SpriteState::sprite_state_phase1_idle;
		m_ptCurrentAnimationState.y = (SpriteState::sprite_state_phase1_idle * 2) + (m_bLeft ? 1 : 0);
		return;
	}
	if (m_stCurrent >= SpriteState::sprite_state_phase2_shift && m_stCurrent < SpriteState::sprite_state_phase3_shift)
	{
		m_stCurrent = SpriteState::sprite_state_phase2_idle;
		m_ptCurrentAnimationState.y = (SpriteState::sprite_state_phase2_idle * 2) + (m_bLeft ? 1 : 0);
		return;
	}
	if (m_stCurrent >= SpriteState::sprite_state_phase3_shift)
	{
		m_stCurrent = SpriteState::sprite_state_phase3_idle;
		m_ptCurrentAnimationState.y = (SpriteState::sprite_state_phase3_idle * 2) + (m_bLeft ? 1 : 0);
		return;
	}
}

void CBOSSAnimationSprite::Draw(HDC hDC, int x, int y)
{
	m_ciSpriteImage.TransparentBlt(hDC,
		// Draw Start pt
		x, y,
		// Draw Size
		m_nFrameWidthSize, m_nFrameHeightSize,
		// Image Start pt.x
		m_ptCurrentAnimationState.x * m_nFrameWidthSize,
		// Image Start pt.y
		m_ptCurrentAnimationState.y * m_nFrameHeightSize,
		// Image Size
		m_nFrameWidthSize, m_nFrameHeightSize,
		// Transparent RGB
		m_colTransparent
	);
}