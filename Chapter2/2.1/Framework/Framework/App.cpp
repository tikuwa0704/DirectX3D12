//-------------------------------------------------------------
// includes
//-------------------------------------------------------------
#include"App.h"
#include <cassert>

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

	// Direct3D 12�̏�����.
	if (!InitD3D())
	{
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

	// Direct3D 12�̏I������.
	TermD3D();

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
		else {

			Render();

		}
	}
}


//-------------------------------------------------------------
//Direct3D�̏���������
//-------------------------------------------------------------
bool App::InitD3D()
{
	//�f�o�C�X�̐���
	auto hr = D3D12CreateDevice(
		nullptr,
		D3D_FEATURE_LEVEL_11_0,
		IID_PPV_ARGS(&m_pDevice));

	if (FAILED(hr)) {
		return false;
	}

	//�R�}���h�L���[�̐���
	{
		D3D12_COMMAND_QUEUE_DESC desc = {};
		desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
		desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		desc.NodeMask = 0;
		
		hr = m_pDevice->CreateCommandQueue(&desc, IID_PPV_ARGS(&m_pQueue));
		if (FAILED(hr)) {
			return false;
		}

	}


	//�X���b�v�`�F�C���̐���
	{
		//DXGI�t�@�N�g���[�̐���
		IDXGIFactory4* pFactory = nullptr;
		hr = CreateDXGIFactory1(IID_PPV_ARGS(&pFactory));

		if (FAILED(hr)) {
			return false;
		}

		//�X���b�v�`�F�C���̐ݒ�
		DXGI_SWAP_CHAIN_DESC desc = {};
		desc.BufferDesc.Width = m_Width;
		desc.BufferDesc.Height = m_Height;
		desc.BufferDesc.RefreshRate.Numerator = 60;
		desc.BufferDesc.RefreshRate.Denominator = 1;
		desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		desc.BufferCount = FrameCount;
		desc.OutputWindow = m_hWnd;
		desc.Windowed = TRUE;
		desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;


		//�X���b�v�`�F�C���̐���
		IDXGISwapChain* pSwapChain = nullptr;
		hr = pFactory->CreateSwapChain(m_pQueue, &desc, &pSwapChain);
		if (FAILED(hr)) {

			if (pFactory)
			{
				(pFactory)->Release();
				pFactory = NULL;
			}
			return false;
		}
		
		//IDXGISwapChain3���擾
		hr = pSwapChain->QueryInterface(IID_PPV_ARGS(&m_pSwapChain));
		if (FAILED(hr)) {

			if (pFactory)
			{
				(pFactory)->Release();
				pFactory = NULL;
			}

			if (pSwapChain)
			{
				(pSwapChain)->Release();
				pSwapChain = NULL;
			}

			return false;
		}
		
		//�o�b�N�o�b�t�@�ԍ����擾
		m_FrameIndex = m_pSwapChain->GetCurrentBackBufferIndex();

		//�s�v�ɂȂ����̂ŉ��

		if (pFactory)
		{
			(pFactory)->Release();
			pFactory = NULL;
		}

		if (pSwapChain)
		{
			(pSwapChain)->Release();
			pSwapChain = NULL;
		}

	}


	//�R�}���h�A���P�[�^�[�̐���
	{
		for (auto i = 0u; i < FrameCount; ++i) {

			hr = m_pDevice->CreateCommandAllocator(
				D3D12_COMMAND_LIST_TYPE_DIRECT,
				IID_PPV_ARGS(&m_pCmdAllocator[i])
			);
			if (FAILED(hr)) {
				return false;
			}

		}
	}

	//�R�}���h���X�g�̐���
	{
		hr = m_pDevice->CreateCommandList(
			0,
			D3D12_COMMAND_LIST_TYPE_DIRECT,
			m_pCmdAllocator[m_FrameIndex],
			nullptr,
			IID_PPV_ARGS(&m_pCmdList)
		);
		if (FAILED(hr)) {
			return false;
		}
	}

	//�����_�[�^�[�Q�b�g�r���[�̐���
	{
		//�f�B�X�N���v�^�q�[�v�̐ݒ�
		D3D12_DESCRIPTOR_HEAP_DESC desc = {};
		desc.NumDescriptors = FrameCount;
		desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		desc.NodeMask = 0;


		//�f�B�X�N���v�^�q�[�v�̐���
		hr = m_pDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&m_pHeapRTV));
		if (FAILED(hr)) {
			return false;
		}


		auto handle = m_pHeapRTV->GetCPUDescriptorHandleForHeapStart();
		auto incrementSize = m_pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		
		for (auto i = 0u; i < FrameCount; ++i) {

			hr = m_pSwapChain->GetBuffer(i, IID_PPV_ARGS(&m_pColorBuffer[i]));
			if (FAILED(hr)) {
				return false;
			}

			D3D12_RENDER_TARGET_VIEW_DESC viewDesc = {};
			viewDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
			viewDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
			viewDesc.Texture2D.MipSlice = 0;
			viewDesc.Texture2D.PlaneSlice = 0;

			//�����_�[�^�[�Q�b�g�r���[�̐���
			m_pDevice->CreateRenderTargetView(m_pColorBuffer[i], &viewDesc, handle);

			m_HandleRTV[i] = handle;
			handle.ptr += incrementSize;

		}

	}

	//�t�F���X�̐���
	{
		//�t�F���X�J�E���^�[�����Z�b�g
		for (auto i = 0u; i < FrameCount; ++i) {
			m_FenceCounter[i] = 0;
		}

		//�t�F���X�̐���
		hr = m_pDevice->CreateFence(
			m_FenceCounter[m_FrameIndex],
			D3D12_FENCE_FLAG_NONE,
			IID_PPV_ARGS(&m_pFence)
		);
		if (FAILED(hr)) {
			return false;
		}

		m_FenceCounter[m_FrameIndex]++;

		//�C�x���g�̐���
		m_FenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
		if (m_FenceEvent == nullptr) {
			return false;
		}

	}

	//�R�}���h���X�g�����
	m_pCmdList->Close();

	return true;
}

