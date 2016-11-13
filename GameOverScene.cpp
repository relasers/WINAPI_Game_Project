#include "stdafx.h"
#include <Windows.h>
#include <iostream>

#include "Framework.h"
#include "GameOverScene.h"

using namespace std;

CGameOverScene::CGameOverScene()
{
}
CGameOverScene::~CGameOverScene()
{
	ReleaseObjects();
}

void CGameOverScene::Update(float fTimeElapsed)
{
	m_cimgBackground->OnUpdate();
}

void CGameOverScene::Draw(HDC hdc)
{
	m_cimgBackground->Draw(hdc, 0, 0, m_rcScene.right, m_rcScene.bottom);
}

bool CGameOverScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_KEYDOWN:
		switch (wParam)
		{
		}
		break;

	case WM_KEYUP:
		switch (wParam)
		{
		case VK_RETURN:
			m_pSound->PlaySnd_BGM(CSoundManager::BGM_SOUND::Stop_BGM);
			m_pFramework->ChangeScene(CurrentScene::Title);
			break;
		}
		break;
	}
	return false;
}

bool CGameOverScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
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

bool CGameOverScene::OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
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

void CGameOverScene::BuildObjects(CFramework * myFramework, CSoundManager *pSoundManager, HWND hWnd, CurrentScene tag, CPlayer **Player)
{
	CScene::BuildObjects(myFramework, pSoundManager, hWnd, tag, Player);
	m_cimgBackground = new CSpriteImage(TEXT("Resource\\Graphics\\Background\\Game_Over.png"), 1, 1);
}

void CGameOverScene::ReleaseObjects()
{
	m_cimgBackground->~CSpriteImage();
}