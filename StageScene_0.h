#pragma once
#include "Scene.h"
#include <atlimage.h>

#define STAGE_0_ENEMY_NUM 10

class CPlayer;
class CEnemy;

template<typename T>
class WarpList;

class CStageScene_0 :public CScene
{
protected:
	CImage	m_cimgBackground;
	CImage m_cimgControl;

	CPlayer			**m_Player;
	WarpList<CEnemy*> m_EnemyList;

	POINT m_ScenePos;
public:
	CStageScene_0();
	virtual ~CStageScene_0();

	virtual void Update(float fTimeElapsed);
	virtual void Draw(HDC hdc);

	// 함수 호출 이후에도 처리를 원하면 false 반환
	virtual bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	virtual bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	virtual bool OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	virtual void BuildObjects(CFramework* pFramework, CSoundManager *pSoundManager, HWND hwnd, CurrentScene tag, CPlayer **Player);
	virtual void ReSetObjects();
	virtual void ReleaseObjects();
	virtual void Update_Scene();

	void UnitInitialize();
	bool ProcessInput();
};

