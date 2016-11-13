#pragma once

#include <windows.h>
#include <atlImage.h>

class CSpriteImage
{

protected:

	CImage m_ciSpriteImage;

	int m_nFrameXNum;
	int m_nFrameYNum;

	int m_nFrameWidthSize;
	int m_nFrameHeightSize;

	POINT m_ptCurrentAnimationState{ 0,0 };

	COLORREF m_colTransparent;

	bool m_Build{ false };
public:
	CSpriteImage(LPCTSTR path, int w, int h);
	virtual ~CSpriteImage();
	
	virtual void OnUpdate();

	virtual void Draw(HDC hDC, int x, int y);
	virtual void Draw(HDC hDC, int x, int y, int w, int h);
};


/*
	class CAnimationSprite
	- 횡스크롤(좌, 우) 기준 애니메이션 스프라이트 클래스

	- 상태를 정의하여 이미지를 상태에 따라 처리할 수 있게 구현
	- 이미지는 홀수 번째 : 오른쪽 방향 / 짝수 번째 : 왼쪽 방향으로 배치 필요
	- 상태 진입 상태, 상태 진행 상태, 상태 퇴장 상태에 따른 상세 구현이 가능
	- 단순히 진행 - 퇴장을 구현하지 않아도 사용 가능
	
*/
class CAnimationSprite : public CSpriteImage
{
public:
	// Sprite State : 캐릭터의 현재 상태
	// 필요에 따른 수정 필요
	typedef enum {
		  sprite_state_idle = 0
		, sprite_state_move
		, sprite_state_jump
		, sprite_state_attack_0
		, sprite_state_attack_1
		, sprite_state_attack_2
		, sprite_state_attack_3
		, sprite_state_dash
		, sprite_state_jump_attack
		, sprite_state_guard
		, sprite_state_parrying
		, sprite_state_hit
		, sprite_state_shot
		, sprite_state_count
	}SpriteState;

	// 상태의 상태(진입/진행/퇴장)
	// 수정하지 마시오
	typedef enum {
		  state_state_excute = 0
		, state_state_action
		, state_state_exit
		
	}StateState;

protected:

	// 상태에 따른 전체 이미지 수
	int m_iMaxState		[SpriteState::sprite_state_count];
	// 상태에 따른 각 이미지의 애니메이션 딜레이
	// 진입 - 진행 - 퇴장 순으로 사용
	// 진입-퇴장 상태가 없으면 0
	int m_iStateDelay	[SpriteState::sprite_state_count][3];

	// 상태에 따른 진입 상태의 시작과 끝 이미지 번호
	int m_iStateExcute	[SpriteState::sprite_state_count][2];
	// 상태에 따른 진행 상태의 시작과 끝 이미지 번호
	int m_iStateAction	[SpriteState::sprite_state_count][2];
	// 상태에 따른 퇴장 상태의 시작과 끝 이미지 번호
	int m_iStateExit	[SpriteState::sprite_state_count][2];

	// 현재 상태
	SpriteState m_stCurrent = sprite_state_idle;
	// 다음 상태 : 퇴장 상태 진행 시 저장했다가 퇴장 상태 종료 후 현재 상태로 변경
	SpriteState m_stNext = sprite_state_idle;

	// 딜레이 카운터
	// m_iStateDelay에 저장된 값만큼 Update를 지연
	// Update에 비해 느린 애니메이션 구현 가능 
	// = 적은 이미지로 비교적 부드러운 애니메이션 효과 연출 가능
	int m_nCount = 0;

	// 현재 방향
	bool m_bLeft = false;
public:

	CAnimationSprite(LPCTSTR path, int w, int h);
	virtual ~CAnimationSprite();

	// State 값 갱신 : 진행 상태의 이미지 번호 갱신. 진입 - 퇴장 상태가 없을 때 사용
	void SetState(SpriteState s, int val);
	// State 값 갱신 : 모든 상태의 이미지 번호 갱신. 진행 상태는 진입 - 퇴장 상태의 값에 따라 결정
	void SetState(SpriteState s, int val, int excute_start, int excute_end, int exit_start, int exit_end);
	
	// Delay 값 갱신 : 진입 - 진행 - 퇴장 상태의 Delay 값 갱신
	void SetStateDelay(SpriteState s, int excute, int action, int exit)
	{ m_iStateDelay[s][0] = excute; m_iStateDelay[s][1] = action; m_iStateDelay[s][2] = exit; }
	// Delay 값 갱신 : 진행 상태의 Delay 값 갱신. 나머지 상태 Delay는 0.
	void SetStateDelay(SpriteState s, int val)
	{ m_iStateDelay[s][0] = 0; m_iStateDelay[s][1] = val; m_iStateDelay[s][2] = 0; }
	// 인자로 넘겨받은 상태로 변경
	void UpdateState(SpriteState s);
	