//-------------------------------------------------------------
//Direct3D�̏I������
//-------------------------------------------------------------
void App::TermD3D()
{
	//GPU�����̊�����ҋ@
	WaitGpu();


	//�C�x���g�j��
	if (m_FenceEvent != nullptr) {

		CloseHandle(m_FenceEvent);
		m_FenceEvent = nullptr;

	}

	//�t�F���X�j��
	SafeRelease(m_pFence);

	//�����_�[�^�[�Q�b�g�r���[�̔j��
	SafeRelease(m_pHeapRTV);
	for(auto i=0u;i<FrameCount;++i){
		SafeRelease(m_pColorBuffer[i]);
	}

	//�R�}���h���X�g�̔j��
	SafeRelease(m_pCmdList);


	//�R�}���h�A���P�[�^�[�̔j��
	for (auto i = 0u; i < FrameCount; ++i) {
		SafeRelease(m_pCmdAllocator[i]);
	}
	
	//�X���b�v�`�F�C���̔j��
	SafeRelease(m_pSwapChain);

	//�R�}���h�L���[�̔j��
	SafeRelease(m_pQueue);

	//�f�o�C�X�̔j��
	SafeRelease(m_pDevice);
}


//-------------------------------------------------------------
//�`�揈��
//-------------------------------------------------------------
void App::Render()
{
	//�R�}���h�̋L�^���J�n
	m_pCmdAllocator[m_FrameIndex]->Reset();
	m_pCmdList->Reset(m_pCmdAllocator[m_FrameIndex], nullptr);

	//���\�[�X�o���A�̐ݒ�
	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = m_pColorBuffer[m_FrameIndex];
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

	//���\�[�X�o���A
	m_pCmdList->ResourceBarrier(1, &barrier);


	//�����_�[�^�[�Q�b�g�̐ݒ�
	m_pCmdList->OMSetRenderTargets(1, &m_HandleRTV[m_FrameIndex], FALSE, nullptr);


	//�N���A�J���[�̐ݒ�
	float clearColor[] = { 0.25f, 0.25f, 0.25f, 1.0f };

	//�����_�[�^�[�Q�b�g�r���[���N���A
	m_pCmdList->ClearRenderTargetView(m_HandleRTV[m_FrameIndex], clearColor, 0, nullptr);


	//�`�揈��
	{
		//�@TODO�F�|���S���`��p�̏���
	}

	//���\�[�X�o���A�̐ݒ�
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = m_pColorBuffer[m_FrameIndex];
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;


	//���\�[�X�o���A
	m_pCmdList->ResourceBarrier(1, &barrier);


	//�R�}���h�̋L�^�I��
	m_pCmdList->Close();


	//�R�}���h�����s
	ID3D12CommandList* ppCmdList[] = { m_pCmdList };
	m_pQueue->ExecuteCommandLists(1, ppCmdList);


	Present(1);

}

//-------------------------------------------------------------
//GPU�̏���������ҋ@
//-------------------------------------------------------------
void App::WaitGpu()
{
	assert(m_pQueue != nullptr);
	assert(m_pFence != nullptr);
	assert(m_FenceEvent != nullptr);

	//�V�O�i������
	m_pQueue->Signal(m_pFence, m_FenceCounter[m_FrameIndex]);

	//������ɃC�x���g��ݒ肷��
	m_pFence->SetEventOnCompletion(m_FenceCounter[m_FrameIndex], m_FenceEvent);

	//�ҋ@����
	WaitForSingleObjectEx(m_FenceEvent, INFINITE, FALSE);

	//�J�E���^�[�𑝂₷
	m_FenceCounter[m_FrameIndex]++;


}


//-------------------------------------------------------------
//��ʂɕ\��
//-------------------------------------------------------------
void App::Present(uint32_t interval)
{
	//��ʂɕ\��
	m_pSwapChain->Present(interval, 0);
	
	//�V�O�i������
	const auto currentValue = m_FenceCounter[m_FrameIndex];
	m_pQueue->Signal(m_pFence, currentValue);

	//�o�b�N�o�b�t�@�ԍ����X�V
	m_FrameIndex = m_pSwapChain->GetCurrentBackBufferIndex();

	//���̃t���[���̕`�揀�����܂��ł���Αҋ@����
	if (m_pFence->GetCompletedValue() < m_FenceCounter[m_FrameIndex]) {

		m_pFence->SetEventOnCompletion(m_FenceCounter[m_FrameIndex], m_FenceEvent);
		WaitForSingleObjectEx(m_FenceEvent, INFINITE, FALSE);

	}

	//���̃t���[���̃t�F���X�J�E���^�[�𑝂₷
	m_FenceCounter[m_FrameIndex] = currentValue + 1;

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
