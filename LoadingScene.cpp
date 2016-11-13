#include "stdafx.h"
#include <Windows.h>
#include <iostream>

#include "Framework.h"
#include "LoadingScene.h"

using namespace std;

CLoadingScene::CLoadingScene()
{
}
CLoadingScene::~CLoadingScene()
{
}

void CLoadingScene::Update(float fTimeElapsed)
{
	m_cimgBackground->OnUpdate();
}

void CLoadingScene::Draw(HDC hdc)
{
	m_cimgBackground->Draw(hdc, 0, 0, m_rcScene.right, m_rcScene.bottom);
}

bool CLoadingScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
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
				m_pSound->PlaySnd_BGM(CSoundManager::BGM_SOUND::Stage_BGM);
				m_pFramework->ChangeScene(CurrentScene::Stage_0);
			break;
		}
		break;
	}
	return false;
}

bool CLoadingScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
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

bool CLoadingScene::OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
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

void CLoadingScene::BuildObjects(CFramework * myFramework, CSoundManager *pSoundManager, HWND hWnd, CurrentScene tag, CPlayer **Player)
{
	CScene::BuildObjects(myFramework, pSoundManager, hWnd, tag, Player);
	m_cimgBackground = new CSpriteImage(TEXT("Resource\\Graphics\\Background\\Lobby_BK.png"), 3, 3);
}

void CLoadingScene::ReleaseObjects()
{
}