//sesui_packer:ignore
#include <windows.h>
#include "../sesui.hpp"

//sesui_packer:resume
bool sesui::begin_tabs ( int count, float width ) {
	const auto window = globals::window_ctx.find ( globals::cur_window );

	if ( window == globals::window_ctx.end ( ) )
		throw "Current window context not valid.";

	const auto titlebar_rect = rect ( window->second.bounds.x, window->second.bounds.y, window->second.bounds.w, window->second.bounds.h * style.titlebar_height + 6.0f );
	const auto window_rect = rect ( window->second.bounds.x, window->second.bounds.y + scale_dpi ( titlebar_rect.h - 6.0f ), window->second.bounds.w, window->second.bounds.h - titlebar_rect.h + 6.0f );

	window->second.tab_width = window->second.bounds.w * static_cast< float > ( width );
	window->second.tab_count = count;

	const auto tab_rect = rect ( window_rect.x, window_rect.y, window->second.tab_width, window_rect.h );

	draw_list.add_rounded_rect ( tab_rect, style.rounding, style.window_accent, true );

	const auto cover_rect = rect ( tab_rect.x, tab_rect.y - 4.0f, window->second.tab_width, 8.0f );

	draw_list.add_rect ( cover_rect, style.window_accent, true );

	const auto cover_rect1 = rect ( tab_rect.x + scale_dpi( tab_rect.w ) - scale_dpi( 8.0f ), tab_rect.y, 8.0f, tab_rect.h );

	draw_list.add_rect ( cover_rect1, style.window_accent, true );

	window->second.main_area = rect ( window_rect.x + scale_dpi ( window->second.tab_width ) + scale_dpi ( style.initial_offset.x ), window->second.bounds.y + scale_dpi ( titlebar_rect.h + style.initial_offset.y ), window->second.bounds.w - window->second.tab_width - style.initial_offset.x * 2.0f, window->second.bounds.h - titlebar_rect.h - style.initial_offset.y * 2.0f );
	window->second.cursor_stack.back()= vec2 ( window->second.main_area.x, window->second.main_area.y );

	const auto tab_dim = vec2 ( window->second.tab_width, window_rect.h / static_cast< float >( window->second.tab_count ) );
	const auto tab_pos = window_rect.y + window->second.selected_tab_offset;

	draw_list.add_rect ( rect ( window_rect.x, tab_pos - scale_dpi ( 32.0f ) / 2.0f, tab_dim.x, 32.0f ), color ( 0, 0, 0, 50 ), true );

	return true;
}

void sesui::tab ( const ses_string& name, int& selected ) {
	const auto window = globals::window_ctx.find ( globals::cur_window );

	if ( window == globals::window_ctx.end ( ) )
		throw "Current window context not valid.";

	const auto titlebar_rect = rect ( window->second.bounds.x, window->second.bounds.y, window->second.bounds.w, window->second.bounds.h * style.titlebar_height + 6.0f );
	const auto window_rect = rect ( window->second.bounds.x, window->second.bounds.y + scale_dpi ( titlebar_rect.h - 6.0f ), window->second.bounds.w, window->second.bounds.h - titlebar_rect.h + 6.0f );
	const auto tab_dim = vec2 ( window->second.tab_width, window_rect.h / static_cast< float >( window->second.tab_count ) );
	const auto tab_pos = window_rect.y + scale_dpi( tab_dim.y * window->second.cur_tab_index );
	const auto text_pos = tab_pos + scale_dpi ( tab_dim.y ) / 2.0f;

	const auto parts = split ( name.get ( ) );
	const auto title = ses_string ( parts.first.data ( ) );
	const auto& id = parts.second;

	vec2 text_size;
	draw_list.get_text_size ( style.control_font, title, text_size );

	if ( input::mouse_in_region ( rect ( window_rect.x, tab_pos, tab_dim.x, tab_dim.y ) ) && input::key_pressed(VK_LBUTTON) ) {
		selected = window->second.cur_tab_index;

		/* reset animations */
		for ( auto& anim_time : window->second.anim_time )
			anim_time = 0.0f;
	}

	if ( selected == window->second.cur_tab_index ) {
		const auto delta = std::fabsf( ( text_pos - window_rect.y ) - window->second.selected_tab_offset );

		if ( window->second.selected_tab_offset < text_pos - window_rect.y )
			window->second.selected_tab_offset += delta * style.animation_speed * 3.0f * draw_list.get_frametime ( );
		else
			window->second.selected_tab_offset -= delta * style.animation_speed * 3.0f * draw_list.get_frametime ( );
	}

	draw_list.add_text ( vec2 ( window_rect.x + scale_dpi ( tab_dim.x ) / 2.0f - text_size.x / 2.0f, text_pos - text_size.y / 2.0f ), style.control_font, title, true, style.control_text );

	window->second.cur_tab_index++;
}

void sesui::end_tabs ( ) {
	const auto window = globals::window_ctx.find ( globals::cur_window );

	if ( window == globals::window_ctx.end ( ) )
		throw "Current window context not valid.";

	window->second.cur_tab_index = 0;
	window->second.tab_count = 0;
}