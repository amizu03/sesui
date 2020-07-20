//sesui_packer:ignore
#include <windows.h>
#include "../sesui.hpp"

//sesui_packer:resume
bool sesui::begin_window ( const ses_string& name, const rect& bounds, bool& opened, uint32_t flags ) {
	if ( !opened )
		return false;

	const auto parts = split ( name.get ( ) );
	const auto title = ses_string ( parts.first.data ( ) );
	const auto& id = parts.second;

	/* set current window context */
	globals::cur_window = parts.first + id;

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
			globals::window_ctx [ globals::cur_window ].layer = top_layer + 1;
		else
			globals::window_ctx [ globals::cur_window ].layer = 0;

		globals::window_ctx [ globals::cur_window ].bounds = bounds;
		globals::window_ctx [ globals::cur_window ].anim_time = std::array< float, 256 > { 0.0f };
		globals::window_ctx [ globals::cur_window ].cur_index = 0;
		//globals::window_ctx [ globals::cur_window ].cur_tab_index = 0;
		globals::window_ctx [ globals::cur_window ].tab_count = 0;
		globals::window_ctx [ globals::cur_window ].selected_tab_offset = 0.0f;

		window_entry = globals::window_ctx.find ( globals::cur_window );
	}

	if ( window_entry == globals::window_ctx.end ( ) )
		throw "Current window context not valid.";

	window_entry->second.cur_group = L"";

	auto titlebar_rect = rect ( window_entry->second.bounds.x, window_entry->second.bounds.y, window_entry->second.bounds.w, window_entry->second.bounds.h * style.titlebar_height + 6.0f );

	/* window moving behavior */
	if ( !( flags & window_flags::no_move ) ) {
		if ( !window_entry->second.moving && input::mouse_in_region ( titlebar_rect ) && input::key_pressed ( VK_LBUTTON ) ) {
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
	}

	const auto resizing_area = rect ( window_entry->second.bounds.x + scale_dpi ( window_entry->second.bounds.w - style.resize_grab_radius ), window_entry->second.bounds.y + scale_dpi( window_entry->second.bounds.h - style.resize_grab_radius ), style.resize_grab_radius * 2.0f, style.resize_grab_radius * 2.0f );

	/* window resizing behavior */
	if ( !( flags & window_flags::no_resize ) ) {
		if ( !window_entry->second.resizing && input::mouse_in_region ( resizing_area ) && input::key_pressed ( VK_LBUTTON ) ) {
			window_entry->second.resizing = true;
		}
		else if ( window_entry->second.resizing && input::key_down ( VK_LBUTTON ) ) {
			window_entry->second.bounds.w = std::max< float > ( unscale_dpi ( input::mouse_pos.x - window_entry->second.bounds.x ), style.window_min_size.x );
			window_entry->second.bounds.h = std::max< float > ( unscale_dpi ( input::mouse_pos.y - window_entry->second.bounds.y ), style.window_min_size.y );
		}
		else {
			window_entry->second.resizing = false;
		}
	}

	titlebar_rect = rect ( window_entry->second.bounds.x, window_entry->second.bounds.y, window_entry->second.bounds.w, window_entry->second.bounds.h * style.titlebar_height + 6.0f );

	const auto window_rect = rect ( window_entry->second.bounds.x, window_entry->second.bounds.y + scale_dpi(titlebar_rect.h - 6.0f), window_entry->second.bounds.w, window_entry->second.bounds.h - titlebar_rect.h + 6.0f );
	const auto remove_rounding_rect = rect ( titlebar_rect.x, titlebar_rect.y + scale_dpi ( titlebar_rect.h - 6.0f), titlebar_rect.w, 6.0f * 2.0f );
	const auto remove_rounding_rect_filler = rect ( remove_rounding_rect.x + 1.0f, remove_rounding_rect.y + 1.0f, remove_rounding_rect.w -unscale_dpi( 2.0f), remove_rounding_rect.h + 6.0f );
	const auto exit_rect = rect( window_entry->second.bounds.x + scale_dpi ( window_entry->second.bounds.w - 16.0f - 8.0f ), window_entry->second.bounds.y + scale_dpi ( titlebar_rect.h - 6.0f ) * 0.5f - scale_dpi ( 8.0f ), 16.0f, 16.0f );

	/* window rect */
	draw_list.add_rounded_rect ( titlebar_rect, style.rounding, style.window_accent, true );
	draw_list.add_rounded_rect ( titlebar_rect, style.rounding, style.window_accent_borders, false );
	draw_list.add_rounded_rect ( window_rect, style.rounding, style.window_background, true );
	draw_list.add_rounded_rect ( window_rect, style.rounding, style.window_borders, false );

	/* covering rounding for top part of main window */
	draw_list.add_rect ( remove_rounding_rect, style.window_background, true );
	draw_list.add_rect ( remove_rounding_rect, style.window_borders, false );
	draw_list.add_rect ( remove_rounding_rect_filler, style.window_background, true );

	if ( !( flags & window_flags::no_title ) ) {
		vec2 text_size;
		draw_list.get_text_size ( style.control_font, title, text_size );

		draw_list.add_text(vec2(window_entry->second.bounds.x + scale_dpi(style.spacing), window_entry->second.bounds.y + scale_dpi ( titlebar_rect.h - 6.0f ) * 0.5f - text_size.y * 0.5f ), style.control_font, title, true, color(200, 200, 200, 255));
	}

	/* close menu button*/
	if ( !( flags & window_flags::no_closebutton ) ) {
		draw_list.add_rounded_rect ( exit_rect, 6.0f, color ( 30, 30, 30, 255 ), true );
		draw_list.add_rounded_rect ( exit_rect, 6.0f, style.window_accent, false ); /* rounding looks more smooth with outline for some reason wtf */

		if ( input::mouse_in_region ( exit_rect ) )
			draw_list.add_rounded_rect ( exit_rect, 3.0f, color ( static_cast< int >( style.window_accent.r ), static_cast< int >( style.window_accent.g ), static_cast< int >( style.window_accent.b ), 50 ), true );

		if ( input::mouse_in_region ( exit_rect ) && input::key_pressed ( VK_LBUTTON ) )
			opened = false;

		draw_list.add_line ( vec2 ( exit_rect.x + scale_dpi ( exit_rect.w ) * 0.333f, exit_rect.y + scale_dpi ( exit_rect.h ) * 0.333f ), vec2 ( exit_rect.x + scale_dpi ( exit_rect.w ) * 0.666f, exit_rect.y + scale_dpi ( exit_rect.h ) * 0.666f ), color ( 255, 255, 255, 255 ) );
		draw_list.add_line ( vec2 ( exit_rect.x + scale_dpi ( exit_rect.w ) * 0.333f, exit_rect.y + scale_dpi ( exit_rect.h ) * 0.666f ), vec2 ( exit_rect.x + scale_dpi ( exit_rect.w ) * 0.666f, exit_rect.y + scale_dpi ( exit_rect.h ) * 0.333f ), color ( 255, 255, 255, 255 ) );
	}
	
	if ( window_entry->second.resizing ) {
		draw_list.add_arrow ( vec2( window_entry->second.bounds.x + scale_dpi( window_entry->second.bounds.w - 6.0f ), window_entry->second.bounds.y + scale_dpi( window_entry->second.bounds.h - 6.0f ) ), 6.0f, -135.0f, style.window_accent, true, true );
	}

	window_entry->second.main_area = rect ( window_entry->second.bounds.x + scale_dpi ( style.initial_offset.x ), window_entry->second.bounds.y + scale_dpi ( titlebar_rect.h + style.initial_offset.y ), window_entry->second.bounds.w - style.initial_offset.x * 2.0f, window_entry->second.bounds.h - titlebar_rect.h - style.initial_offset.y * 2.0f );
	window_entry->second.cursor_stack.push_back( vec2( window_entry->second.main_area.x, window_entry->second.main_area.y ));

	return true;
}

