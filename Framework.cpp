#include "stdafx.h"
#include "Framework.h"
#include <string>

#include "SpriteImage.h"

#include "TitleScene.h"
#include "LobbyScene.h"
#include "StageScene_0.h"
#include "StageScene_1.h"
#include "StageScene_2.h"
#include "GameOverScene.h"
#include "GameClearScene.h"
#include "LoadingScene.h"

using namespace std;

template <typename T>
T GetUserDataPtr(HWND hWnd) 
{
	return reinterpret_cast<T>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
}
void SetUserDataPtr(HWND hWnd, LPVOID ptr) 
{
	LONG_PTR result = ::SetWindowLongPtr(
		hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(ptr));
}

CFramework::CFramework()
{
}
CFramework::~CFramework()
{
	OnDestroy();
}

bool CFramework::OnCreate(HWND hWnd, const RECT & rc)
{
	m_hWnd = hWnd;
	m_rcClient = rc;
	srand((unsigned int)time(NULL));

	// 클라이언트 좌표 초기화
	m_rcClient.right -= m_rcClient.left;
	m_rcClient.bottom -= m_rcClient.top;
	m_rcClient.left = 0;
	m_rcClient.top = 0;

	// 버퍼 생성
	CreatebackBuffer();

	// 클래스와 윈도우 프로시저 연결
	::SetUserDataPtr(m_hWnd, this);

	// 사운드 생성
	m_Sound.Initialize();

	// 현재 Scene은 없는걸로
	m_pCurrentScene = nullptr;
	// Scene 배열 초기화
	for (auto& p : m_arrScene) 
		p = nullptr;

	// 캐릭터 생성
	BuildPlayer();
	
	//// Scene 생성
	//BuildScene();
	ChangeScene(CScene::CurrentScene::Title);

	// 캡션 변경
	lstrcpy(m_CaptionTitle, TITLESTRING);

#if defined(SHOW_CAPTIONFPS)
	lstrcat(m_CaptionTitle, TEXT(" ("));
#endif
	m_TitleLength = lstrlen(m_CaptionTitle);
	SetWindowText(m_hWnd, m_CaptionTitle);
	
	// 타이머 초기화
	m_LastUpdate_time = chrono::system_clock::now();
	m_current_time = chrono::system_clock::now();
	m_fps = 0;

	return (m_hWnd != NULL);
}