	// 현재 방향을 알려주는 Getter 함수
	bool IsLeft() const { return m_bLeft; }
	// 방향을 설정하는 Setter 함수
	void SetDirection(bool bLeft);

	// 상태가 가지는 진입 - 진행 - 퇴장 이미지 수의 합을 반환
	int GetMaxState(SpriteState s) const { return m_iMaxState[s]; }

	// 현재 상태를 반환
	SpriteState GetCurrentState() const { return m_stCurrent; }
	// 현재 상태가 어떤 상태인지(진입 - 진행 - 퇴장) 반환
	StateState GetCurrentStateState() const;
	// 현재 출력중인 이미지 번호를 반환
	LONG GetCurrentAnimationCount() const { return m_ptCurrentAnimationState.x; }

	// 갱신
	virtual void OnUpdate();

	// 그리기
	virtual void Draw(HDC hDC, int x, int y);
};

class CEnemyAnimationSprite : public CSpriteImage
{
public:
	// Sprite State : 캐릭터의 현재 상태
	// 필요에 따른 수정 필요
	typedef enum {
		sprite_state_idle = 0
		, sprite_state_move
		, sprite_state_attack
		, sprite_state_guard
		, sprite_state_hit
		, sprite_state_pop
		, sprite_state_count
	}ESpriteState;

	// 상태의 상태(진입/진행/퇴장)
	// 수정하지 마시오
	typedef enum {
		state_state_excute = 0
		, state_state_action
		, state_state_exit

	}StateState;

protected:

	// 상태에 따른 전체 이미지 수
	int m_iMaxState[ESpriteState::sprite_state_count];
	// 상태에 따른 각 이미지의 애니메이션 딜레이
	// 진입 - 진행 - 퇴장 순으로 사용
	// 진입-퇴장 상태가 없으면 0
	int m_iStateDelay[ESpriteState::sprite_state_count][3];

	// 상태에 따른 진입 상태의 시작과 끝 이미지 번호
	int m_iStateExcute[ESpriteState::sprite_state_count][2];
	// 상태에 따른 진행 상태의 시작과 끝 이미지 번호
	int m_iStateAction[ESpriteState::sprite_state_count][2];
	// 상태에 따른 퇴장 상태의 시작과 끝 이미지 번호
	int m_iStateExit[ESpriteState::sprite_state_count][2];

	// 현재 상태
	ESpriteState m_stCurrent{ sprite_state_pop };
	// 다음 상태 : 퇴장 상태 진행 시 저장했다가 퇴장 상태 종료 후 현재 상태로 변경
	ESpriteState m_stNext = sprite_state_idle;

	// 딜레이 카운터
	// m_iStateDelay에 저장된 값만큼 Update를 지연
	// Update에 비해 느린 애니메이션 구현 가능 
	// = 적은 이미지로 비교적 부드러운 애니메이션 효과 연출 가능
	int m_nCount = 0;

	// 현재 방향
	bool m_bLeft{ true };
public:

	CEnemyAnimationSprite(LPCTSTR path, int w, int h);
	virtual ~CEnemyAnimationSprite();

	// State 값 갱신 : 진행 상태의 이미지 번호 갱신. 진입 - 퇴장 상태가 없을 때 사용
	void SetState(ESpriteState s, int val);
	// State 값 갱신 : 모든 상태의 이미지 번호 갱신. 진행 상태는 진입 - 퇴장 상태의 값에 따라 결정
	void SetState(ESpriteState s, int val, int excute_start, int excute_end, int exit_start, int exit_end);

	// Delay 값 갱신 : 진입 - 진행 - 퇴장 상태의 Delay 값 갱신
	void SetStateDelay(ESpriteState s, int excute, int action, int exit)
	{
		m_iStateDelay[s][0] = excute; m_iStateDelay[s][1] = action; m_iStateDelay[s][2] = exit;
	}
	// Delay 값 갱신 : 진행 상태의 Delay 값 갱신. 나머지 상태 Delay는 0.
	void SetStateDelay(ESpriteState s, int val)
	{
		m_iStateDelay[s][0] = 0; m_iStateDelay[s][1] = val; m_iStateDelay[s][2] = 0;
	}
	// 인자로 넘겨받은 상태로 변경
	void UpdateState(ESpriteState s);

	// 현재 방향을 알려주는 Getter 함수
	bool IsLeft() const { return m_bLeft; }
	// 방향을 설정하는 Setter 함수
	void SetDirection(bool bLeft);

