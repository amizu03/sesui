//sesui_packer:ignore
#include <windows.h>
#include "../sesui.hpp"

//sesui_packer:resume
void sesui::begin_window ( const ses_string& title, const  rect& bounds ) {
	/* set current window context */
	globals::cur_window = title;

	auto window_entry = globals::window_ctx.find ( title.get ( ) );

	if ( window_entry == globals::window_ctx.end ( ) ) {
		int top_layer = -1;

		/* add on top of all other windows create before this one */
		if ( !globals::window_ctx.empty ( ) ) {
			for ( const auto& window : globals::window_ctx )
				if ( window.second.layer > top_layer )
					top_layer = window.second.layer;
		}

		if ( top_layer != -1 )
			globals::window_ctx [ title.get ( ) ].layer = top_layer + 1;
		else
			globals::window_ctx [ title.get ( ) ].layer = 0;

		globals::window_ctx [ title.get ( ) ].bounds = bounds;

		window_entry = globals::window_ctx.find ( title.get ( ) );
	}

	auto titlebar_rect = rect ( window_entry->second.bounds.x, window_entry->second.bounds.y, window_entry->second.bounds.w, window_entry->second.bounds.h * style.titlebar_height + 6.0f );

	if ( !window_entry->second.moving && input::mouse_in_region( titlebar_rect ) && input::key_pressed (VK_LBUTTON) ) {
		window_entry->second.moving = true;
		window_entry->second.click_offset = vec2 ( input::mouse_pos.x - window_entry->second.bounds.x, input::mouse_pos.y - window_entry->second.bounds.y );
	}
	else if ( window_entry->second.moving && input::key_down ( VK_LBUTTON ) ) {
		window_entry->second.bounds.x = input::mouse_pos.x - window_entry->second.click_offset.x;
		window_entry->second.bounds.y = input::mouse_pos.y - window_entry->second.click_offset.y;
	}
	else {
		window_entry->second.moving = false;
	}

	titlebar_rect = rect ( window_entry->second.bounds.x, window_entry->second.bounds.y, window_entry->second.bounds.w, window_entry->second.bounds.h * style.titlebar_height + 6.0f );

	const auto window_rect = rect ( window_entry->second.bounds.x, window_entry->second.bounds.y + titlebar_rect.h - 6.0f, window_entry->second.bounds.w, window_entry->second.bounds.h - titlebar_rect.h + 6.0f );
	const auto remove_rounding_rect = rect ( titlebar_rect.x, titlebar_rect.y + titlebar_rect.h - 6.0f, titlebar_rect.w, 6.0f * 2.0f );
	const auto remove_rounding_rect_filler = rect ( remove_rounding_rect.x + 1.0f, remove_rounding_rect.y + 1.0f, remove_rounding_rect.w - 2.0f, remove_rounding_rect.h + 6.0f );

	/* window rect */
	draw_list.add_rounded_rect ( titlebar_rect, style.rounding, style.window_accent, true );
	draw_list.add_rounded_rect ( titlebar_rect, style.rounding, style.window_accent_borders, false );
	draw_list.add_rounded_rect ( window_rect, style.rounding, style.window_background, true );
	draw_list.add_rounded_rect ( window_rect, style.rounding, style.window_borders, false );

	/* covering rounding for top part of main window */
	draw_list.add_rect ( remove_rounding_rect, style.window_background, true );
	draw_list.add_rect ( remove_rounding_rect, style.window_borders, false );
	draw_list.add_rect ( remove_rounding_rect_filler, style.window_background, true );
}

void sesui::end_window ( ) {

}