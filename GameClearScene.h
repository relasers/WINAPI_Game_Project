#pragma once
#include "Scene.h"
#include "SpriteImage.h"

class CGameClearScene :public CScene
{
protected:

	CSpriteImage	*m_cimgBackground;

public:
	CGameClearScene();
	virtual ~CGameClearScene();

	virtual void Update(float fTimeElapsed);
	virtual void Draw(HDC hdc);

	// 함수 호출 이후에도 처리를 원하면 false 반환
	virtual bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	virtual bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	virtual bool OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	virtual void BuildObjects(CFramework* pFramework, CSoundManager *pSoundManager, HWND hwnd, CurrentScene tag, CPlayer **Player);
	virtual void ReSetObjects() {};
	virtual void ReleaseObjects();
	virtual void Update_Scene() {}
};

