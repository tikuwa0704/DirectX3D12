//-------------------------------------------------------------
// includes
//-------------------------------------------------------------
#include"App.h"

namespace /* anonymous */ {

	const auto ClassName = TEXT("SampleWindowClass");  //!< �E�B���h�E�N���X��

}// namespace /* anonymous */

//-------------------------------------------------------------
//�@�R���X�g���N�^
//-------------------------------------------------------------
App::App(uint32_t width, uint32_t height)
	: m_hInst(nullptr)
	, m_hWnd(nullptr)
	, m_Width(width)
	, m_Height(height)
{/* DO_NOTHING */ }

App::~App()
{/* DO_NOTHING */}

//-------------------------------------------------------------
//  ���s
//-------------------------------------------------------------
void App::Run()
{
	if (InitApp()) {
		MainLoop();
	}

	TermApp();
}

//-------------------------------------------------------------
//�@����������
//-------------------------------------------------------------
bool App::InitApp()
{
	//�E�B���h�E�̏�����
	if (!InitWnd()) {
		return false;
	}

	//����I��
	return true;
}

//-------------------------------------------------------------
//�I������
//-------------------------------------------------------------
void App::TermApp()
{
	//�E�B���h�E�̏I������
	TermWnd();
}


//-------------------------------------------------------------
//�E�B���h�E�̏���������
//-------------------------------------------------------------
bool App::InitWnd()
{
	//�C���X�^���X�n���h�����擾
	auto hInst = GetModuleHandle(nullptr);
	if(hInst == nullptr)
	{
		return false;
	}

	//�E�B���h�E�̐ݒ�
	WNDCLASSEX wc = {};
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WndProc;
	wc.hIcon = LoadIcon(hInst, IDI_APPLICATION);
	wc.hCursor = LoadCursor(hInst, IDC_ARROW);
	wc.hbrBackground = GetSysColorBrush(COLOR_BACKGROUND);
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = ClassName;
	wc.hIconSm = LoadIcon(hInst, IDI_APPLICATION);

	//�E�B���h�E�̓o�^
	if(!RegisterClassEx(&wc))
	{
		return false;
	}

	//�C���X�^���X�n���h���ݒ�
	m_hInst = hInst;

	//�E�B���h�E�̃T�C�Y�̐ݒ�
	RECT rc = {};
	rc.right = static_cast<LONG>(m_Width);
	rc.bottom = static_cast<LONG>(m_Height);

	//�E�B���h�E�T�C�Y�𒲐�
	auto style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU;
	AdjustWindowRect(&rc, style, FALSE);

	//�E�B���h�E�𐶐�
	m_hWnd = CreateWindowEx(
		0,
		ClassName,
		TEXT("Sample"),
		style,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		rc.right - rc.left,
		rc.bottom - rc.top,
		nullptr,
		nullptr,
		m_hInst,
		nullptr);


	if (m_hWnd == nullptr)
	{
		return false;
	}

	//�E�B���h�E��\��
	ShowWindow(m_hWnd, SW_SHOWNORMAL);

	//�E�B���h�E���X�V
	UpdateWindow(m_hWnd);

	//�E�B���h�E�Ƀt�H�[�J�X��ݒ�
	SetFocus(m_hWnd);

	//����I��
	return true;
}

//-------------------------------------------------------------
//�E�B���h�E�̏I������
//-------------------------------------------------------------
void App::TermWnd()
{
	//�E�B���h�E�̓o�^������
	if (m_hInst != nullptr)
	{
		UnregisterClass(ClassName, m_hInst);
	}

	m_hInst = nullptr;
	m_hWnd = nullptr;
}

//-------------------------------------------------------------
//���C�����[�v
//-------------------------------------------------------------
void App::MainLoop()
{
	MSG msg = {};
	while(WM_QUIT != msg.message) {

		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE) == TRUE)
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
}

LRESULT App::WndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	switch (msg)
	{
		case WM_DESTROY:
		{ PostQuitMessage(0); }
		break;
		default:
		{/* DO_NOTHING */}
		break;
	}

	return DefWindowProc(hWnd,msg,wp,lp);
}