	// 상태가 가지는 진입 - 진행 - 퇴장 이미지 수의 합을 반환
	int GetMaxState(ESpriteState s) const { return m_iMaxState[s]; }
	// 현재 상태를 반환
	ESpriteState GetCurrentState() const { return m_stCurrent; }
	// 현재 출력중인 이미지 번호를 반환
	LONG GetCurrentAnimationCount() const { return m_ptCurrentAnimationState.x; }
	// 현재 상태가 어떤 상태인지(진입 - 진행 - 퇴장) 반환
	StateState GetCurrentStateState() const;

	// 갱신
	virtual void OnUpdate(bool reset = false);

	// 그리기
	virtual void Draw(HDC hDC, int x, int y);
};
class CEnemyArcher_AnimationSprite : public CSpriteImage
{
public:
	// Sprite State : 캐릭터의 현재 상태
	// 필요에 따른 수정 필요
	typedef enum {
		sprite_state_idle = 0
		, sprite_state_move
		, sprite_state_attack
		, sprite_state_shot
		, sprite_state_hit
		, sprite_state_pop
		, sprite_state_count
	}ESpriteState;

	// 상태의 상태(진입/진행/퇴장)
	// 수정하지 마시오
	typedef enum {
		state_state_excute = 0
		, state_state_action
		, state_state_exit

	}StateState;

protected:

	// 상태에 따른 전체 이미지 수
	int m_iMaxState[ESpriteState::sprite_state_count];
	// 상태에 따른 각 이미지의 애니메이션 딜레이
	// 진입 - 진행 - 퇴장 순으로 사용
	// 진입-퇴장 상태가 없으면 0
	int m_iStateDelay[ESpriteState::sprite_state_count][3];

	// 상태에 따른 진입 상태의 시작과 끝 이미지 번호
	int m_iStateExcute[ESpriteState::sprite_state_count][2];
	// 상태에 따른 진행 상태의 시작과 끝 이미지 번호
	int m_iStateAction[ESpriteState::sprite_state_count][2];
	// 상태에 따른 퇴장 상태의 시작과 끝 이미지 번호
	int m_iStateExit[ESpriteState::sprite_state_count][2];

	// 현재 상태
	ESpriteState m_stCurrent = sprite_state_pop;
	// 다음 상태 : 퇴장 상태 진행 시 저장했다가 퇴장 상태 종료 후 현재 상태로 변경
	ESpriteState m_stNext = sprite_state_idle;

	// 딜레이 카운터
	// m_iStateDelay에 저장된 값만큼 Update를 지연
	// Update에 비해 느린 애니메이션 구현 가능 
	// = 적은 이미지로 비교적 부드러운 애니메이션 효과 연출 가능
	int m_nCount = 0;

	// 현재 방향
	bool m_bLeft = true;
public:

	CEnemyArcher_AnimationSprite(LPCTSTR path, int w, int h);
	virtual ~CEnemyArcher_AnimationSprite();

	// State 값 갱신 : 진행 상태의 이미지 번호 갱신. 진입 - 퇴장 상태가 없을 때 사용
	void SetState(ESpriteState s, int val);
	// State 값 갱신 : 모든 상태의 이미지 번호 갱신. 진행 상태는 진입 - 퇴장 상태의 값에 따라 결정
	void SetState(ESpriteState s, int val, int excute_start, int excute_end, int exit_start, int exit_end);

	// Delay 값 갱신 : 진입 - 진행 - 퇴장 상태의 Delay 값 갱신
	void SetStateDelay(ESpriteState s, int excute, int action, int exit)
	{
		m_iStateDelay[s][0] = excute; m_iStateDelay[s][1] = action; m_iStateDelay[s][2] = exit;
	}
	// Delay 값 갱신 : 진행 상태의 Delay 값 갱신. 나머지 상태 Delay는 0.
	void SetStateDelay(ESpriteState s, int val)
	{
		m_iStateDelay[s][0] = 0; m_iStateDelay[s][1] = val; m_iStateDelay[s][2] = 0;
	}
	// 인자로 넘겨받은 상태로 변경
	void UpdateState(ESpriteState s);

	// 현재 방향을 알려주는 Getter 함수
	bool IsLeft() const { return m_bLeft; }
	// 방향을 설정하는 Setter 함수
	void SetDirection(bool bLeft);

	// 상태가 가지는 진입 - 진행 - 퇴장 이미지 수의 합을 반환
	int GetMaxState(ESpriteState s) const { return m_iMaxState[s]; }
	// 현재 상태를 반환
	ESpriteState GetCurrentState() const { return m_stCurrent; }
	// 현재 출력중인 이미지 번호를 반환
	LONG GetCurrentAnimationCount() const { return m_ptCurrentAnimationState.x; }
	// 현재 상태가 어떤 상태인지(진입 - 진행 - 퇴장) 반환
	StateState GetCurrentStateState() const;

	// 갱신
	virtual void OnUpdate(bool reset = false);