void sesui::same_line ( ) {
	auto window = globals::window_ctx.find ( globals::cur_window );

	if ( window == globals::window_ctx.end ( ) )
		throw "Current window context not valid.";

	window->second.same_line = true;
}

void sesui::end_window ( ) {
	auto window_entry = globals::window_ctx.find ( globals::cur_window );

	if ( window_entry == globals::window_ctx.end ( ) )
		throw "Current window context not valid.";

	/* draw tooltip if we have one */
	if ( !window_entry->second.selected_tooltip.empty( ) ) {
		vec2 text_size;
		draw_list.get_text_size ( style.control_font, window_entry->second.selected_tooltip.data( ), text_size );

		rect bounds = rect ( input::mouse_pos.x - scale_dpi( style.padding ), input::mouse_pos.y - scale_dpi (style.padding), unscale_dpi(text_size.x )+ style.padding * 2.0f,  unscale_dpi(text_size.y) + style.padding * 2.0f );

		bounds.y -= scale_dpi( bounds.h);  

		draw_list.add_rounded_rect ( bounds, style.rounding, color ( style.window_foreground.r, style.window_foreground.g, style.window_foreground.b, static_cast< uint8_t >( 0 ) ).lerp ( style.window_foreground, window_entry->second.tooltip_anim_time ), true, true );
		draw_list.add_rounded_rect ( bounds, style.rounding, color ( style.window_borders.r, style.window_borders.g, style.window_borders.b, static_cast< uint8_t >( 0 ) ).lerp ( style.window_borders, window_entry->second.tooltip_anim_time ), false, true );
		draw_list.add_text ( vec2( bounds .x + scale_dpi(style.padding), bounds .y + scale_dpi(style.padding )), style.control_font, window_entry->second.selected_tooltip.data ( ), false, color ( style.control_text.r, style.control_text.g, style.control_text.b, static_cast< uint8_t >( 0 ) ).lerp ( style.control_text, window_entry->second.tooltip_anim_time ), true );
	}
	else {
		window_entry->second.tooltip_anim_time = 0.0f;
	}

	window_entry->second.selected_tooltip.clear ( );
	window_entry->second.cur_index = 0;
	window_entry->second.cur_tab_index = 0;
	window_entry->second.tab_count = 0;
	window_entry->second.cursor_stack.pop_back();

	if ( !window_entry->second.cursor_stack.empty ( ) )
		throw "Cursor stack was not empty at end of frame. Did you forget to call sesui::end_window or sesui::end_group?";

	globals::cur_window = L"";
}