void CFramework::CreatebackBuffer()
{
	if (m_hDC)
	{
		::SelectObject(m_hDC, NULL);
		::DeleteDC(m_hDC);
	}
	if (m_hBitmapBackBuffer) ::DeleteObject(m_hBitmapBackBuffer);

	HDC hdc = ::GetDC(m_hWnd);
	m_hDC = ::CreateCompatibleDC(hdc);
	m_hBitmapBackBuffer = ::CreateCompatibleBitmap(hdc, m_rcClient.right, m_rcClient.bottom);
	::SelectObject(m_hDC, m_hBitmapBackBuffer);

	SetBKColor(RGB(255, 255, 255));

	ClearBackgroundColor();

	::ReleaseDC(m_hWnd, hdc);
}
void CFramework::BuildScene()
{
	m_arrScene[GetSceneEnumInt(Title)] = new CTitleScene();
	m_arrScene[GetSceneEnumInt(Title)]->BuildObjects(this, &m_Sound, m_hWnd, CScene::CurrentScene::Title, &m_Player);

	m_arrScene[GetSceneEnumInt(Lobby)] = new CLobbyScene();
	m_arrScene[GetSceneEnumInt(Lobby)]->BuildObjects(this, &m_Sound, m_hWnd, CScene::CurrentScene::Lobby, &m_Player);

	m_arrScene[GetSceneEnumInt(Stage_0)] = new CStageScene_0();
	m_arrScene[GetSceneEnumInt(Stage_0)]->BuildObjects(this, &m_Sound, m_hWnd, CScene::CurrentScene::Stage_0, &m_Player);

	m_arrScene[GetSceneEnumInt(Stage_1)] = new CStageScene_1();
	m_arrScene[GetSceneEnumInt(Stage_1)]->BuildObjects(this, &m_Sound, m_hWnd, CScene::CurrentScene::Stage_1, &m_Player);
	
	m_arrScene[GetSceneEnumInt(Stage_2)] = new CStageScene_2();
	m_arrScene[GetSceneEnumInt(Stage_2)]->BuildObjects(this, &m_Sound, m_hWnd, CScene::CurrentScene::Stage_2, &m_Player);

	m_arrScene[GetSceneEnumInt(GameOver)] = new CGameOverScene();
	m_arrScene[GetSceneEnumInt(GameOver)]->BuildObjects(this, &m_Sound, m_hWnd, CScene::CurrentScene::GameOver, &m_Player);

	m_arrScene[GetSceneEnumInt(GameClear)] = new CGameClearScene();
	m_arrScene[GetSceneEnumInt(GameClear)]->BuildObjects(this, &m_Sound, m_hWnd, CScene::CurrentScene::GameClear, &m_Player);

	m_arrScene[GetSceneEnumInt(Loading)] = new CLoadingScene();
	m_arrScene[GetSceneEnumInt(Loading)]->BuildObjects(this, &m_Sound, m_hWnd, CScene::CurrentScene::Loading, &m_Player);

	m_pCurrentScene = m_arrScene[GetSceneEnumInt(Title)]; // 현재신은 타이틀로 초기화

}
void CFramework::BuildPlayer()
{
	m_Player = new CPlayer(TEXT("Resource\\Graphics\\Sprite\\캐릭터.png"), &m_Sound, 20, 26);

	m_Player->SetState(CAnimationSprite::SpriteState::sprite_state_idle, 4);
	m_Player->SetState(CAnimationSprite::SpriteState::sprite_state_move, 4, 0, 4, 4, 4);
	m_Player->SetState(CAnimationSprite::SpriteState::sprite_state_jump, 16);
	m_Player->SetState(CAnimationSprite::SpriteState::sprite_state_attack_0, 4, 0, 0, 3, 4);
	m_Player->SetState(CAnimationSprite::SpriteState::sprite_state_attack_1, 4, 0, 0, 3, 4);
	m_Player->SetState(CAnimationSprite::SpriteState::sprite_state_attack_2, 4, 0, 0, 3, 4);
	m_Player->SetState(CAnimationSprite::SpriteState::sprite_state_attack_3, 5, 0, 0, 3, 5);
	m_Player->SetState(CAnimationSprite::SpriteState::sprite_state_dash, 12, 0, 10, 10, 12);
	m_Player->SetState(CAnimationSprite::SpriteState::sprite_state_jump_attack, 4, 0, 1, 3, 4);
	m_Player->SetState(CAnimationSprite::SpriteState::sprite_state_guard, 16, 0, 1, 3, 16);
	m_Player->SetState(CAnimationSprite::SpriteState::sprite_state_parrying, 9, 0, 4, 8, 9);
	m_Player->SetState(CAnimationSprite::SpriteState::sprite_state_hit, 12, 0, 4, 9, 12);
	m_Player->SetState(CAnimationSprite::SpriteState::sprite_state_shot, 20, 0, 5, 7, 20);

	m_Player->SetStateDelay(CAnimationSprite::SpriteState::sprite_state_idle, 5);
	m_Player->SetStateDelay(CAnimationSprite::SpriteState::sprite_state_move, 5, 3, 3);
	m_Player->SetStateDelay(CAnimationSprite::SpriteState::sprite_state_jump, 2);
	m_Player->SetStateDelay(CAnimationSprite::SpriteState::sprite_state_attack_0, 1, 5, 3);
	m_Player->SetStateDelay(CAnimationSprite::SpriteState::sprite_state_attack_1, 1, 5, 3);
	m_Player->SetStateDelay(CAnimationSprite::SpriteState::sprite_state_attack_2, 1, 5, 3);
	m_Player->SetStateDelay(CAnimationSprite::SpriteState::sprite_state_attack_3, 1, 5, 3);
	m_Player->SetStateDelay(CAnimationSprite::SpriteState::sprite_state_dash, 1, 1, 5);
	m_Player->SetStateDelay(CAnimationSprite::SpriteState::sprite_state_jump_attack, 1, 10, 3);
	m_Player->SetStateDelay(CAnimationSprite::SpriteState::sprite_state_guard, 3, 5, 3);
	m_Player->SetStateDelay(CAnimationSprite::SpriteState::sprite_state_parrying, 1, 5, 20);
	m_Player->SetStateDelay(CAnimationSprite::SpriteState::sprite_state_hit, 1, 10, 10);
	m_Player->SetStateDelay(CAnimationSprite::SpriteState::sprite_state_shot, 4, 4, 6);
}

