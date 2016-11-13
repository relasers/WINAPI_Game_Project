#include "stdafx.h"
#include <Windows.h>
#include <iostream>

#include "Framework.h"
#include "TitleScene.h"

using namespace std;

CTitleScene::CTitleScene()
{
}
CTitleScene::~CTitleScene()
{
	ReleaseObjects();
}

void CTitleScene::Update(float fTimeElapsed)
{
	if (m_Alpha > 0) m_Alpha -= 1;
	else
	{
		m_cimgBackground->OnUpdate();
		if (!m_Sound)
		{
			m_pSound->PlaySnd_BGM(CSoundManager::BGM_SOUND::Stop_BGM);
			m_pSound->PlaySnd_BGM(CSoundManager::BGM_SOUND::Title_BGM);
		}
		m_Sound = true;
	}
}

void CTitleScene::Draw(HDC hdc)
{
	if (m_Alpha > 0) m_Logo.AlphaBlend(hdc, 0, 0, m_rcScene.right, m_rcScene.bottom, 0, 0, m_Logo.GetWidth(), m_Logo.GetHeight(), m_Alpha);
	else m_cimgBackground->Draw(hdc, 0, 0, m_rcScene.right, m_rcScene.bottom);
}

bool CTitleScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_KEYDOWN:
		return true;
	case WM_KEYUP:
		m_pSound->PlaySnd_BGM(CSoundManager::BGM_SOUND::Stop_BGM);
		m_pSound->PlaySnd_BGM(CSoundManager::BGM_SOUND::Lobby_BGM);
		m_pFramework->ChangeScene(CurrentScene::Lobby);
		return true;
	}
	return false;
}

bool CTitleScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_LBUTTONDOWN:
		return true;
	case WM_RBUTTONDOWN:
		break;
	case WM_MOUSEMOVE:
		return true;
	}
	return false;
}

bool CTitleScene::OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
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

void CTitleScene::BuildObjects(CFramework * myFramework, CSoundManager *pSoundManager, HWND hWnd, CurrentScene tag, CPlayer **Player)
{
	CScene::BuildObjects(myFramework, pSoundManager, hWnd, tag, Player);
	m_Logo.Load(TEXT("Resource\\Graphics\\Background\\Logo.jpg"));
	m_cimgBackground = new CSpriteImage(TEXT("Resource\\Graphics\\Background\\Title_BK.png"), 3, 2);
	m_pSound->PlaySnd_BGM(CSoundManager::BGM_SOUND::Logo_BGM);
	m_Alpha = 255;
}

void CTitleScene::ReleaseObjects()
{
	m_cimgBackground->~CSpriteImage();
}