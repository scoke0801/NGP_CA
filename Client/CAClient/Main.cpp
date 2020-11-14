// PopUpTheBall.cpp: 응용 프로그램의 진입점을 정의합니다.
//

#include "stdafx.h"
#include "Main.h"
#include "GameFramework.h"

#define MAX_LOADSTRING 100

// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.
CFramework gFramework;

// 이 코드 모듈에 들어 있는 함수의 정방향 선언입니다.
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 여기에 코드를 입력합니다.

    // 전역 문자열을 초기화합니다.
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_CACLIENT, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 응용 프로그램 초기화를 수행합니다.
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_CACLIENT));

    MSG msg;

	// GetMessage()는 입력값이 없는 경우에 업데이트 처리가 밀리는 문제가 있음
	// 그래서 보통 PeekMessage()를 사용
	
  /*  while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }*/
	

	while (true)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				break;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			gFramework.preUpdate();
		}
	}

    return (int) msg.wParam;
}



//
//  함수: MyRegisterClass()
//
//  목적: 창 클래스를 등록합니다.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;				

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;	
    wcex.lpfnWndProc    = WndProc;					
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CACLIENT));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= nullptr; //MAKEINTRESOURCEW(IDC_POPUPTHEBALL);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   함수: InitInstance(HINSTANCE, int)
//
//   목적: 인스턴스 핸들을 저장하고 주 창을 만듭니다.
//
//   설명:
//
//        이 함수를 통해 인스턴스 핸들을 전역 변수에 저장하고
//        주 프로그램 창을 만든 다음 표시합니다.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

	// 생성할 윈도우의 형식을 지정해줍니다.
	DWORD dwStyle =
		WS_OVERLAPPED	  //디폴트 윈도우.타이틀 바와 크기 조절이 안되는 경계선을 갖는 윈도우를 만듭니다.
		| WS_SYSMENU	  //시스템 메뉴를 가진 윈도우를 만듭니다.
		| WS_MINIMIZEBOX  //최소화 버튼을 갖는 윈도우를 만듭니다.
		| WS_CAPTION	  //타이틀 바를 가진 윈도우를 만들며 WS_BORDER 스타일을 포함합니다.
	//  | WS_BORDER		  //단선으로 된 경계선(크기 조절 불가능)을 만듭니다
	//  | WS_CLIPCHILDREN //차일드가 위치한 영역은 그리지 않습니다.
	//  | WS_CLIPSIBLINGS //차일드끼리 상호 겹친 영역은 그리기 영역에서 제외
		;

	RECT rtClient, rtDesktop;

	// 윈도우가 생성될 위치를 화면의 중간이 되도록 합니다.
	GetWindowRect(GetDesktopWindow(), &rtDesktop); // 화면의 크기를 얻어옴

	//cout << rtDesktop.left << ", " << rtDesktop.top << ", "
	//	<< rtDesktop.right << ", " << rtDesktop.bottom << endl;

	// 클라이언트의 크기를 설정합니다.
	rtClient.left = rtClient.top = 0;
	rtClient.right = CLIENT_WIDTH;
	rtClient.bottom = CLIENT_HEIGHT;

	// 윈도우 사이즈에서 추가되는 크기들 (캡션, 외각선 등)을 보정합니다.
	AdjustWindowRect(&rtClient, dwStyle, FALSE);

	//클라이언트가 생성될 좌표가 데스크톱의 중앙이 되도록 지정합니다.
	POINT ptClientPos;
	
	ptClientPos.x = (rtDesktop.right - rtClient.right) * 0.5f;
	ptClientPos.y = (rtDesktop.bottom - rtClient.bottom) * 0.5f;

	//cout << ptClientPos.x << ", " << ptClientPos.y << endl;

	HWND hWnd = CreateWindowW(
		szWindowClass,		//윈도우 클래스 명
		szTitle,			//타이틀에 입력될 문자열
		dwStyle,			//윈도우 스타일
		ptClientPos.x,		//응용 프로그램이 시작될 x좌표
		ptClientPos.y,		//응용 프로그램이 시작될 y좌표
		rtClient.right - rtClient.left,//응용 프로그램의 가로 길이
		rtClient.bottom - rtClient.top,//응용 프로그램의 세로 길이 
		nullptr,			//부모 윈도우
		nullptr,			//메뉴 핸들
		hInstance,			//인스턴스 핸들 
		nullptr				//추가 파라메터
	);

	// 윈도우가 정상적으로 생성되지 않았다면 프로그램을 종료하도록 합니다.
	if (!hWnd)
	{
		return FALSE;
	}

	gFramework.init(hWnd, hInst);

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	// 소켓 통신 스레드 생성
	CreateThread(NULL, 0, ClientMain,
		reinterpret_cast<LPVOID>(&gFramework), 0, NULL);
	return TRUE;
}

//
//  함수: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  목적:  주 창의 메시지를 처리합니다.
//
//  WM_COMMAND  - 응용 프로그램 메뉴를 처리합니다.
//  WM_PAINT    - 주 창을 그립니다.
//  WM_DESTROY  - 종료 메시지를 게시하고 반환합니다.
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		return gFramework.ProcessWindowInput(hWnd, message, wParam, lParam);
	}
    return 0;
}