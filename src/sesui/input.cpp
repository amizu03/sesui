//sesui_packer:ignore
#include <windows.h>
#include "sesui.hpp"

//sesui_packer:resume
std::array< bool, 256 > sesui::input::key_state { false };
std::array< bool, 256 > sesui::input::old_key_state { false };
sesui::vec2 sesui::input::mouse_pos = vec2 ( );
sesui::vec2 sesui::input::start_click_pos = vec2( );

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

	if ( key_pressed ( VK_LBUTTON ) )
		start_click_pos = { pos.x, pos.y };
}

bool sesui::input::key_pressed ( int key ) {
	return key_state [ key ] && !old_key_state [ key ];
}

bool sesui::input::key_down ( int key ) {
	return key_state [ key ];
}

bool sesui::input::key_released ( int key ) {
	return !key_state [ key ] && old_key_state [ key ];
}

bool sesui::input::mouse_in_region ( const rect& bounds ) {
	const auto scaled_w = scale_dpi ( bounds.w );
	const auto scaled_h = scale_dpi ( bounds.h );

	return mouse_pos.x >= bounds.x && mouse_pos.y >= bounds.y && mouse_pos.x <= bounds.x + scaled_w && mouse_pos.y <= bounds.y + scaled_h;
}

bool sesui::input::click_in_region ( const rect& bounds ) {
	const auto scaled_w = scale_dpi ( bounds.w );
	const auto scaled_h = scale_dpi ( bounds.h );

	return start_click_pos.x >= bounds.x && start_click_pos.y >= bounds.y && start_click_pos.x <= bounds.x + scaled_w && start_click_pos.y <= bounds.y + scaled_h;
}