void CFramework::ReleaseScene()
{
}

bool CFramework::OnDestroy()
{
	if (m_hbrBackground) ::DeleteObject(m_hbrBackground);
	
	::SelectObject(m_hDC, NULL);
	if (m_hBitmapBackBuffer) ::DeleteObject(m_hBitmapBackBuffer);
	if (m_hDC) ::DeleteDC(m_hDC);

	return false;
}

bool CFramework::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	if (m_pCurrentScene)
	if (m_pCurrentScene->OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam)) return false;
	switch (nMessageID)
	{
	case WM_KEYDOWN:
		return true;
	case WM_KEYUP:
		switch (wParam)
		{
		case VK_ESCAPE:
			DestroyWindow(hWnd);
			break;
		}
		return true;
	}
	return false;
}
bool CFramework::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	if(m_pCurrentScene)
	if (m_pCurrentScene->OnProcessingMouseMessage(hWnd, nMessageID, wParam, lParam)) return false;
	switch (nMessageID)
	{
	case WM_LBUTTONUP:
		break;

	case WM_LBUTTONDOWN:
		break;

	case WM_MOUSEMOVE:
		break;
	}
	return false;
}
HRESULT CFramework::OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	default:
		return ::DefWindowProc(hWnd, nMessageID, wParam, lParam);

	}
	return 0;
}
void CFramework::Update(float fTimeElapsed)
{
	if(m_pCurrentScene) m_pCurrentScene->Update(fTimeElapsed);
	m_Sound.Update(fTimeElapsed);
}
void CFramework::SetBKColor(COLORREF color)
{
	m_clrBackBuffer = color;
	if (m_hbrBackground) ::DeleteObject(m_hbrBackground);
	m_hbrBackground = ::CreateSolidBrush(m_clrBackBuffer);
}
void CFramework::ClearBackgroundColor()
{
	::FillRect(m_hDC, &m_rcClient, m_hbrBackground);
}
void CFramework::PreproccessingForDraw()
{
	ClearBackgroundColor();
	::SetBkMode(m_hDC, TRANSPARENT);
	::SetStretchBltMode(m_hDC, COLORONCOLOR);

	if(m_pCurrentScene) m_pCurrentScene->Draw(m_hDC);
}

void CFramework::OnDraw(HDC hDC)
{
	::BitBlt(hDC, m_rcClient.left, m_rcClient.top, m_rcClient.right, m_rcClient.bottom, m_hDC, 0, 0, SRCCOPY);
}