	// 그리기
	virtual void Draw(HDC hDC, int x, int y);
};

class CBOSSAnimationSprite : public CSpriteImage
{
public:
	// Sprite State : 캐릭터의 현재 상태
	// 필요에 따른 수정 필요
	typedef enum {
		  sprite_state_phase1_idle = 0
		, sprite_state_phase1_hit
		, sprite_state_phase2_shift
		, sprite_state_phase2_idle
		, sprite_state_phase2_summon
		, sprite_state_phase2_attack
		, sprite_state_phase2_hit
		, sprite_state_phase3_shift
		, sprite_state_phase3_idle
		, sprite_state_phase3_dash_ready
		, sprite_state_phase3_dash
		, sprite_state_phase3_attack
		, sprite_state_phase3_die
		, sprite_state_count
	}SpriteState;

	// 상태의 상태(진입/진행/퇴장)
	// 수정하지 마시오
	typedef enum {
		state_state_excute = 0
		, state_state_action
		, state_state_exit

	}StateState;

protected:

	// 상태에 따른 전체 이미지 수
	int m_iMaxState[SpriteState::sprite_state_count];
	// 상태에 따른 각 이미지의 애니메이션 딜레이
	// 진입 - 진행 - 퇴장 순으로 사용
	// 진입-퇴장 상태가 없으면 0
	int m_iStateDelay[SpriteState::sprite_state_count][3];

	// 상태에 따른 진입 상태의 시작과 끝 이미지 번호
	int m_iStateExcute[SpriteState::sprite_state_count][2];
	// 상태에 따른 진행 상태의 시작과 끝 이미지 번호
	int m_iStateAction[SpriteState::sprite_state_count][2];
	// 상태에 따른 퇴장 상태의 시작과 끝 이미지 번호
	int m_iStateExit[SpriteState::sprite_state_count][2];

	// 현재 상태
	SpriteState m_stCurrent = sprite_state_phase1_idle;
	// 다음 상태 : 퇴장 상태 진행 시 저장했다가 퇴장 상태 종료 후 현재 상태로 변경
	SpriteState m_stNext = sprite_state_phase1_idle;

	// 딜레이 카운터
	// m_iStateDelay에 저장된 값만큼 Update를 지연
	// Update에 비해 느린 애니메이션 구현 가능 
	// = 적은 이미지로 비교적 부드러운 애니메이션 효과 연출 가능
	int m_nCount = 0;

	// 현재 방향
	bool m_bLeft = false;
	int m_phase = 1;
public:

	CBOSSAnimationSprite(LPCTSTR path, int w, int h);
	virtual ~CBOSSAnimationSprite();

	// State 값 갱신 : 진행 상태의 이미지 번호 갱신. 진입 - 퇴장 상태가 없을 때 사용
	void SetState(SpriteState s, int val);
	// State 값 갱신 : 모든 상태의 이미지 번호 갱신. 진행 상태는 진입 - 퇴장 상태의 값에 따라 결정
	void SetState(SpriteState s, int val, int excute_start, int excute_end, int exit_start, int exit_end);

	// Delay 값 갱신 : 진입 - 진행 - 퇴장 상태의 Delay 값 갱신
	void SetStateDelay(SpriteState s, int excute, int action, int exit)
	{
		m_iStateDelay[s][0] = excute; m_iStateDelay[s][1] = action; m_iStateDelay[s][2] = exit;
	}
	// Delay 값 갱신 : 진행 상태의 Delay 값 갱신. 나머지 상태 Delay는 0.
	void SetStateDelay(SpriteState s, int val)
	{
		m_iStateDelay[s][0] = 0; m_iStateDelay[s][1] = val; m_iStateDelay[s][2] = 0;
	}
	// 인자로 넘겨받은 상태로 변경
	void UpdateState(SpriteState s);

	void SetPhase(int phase) { m_phase = phase; }
	// 현재 방향을 알려주는 Getter 함수
	bool IsLeft() const { return m_bLeft; }
	// 방향을 설정하는 Setter 함수
	void SetDirection(bool bLeft);

	// 상태가 가지는 진입 - 진행 - 퇴장 이미지 수의 합을 반환
	int GetMaxState(SpriteState s) const { return m_iMaxState[s]; }
	// 현재 상태를 반환
	SpriteState GetCurrentState() const { return m_stCurrent; }
	// 현재 출력중인 이미지 번호를 반환
	LONG GetCurrentAnimationCount() const { return m_ptCurrentAnimationState.x; }
	// 현재 상태가 어떤 상태인지(진입 - 진행 - 퇴장) 반환
	StateState GetCurrentStateState() const;

	// 갱신
	virtual void OnUpdate(bool reset = false);

	// 그리기
	virtual void Draw(HDC hDC, int x, int y);
};