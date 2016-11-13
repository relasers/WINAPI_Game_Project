#include "stdafx.h"
#include <Windows.h>
#include <iostream>

#include "Framework.h"
#include "LobbyScene.h"

using namespace std;

CLobbyScene::CLobbyScene()
{
}
CLobbyScene::~CLobbyScene()
{
	ReleaseObjects();
}

void CLobbyScene::Update(float fTimeElapsed)
{
	m_cimgBackground->OnUpdate();
	m_cimgMenu->OnUpdate();
	m_cimgMenuCusser->OnUpdate();
}

void CLobbyScene::Draw(HDC hdc)
{
	m_cimgBackground->Draw(hdc, 0, 0, m_rcScene.right, m_rcScene.bottom);
	m_cimgMenu->Draw(hdc, m_rcScene.right * 0.625, 430);//750
	m_cimgMenuCusser->Draw(hdc, m_rcScene.right * 0.625 - 70, 430 + m_CusserMove);//680
}

bool CLobbyScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
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
			if (m_CusserMove == 0)
			{
				m_pSound->PlaySnd_BGM(CSoundManager::BGM_SOUND::Stop_BGM);
				m_pSound->PlaySnd_BGM(CSoundManager::BGM_SOUND::Stop_BGM);
				m_pSound->PlaySnd_BGM(CSoundManager::BGM_SOUND::Stop_BGM);
				m_pSound->PlaySnd_BGM(CSoundManager::BGM_SOUND::Stop_BGM);
				m_pSound->PlaySnd_BGM(CSoundManager::BGM_SOUND::Stop_BGM);
				m_pSound->PlaySnd_BGM(CSoundManager::BGM_SOUND::Stage_BGM);
				m_pFramework->ChangeScene(CurrentScene::Stage_0);
			}
			else if (m_CusserMove == 65)
				PostQuitMessage(0);
			break;
		case VK_UP:
			m_CusserMove = 0;
			m_pSound->PlaySnd_UI(CSoundManager::UI_SOUND::Lobby_Menu_Select_UI);
			break;
		case VK_DOWN:
			m_CusserMove = 65;
			m_pSound->PlaySnd_UI(CSoundManager::UI_SOUND::Lobby_Menu_Select_UI);
			break;
		}
		break;
	}
	return false;
}

bool CLobbyScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
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

bool CLobbyScene::OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
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

void CLobbyScene::BuildObjects(CFramework * myFramework, CSoundManager *pSoundManager, HWND hWnd, CurrentScene tag, CPlayer **Player)
{
	CScene::BuildObjects(myFramework, pSoundManager, hWnd, tag, Player);
	m_cimgBackground = new CSpriteImage(TEXT("Resource\\Graphics\\Background\\Lobby_BK.png"), 3, 3);
	m_cimgMenu		 = new CSpriteImage(TEXT("Resource\\Graphics\\UI\\Lobby_Menu.png"), 4, 1);
	m_cimgMenuCusser = new CSpriteImage(TEXT("Resource\\Graphics\\UI\\Lobby_Cusser.png"), 4, 1);
}

void CLobbyScene::ReleaseObjects()
{
	m_cimgBackground->~CSpriteImage();
	m_cimgMenu->~CSpriteImage();
	m_cimgMenuCusser->~CSpriteImage();
}