void CFramework::ChangeScene(CScene::CurrentScene Tag, bool bDestroy)
{
	const int tag = GetEnumValueByType(Tag);
	//
	if (bDestroy) {
		int d_tag = GetEnumValueByType(m_pCurrentScene->GetMyTag());
		delete m_pCurrentScene;
		m_pCurrentScene = nullptr;
		m_arrScene[d_tag] = nullptr;
	}
	if (m_arrScene[tag]) { // 만약 nullptr가 아닐 경우
		m_pCurrentScene = m_arrScene[tag];
		//if (Tag == CScene::CurrentScene::Stage_0)
		if(m_pCurrentScene == m_arrScene[GetSceneEnumInt(Stage_0)])
		{
			if (m_arrScene[GetSceneEnumInt(Stage_0)]) m_arrScene[GetSceneEnumInt(Stage_0)]->ReSetObjects();
			if (m_arrScene[GetSceneEnumInt(Stage_1)]) m_arrScene[GetSceneEnumInt(Stage_1)]->ReSetObjects();
			if (m_arrScene[GetSceneEnumInt(Stage_2)]) m_arrScene[GetSceneEnumInt(Stage_2)]->ReSetObjects();
			m_Player->ReSet();
		}
	}
	else {
		switch (Tag) {
		case CScene::CurrentScene::Title:
			m_arrScene[GetSceneEnumInt(Title)] = new CTitleScene();
			m_arrScene[GetSceneEnumInt(Title)]->BuildObjects(this, &m_Sound, m_hWnd, CScene::CurrentScene::Title, &m_Player);
			break;
		case CScene::CurrentScene::Lobby:
			m_arrScene[GetSceneEnumInt(Lobby)] = new CLobbyScene();
			m_arrScene[GetSceneEnumInt(Lobby)]->BuildObjects(this, &m_Sound, m_hWnd, CScene::CurrentScene::Lobby, &m_Player);
			break;
		case CScene::CurrentScene::Stage_0:
			m_arrScene[GetSceneEnumInt(Stage_0)] = new CStageScene_0();
			m_arrScene[GetSceneEnumInt(Stage_0)]->BuildObjects(this, &m_Sound, m_hWnd, CScene::CurrentScene::Stage_0, &m_Player);
			break;																										
		case CScene::CurrentScene::Stage_1:																				
			m_arrScene[GetSceneEnumInt(Stage_1)] = new CStageScene_1();													
			m_arrScene[GetSceneEnumInt(Stage_1)]->BuildObjects(this, &m_Sound, m_hWnd, CScene::CurrentScene::Stage_1, &m_Player);
			break;																										
		case CScene::CurrentScene::Stage_2:																				
			m_arrScene[GetSceneEnumInt(Stage_2)] = new CStageScene_2();													
			m_arrScene[GetSceneEnumInt(Stage_2)]->BuildObjects(this, &m_Sound, m_hWnd, CScene::CurrentScene::Stage_2, &m_Player);
			break;
		case CScene::CurrentScene::GameOver:
			m_arrScene[GetSceneEnumInt(GameOver)] = new CGameOverScene();
			m_arrScene[GetSceneEnumInt(GameOver)]->BuildObjects(this, &m_Sound, m_hWnd, CScene::CurrentScene::GameOver, &m_Player);
			break;
		case CScene::CurrentScene::GameClear:
			m_arrScene[GetSceneEnumInt(GameClear)] = new CGameClearScene();
			m_arrScene[GetSceneEnumInt(GameClear)]->BuildObjects(this, &m_Sound, m_hWnd, CScene::CurrentScene::GameClear, &m_Player);
			break;
		case CScene::CurrentScene::Loading:
			m_arrScene[GetSceneEnumInt(Loading)] = new CLoadingScene();
			m_arrScene[GetSceneEnumInt(Loading)]->BuildObjects(this, &m_Sound, m_hWnd, CScene::CurrentScene::Loading, &m_Player);
			break;
		}
		m_pCurrentScene = m_arrScene[tag];
	}
}

void CFramework::FrameAdvance()
{
	// Get tick
	m_timeElapsed = chrono::system_clock::now() - m_current_time;
	if (m_timeElapsed.count() > MAX_FPS)
	{
		m_current_time = chrono::system_clock::now();
	
		if (m_timeElapsed.count() > 0.0)
			m_fps = 1.0 / m_timeElapsed.count();
	}
	// 최대 FPS 미만의 시간이 경과하면 진행 생략
	else return;

	Update(m_timeElapsed.count());
	PreproccessingForDraw();

	InvalidateRect(m_hWnd, &m_rcClient, FALSE);

#if defined(SHOW_CAPTIONFPS)

	m_UpdateElapsed = chrono::system_clock::now() - m_LastUpdate_time;
	if (m_UpdateElapsed.count() > MAX_UPDATE_FPS)
		m_LastUpdate_time = chrono::system_clock::now();
	else return;

	_itow_s(m_fps + 0.1f, m_CaptionTitle + m_TitleLength, TITLE_MX_LENGTH - m_TitleLength, 10);
	wcscat_s(m_CaptionTitle + m_TitleLength, TITLE_MX_LENGTH - m_TitleLength, TEXT(" FPS)"));
	SetWindowText(m_hWnd, m_CaptionTitle);
#endif

}

LRESULT CFramework::WndProc(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	CFramework* self = ::GetUserDataPtr<CFramework*>(hWnd);
	if (!self)
		return ::DefWindowProc(hWnd, nMessageID, wParam, lParam);

	switch (nMessageID)
	{

	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:

	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:

	case WM_MOUSEMOVE:
		self->OnProcessingMouseMessage(hWnd, nMessageID, wParam, lParam);
		break;

	case WM_KEYDOWN:
	case WM_KEYUP:
		self->OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam);
		break;

	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = ::BeginPaint(hWnd, &ps);
			
			self->OnDraw(hdc);

			::EndPaint(hWnd, &ps);
		}
		break;

	case WM_DESTROY:
		::SetUserDataPtr(hWnd, NULL);
		::PostQuitMessage(0);
		break;

	default:
		return self->OnProcessingWindowMessage(hWnd, nMessageID, wParam, lParam);

	}
	return 0;
}
