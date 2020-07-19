﻿#include <d3d9.h>
#include <d3dx9.h>
#include <windows.h>
#include <cstdlib>
#include <memory>
#include <chrono>

#include "d3d9_render.hpp"
#include "sesui/sesui.hpp"

static LPDIRECT3D9 g_pD3D = nullptr;
static LPDIRECT3DDEVICE9 g_pd3dDevice = nullptr;
static D3DPRESENT_PARAMETERS g_d3dpp = {};

bool CreateDeviceD3D ( HWND hWnd );
void CleanupDeviceD3D ( );
void ResetDevice ( );
LRESULT WINAPI WndProc ( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );

unsigned long font_count = 0;

int main ( ) {
	unsigned long font_count = 0;
	AddFontMemResourceEx ( &sesui::fonts::font_default, sizeof sesui::fonts::font_default, nullptr, &font_count );

	/* bind draw list methods to our own drawing functions */
	sesui::draw_list.draw_polygon = render::polygon;
	sesui::draw_list.draw_text = render::text;
	sesui::draw_list.get_text_size = render::get_text_size;
	sesui::draw_list.get_frametime = render::get_frametime;
	sesui::draw_list.begin_clip = render::begin_clip;
	sesui::draw_list.end_clip = render::end_clip;
	sesui::draw_list.create_font = render::create_font;

	//sesui::style.window_background = sesui::color ( 255, 255, 255, 255 );
	//sesui::style.window_foreground = sesui::color ( 230, 230, 230, 255 );
	//sesui::style.window_borders = sesui::color ( 214, 214, 214, 255 );
	//sesui::style.window_accent = sesui::color ( 255, 0, 77, 255 );
	//sesui::style.window_accent_borders = sesui::color ( 255, 0 + 125, 77 + 125, 255 );
	//sesui::style.control_background = sesui::color ( 230, 230, 230, 255 );
	//sesui::style.control_borders = sesui::color ( 214, 214, 214, 255 );
	//sesui::style.control_text = sesui::color ( 26, 26, 26, 255 );
	//sesui::style.control_text_hovered = sesui::color ( 56, 56, 56, 255 );
	//sesui::style.control_accent = sesui::color ( 255, 0, 77, 255 );
	//sesui::style.control_accent_borders = sesui::color ( 255, 0 + 125, 77 + 125, 255 );

	WNDCLASSEX wc = { sizeof ( WNDCLASSEX ), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandleA ( nullptr ), nullptr, nullptr, nullptr, nullptr, "SESUI Test Window", nullptr };
	::RegisterClassExA ( &wc );
	HWND hwnd = ::CreateWindowExA ( 0, wc.lpszClassName, "SESUI Test Window", WS_OVERLAPPEDWINDOW, 100, 100, 1280, 800, nullptr, nullptr, wc.hInstance, nullptr );

	if ( !CreateDeviceD3D ( hwnd ) ) {
		CleanupDeviceD3D ( );
		::UnregisterClass ( wc.lpszClassName, wc.hInstance );
		return 1;
	}

	::ShowWindow ( hwnd, SW_SHOWDEFAULT );
	::UpdateWindow ( hwnd );

	MSG msg;
	memset ( &msg, 0, sizeof ( msg ) );

	uint64_t last_render_ms = 0;

	bool test_checkbox1 = false;
	bool test_checkbox2 = false;
	bool test_checkbox3 = false;
	bool test_checkbox4 = false;
	bool test_checkbox5 = false;
	float test_float_slider = 0.0f;
	double test_double_slider = 0.0;
	int test_int_slider = 0;

	/* gui data for testing */
	while ( msg.message != WM_QUIT ) {
		if ( ::PeekMessageA ( &msg, nullptr, 0U, 0U, PM_REMOVE ) ) {
			::TranslateMessage ( &msg );
			::DispatchMessageA ( &msg );
			continue;
		}

		render::device = g_pd3dDevice;

		g_pd3dDevice->SetVertexShader ( nullptr );
		g_pd3dDevice->SetPixelShader ( nullptr );
		g_pd3dDevice->SetVertexShader ( nullptr );
		g_pd3dDevice->SetPixelShader ( nullptr );
		g_pd3dDevice->SetFVF ( D3DFVF_XYZRHW | D3DFVF_DIFFUSE );
		g_pd3dDevice->SetRenderState ( D3DRS_LIGHTING, false );
		g_pd3dDevice->SetRenderState ( D3DRS_FOGENABLE, false );
		g_pd3dDevice->SetRenderState ( D3DRS_CULLMODE, D3DCULL_NONE );
		g_pd3dDevice->SetRenderState ( D3DRS_FILLMODE, D3DFILL_SOLID );
		g_pd3dDevice->SetRenderState ( D3DRS_ZENABLE, D3DZB_FALSE );
		g_pd3dDevice->SetRenderState ( D3DRS_SCISSORTESTENABLE, false );
		g_pd3dDevice->SetRenderState ( D3DRS_ZWRITEENABLE, false );
		g_pd3dDevice->SetRenderState ( D3DRS_STENCILENABLE, false );
		g_pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE, true );
		g_pd3dDevice->SetRenderState ( D3DRS_ALPHATESTENABLE, false );
		g_pd3dDevice->SetRenderState ( D3DRS_SEPARATEALPHABLENDENABLE, true );
		g_pd3dDevice->SetRenderState ( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
		g_pd3dDevice->SetRenderState ( D3DRS_SRCBLENDALPHA, D3DBLEND_INVDESTALPHA );
		g_pd3dDevice->SetRenderState ( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
		g_pd3dDevice->SetRenderState ( D3DRS_DESTBLENDALPHA, D3DBLEND_ONE );
		g_pd3dDevice->SetRenderState ( D3DRS_SRGBWRITEENABLE, false );
		g_pd3dDevice->SetRenderState ( D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_ALPHA );
		g_pd3dDevice->SetRenderState ( D3DRS_MULTISAMPLEANTIALIAS, true );
		g_pd3dDevice->SetRenderState ( D3DRS_ANTIALIASEDLINEENABLE, true );

		g_pd3dDevice->Clear ( 0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_RGBA ( 0, 0, 0, 255 ), 1.0f, 0 );

		if ( g_pd3dDevice->BeginScene ( ) >= 0 ) {

				sesui::begin_frame(L"SESUI Test Window");

				sesui::menu_key(VK_INSERT);

				if (sesui::globals::opened)
				{
					sesui::begin_window(L"SESUI Test Window", sesui::rect(200, 200, 600, 500)); {
						sesui::checkbox(L"onii", test_checkbox1);
						sesui::checkbox(L"is", test_checkbox2);
						sesui::checkbox(L"a", test_checkbox3);
						sesui::checkbox(L"fucking", test_checkbox4);
						sesui::checkbox(L"hoe", test_checkbox5);
						sesui::tooltip(L"Scales GUI appropriately");
						sesui::slider(L"GUI DPI", sesui::globals::dpi, 0.5f, 3.0f);
						sesui::tooltip(L"Percentage slider thingy");
						sesui::slider(L"float slider", test_float_slider, -180.0f, 180.0f, L"%.2f%%");
						sesui::slider(L"double slider", test_double_slider, 0.0, 100.0);
						sesui::tooltip(L"Angle (degree) slider");
						sesui::slider(L"int slider", test_int_slider, 0, 360, L"%d deg");

						sesui::end_window();
					}



					sesui::render();
					sesui::end_frame();
				}


			/* save current frametime */
			const auto ms = std::chrono::duration_cast< std::chrono::milliseconds >( std::chrono::system_clock::now ( ).time_since_epoch ( ) ).count ( );
			const auto delta_ms = ms - last_render_ms;
			render::frametime = static_cast< float > ( delta_ms ) / 1000.0f;
			last_render_ms = ms;

			g_pd3dDevice->EndScene ( );
		}

		HRESULT result = g_pd3dDevice->Present ( nullptr, nullptr, nullptr, nullptr );

		if ( result == D3DERR_DEVICELOST && g_pd3dDevice->TestCooperativeLevel ( ) == D3DERR_DEVICENOTRESET )
			ResetDevice ( );
	}

	CleanupDeviceD3D ( );
	::DestroyWindow ( hwnd );
	::UnregisterClassA ( wc.lpszClassName, wc.hInstance );

	return 0;
}

// Helper functions

bool CreateDeviceD3D ( HWND hWnd ) {
	if ( !( g_pD3D = Direct3DCreate9 ( D3D_SDK_VERSION ) ) )
		return false;

	// Create the D3DDevice
	ZeroMemory ( &g_d3dpp, sizeof ( g_d3dpp ) );
	g_d3dpp.Windowed = TRUE;
	g_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	g_d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
	g_d3dpp.EnableAutoDepthStencil = TRUE;
	g_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
	g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;

	if ( g_pD3D->CreateDevice ( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &g_d3dpp, &g_pd3dDevice ) < 0 )
		return false;

	return true;
}

void CleanupDeviceD3D ( ) {
	if ( g_pd3dDevice ) { g_pd3dDevice->Release ( ); g_pd3dDevice = nullptr; }
	if ( g_pD3D ) { g_pD3D->Release ( ); g_pD3D = nullptr; }
}

void ResetDevice ( ) {
	HRESULT hr = g_pd3dDevice->Reset ( &g_d3dpp );

	if ( hr == D3DERR_INVALIDCALL )
		abort();

	if ( sesui::style.control_font.data ) {
		reinterpret_cast< ID3DXFont* > ( sesui::style.control_font.data )->Release ( );
		sesui::style.control_font.data = nullptr;
	}
}

// Win32 message handler
LRESULT WINAPI WndProc ( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam ) {
	switch ( msg ) {
	case WM_SIZE:
		if ( g_pd3dDevice && wParam != SIZE_MINIMIZED ) {
			g_d3dpp.BackBufferWidth = LOWORD ( lParam );
			g_d3dpp.BackBufferHeight = HIWORD ( lParam );
			ResetDevice ( );
		}
		return 0;
	case WM_SYSCOMMAND:
		if ( ( wParam & 0xfff0 ) == SC_KEYMENU ) // Disable ALT application menu
			return 0;
		break;
	case WM_DESTROY:
		::PostQuitMessage ( 0 );
		return 0;
	}

	return ::DefWindowProcA ( hWnd, msg, wParam, lParam );
}
