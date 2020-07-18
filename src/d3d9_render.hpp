#pragma once
#include <cstdint>
#include <string_view>
#include <functional>
#include <d3d9.h>
#include <d3dx9.h>
#include <windows.h>
#include "sesui/sesui.hpp"

namespace render {
	extern float frametime;
	extern IDirect3DDevice9* device;

	void polygon ( const std::vector< sesui::vec2 >& verticies, const sesui::color& color, bool filled ) noexcept;
	void text ( const sesui::vec2& pos, const sesui::font& font, const sesui::ses_string& text, const sesui::color& color ) noexcept;
	void get_text_size ( const sesui::font& font, const sesui::ses_string& text, sesui::vec2& dim_out ) noexcept;
	float get_frametime ( ) noexcept;
	void begin_clip ( const sesui::rect& region ) noexcept;
	void end_clip ( ) noexcept;
}