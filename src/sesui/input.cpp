//sesui_packer:ignore
#include <windows.h>
#include "sesui.hpp"

//sesui_packer:resume
std::array< bool, 256 > sesui::input::key_state { false };
std::array< bool, 256 > sesui::input::old_key_state { false };
sesui::vec2 sesui::input::mouse_pos = vec2 ( );
sesui::vec2 sesui::input::start_click_pos = vec2( );
bool sesui::input::queue_enable = false;
bool sesui::input::enabled = true;
float sesui::input::scroll_amount = 0.0f;

void sesui::input::get_scroll_amount ( const float scroll_amount ) {
	sesui::input::scroll_amount = scroll_amount;
}

void sesui::input::enable_input ( bool enabled ) {
	if ( enabled )
		queue_enable = true;
	else
		sesui::input::enabled = false;
}

void sesui::input::get_input ( const ses_string& window ) {
	HWND hwnd = FindWindowW ( nullptr, window.get ( ) );

	if ( !hwnd )
		throw std::exception ( "Failed to find target input window." );

	for ( int i = 0; i < 256; i++ ) {
		old_key_state [ i ] = key_state [ i ];
		key_state [ i ] = GetAsyncKeyState ( i );
	}

	POINT pos;
	GetCursorPos ( &pos );
	ScreenToClient ( hwnd, &pos );

	mouse_pos = { pos.x, pos.y };

	if ( key_state [ VK_LBUTTON ] && !old_key_state [ VK_LBUTTON ] )
		start_click_pos = { pos.x, pos.y };
}

bool sesui::input::key_pressed ( int key ) {
	return key_state [ key ] && !old_key_state [ key ] && enabled;
}

bool sesui::input::key_down ( int key ) {
	return key_state [ key ] && enabled;
}

bool sesui::input::key_released ( int key ) {
	return !key_state [ key ] && old_key_state [ key ] && enabled;
}

bool sesui::input::in_clip ( const vec2& pos ) {
	return globals::clip_enabled && pos.x >= globals::clip.x && pos.y >= globals::clip.y && pos.x <= globals::clip.x + scale_dpi ( globals::clip.w ) && pos.y <= globals::clip.y + scale_dpi ( globals::clip.h );
}

bool sesui::input::mouse_in_region ( const rect& bounds, bool force ) {
	const auto scaled_w = scale_dpi ( bounds.w );
	const auto scaled_h = scale_dpi ( bounds.h );

	if ( globals::clip_enabled && !( mouse_pos.x >= globals::clip.x && mouse_pos.y >= globals::clip.y && mouse_pos.x <= globals::clip.x + scale_dpi ( globals::clip.w ) && mouse_pos.y <= globals::clip.y + scale_dpi ( globals::clip.h ) ) )
		return false;

	return mouse_pos.x >= bounds.x && mouse_pos.y >= bounds.y && mouse_pos.x <= bounds.x + scaled_w && mouse_pos.y <= bounds.y + scaled_h && ( force ? true : enabled );
}

bool sesui::input::click_in_region ( const rect& bounds, bool force ) {
	const auto scaled_w = scale_dpi ( bounds.w );
	const auto scaled_h = scale_dpi ( bounds.h );

	if ( globals::clip_enabled && !( mouse_pos.x >= globals::clip.x && mouse_pos.y >= globals::clip.y && mouse_pos.x <= globals::clip.x + scale_dpi ( globals::clip.w ) && mouse_pos.y <= globals::clip.y + scale_dpi ( globals::clip.h ) ) )
		return false;

	return start_click_pos.x >= bounds.x && start_click_pos.y >= bounds.y && start_click_pos.x <= bounds.x + scaled_w && start_click_pos.y <= bounds.y + scaled_h && ( force ? true : enabled );
}