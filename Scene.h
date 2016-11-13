#pragma once

#define INPUT_KEY_CYCLE 0.0

class CFramework;
class CSoundManager;
class CPlayer;

class CScene
{

public:

	enum class CurrentScene : int {
		None = 0
		, Title
		, Lobby
		, Stage_0
		, Stage_1
		, Stage_2
		, GameOver
		, GameClear
		, Loading
		, Count
	};

protected:

	CFramework		*m_pFramework{ nullptr };
	CSoundManager	*m_pSound{ nullptr };
	CPlayer			**m_Player{nullptr};

	HWND			m_hWnd;
	RECT			m_rcScene{ 0, 0, 0, 0 };
	POINT			m_PlayerPos{ 0, 0 };

	CurrentScene	m_Tag;

	float			m_fCurrentUpdate{ 0.0f };
	bool			m_Build{ false };
	bool			m_Clear{ false };
	bool			m_bF1{ false };
public:
	CScene();
	virtual ~CScene();
	
	// 함수 호출 이후에도 처리를 원하면 false 반환
	virtual bool OnProcessingKeyboardMessage	(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	virtual bool OnProcessingMouseMessage		(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	virtual bool OnProcessingWindowMessage		(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	virtual void BuildObjects(CFramework* pFramework, CSoundManager *pSoundManager, HWND hwnd, CurrentScene tag, CPlayer **Player);
	virtual void ReSetObjects() = 0;
	virtual void ReleaseObjects() = 0;

	virtual void Update(float fTimeElapsed) = 0;
	virtual void Draw(HDC hdc) = 0;
	virtual void Update_Scene() = 0;

	CFramework* GetMyFramework() { return m_pFramework; }

	bool IsMyTag(CurrentScene tag) const { return (tag == m_Tag); }
	CurrentScene GetMyTag() const { return m_Tag; };
};

