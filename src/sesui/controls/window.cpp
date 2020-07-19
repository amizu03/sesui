//sesui_packer:ignore
#include <windows.h>
#include "../sesui.hpp"

void sesui::menu_key( int vK )
{
	if ( input::key_pressed(vK) )
		sesui::globals::opened = !sesui::globals::opened;
}

//sesui_packer:resume
void sesui::begin_window ( const ses_string& title, const rect& bounds ) {
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
		globals::window_ctx [ title.get ( ) ].anim_time = std::array< float, 256 > { 0.0f };
		globals::window_ctx [ title.get ( ) ].cur_index = 0;

		window_entry = globals::window_ctx.find ( title.get ( ) );
	}

	if ( window_entry == globals::window_ctx.end ( ) )
		throw "Current window context not valid.";

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

	const auto window_rect = rect ( window_entry->second.bounds.x, window_entry->second.bounds.y + scale_dpi(titlebar_rect.h - 6.0f), window_entry->second.bounds.w, window_entry->second.bounds.h - titlebar_rect.h + 6.0f );
	const auto remove_rounding_rect = rect ( titlebar_rect.x, titlebar_rect.y + scale_dpi ( titlebar_rect.h - 6.0f), titlebar_rect.w, 6.0f * 2.0f );
	const auto remove_rounding_rect_filler = rect ( remove_rounding_rect.x + 1.0f, remove_rounding_rect.y + 1.0f, remove_rounding_rect.w -unscale_dpi( 2.0f), remove_rounding_rect.h + 6.0f );
	const auto exit_rect = rect(window_entry->second.bounds.x + scale_dpi(25.f) + scale_dpi(window_entry->second.bounds.w) - scale_dpi(60.f), window_entry->second.bounds.y + scale_dpi(titlebar_rect.h - 6.0f) - scale_dpi(35.f), 20.f, 20.f);

	/* window rect */
	draw_list.add_rounded_rect ( titlebar_rect, style.rounding, style.window_accent, true );
	draw_list.add_rounded_rect ( titlebar_rect, style.rounding, style.window_accent_borders, false );
	draw_list.add_rounded_rect ( window_rect, style.rounding, style.window_background, true );
	draw_list.add_rounded_rect ( window_rect, style.rounding, style.window_borders, false );

	/* covering rounding for top part of main window */
	draw_list.add_rect ( remove_rounding_rect, style.window_background, true );
	draw_list.add_rect ( remove_rounding_rect, style.window_borders, false );
	draw_list.add_rect ( remove_rounding_rect_filler, style.window_background, true );

	draw_list.add_text(vec2(window_entry->second.bounds.x + scale_dpi(15.f), window_entry->second.bounds.y - scale_dpi(30.f) + scale_dpi(titlebar_rect.h - 6.0f)), style.control_font, title, true, color(200, 200, 200, 255), true);

	/* close menu button*/
	draw_list.add_rounded_rect(exit_rect, 3.0f, color(30, 30, 30, 255), true, true);
	draw_list.add_rounded_rect(exit_rect, 3.0f, style.window_accent, false, true); /* rounding looks more smooth with outline for some reason wtf */

	if (input::mouse_in_region(exit_rect))
		draw_list.add_rounded_rect(exit_rect, 3.0f, color(static_cast<int>(style.window_accent.r), static_cast<int>(style.window_accent.g), static_cast<int>(style.window_accent.b), 50), true, true);

	if (input::mouse_in_region(exit_rect) && input::key_pressed(VK_LBUTTON))
		sesui::globals::opened = false;

	draw_list.add_text(vec2(window_entry->second.bounds.x + scale_dpi(32.f) + scale_dpi(window_entry->second.bounds.w) - scale_dpi(60.f), window_entry->second.bounds.y + scale_dpi(titlebar_rect.h - 6.0f) - scale_dpi(34.f)), style.control_font, L"x", false, style.window_accent, true);

	window_entry->second.cursor = vec2( window_entry->second.bounds.x, window_entry->second.bounds.y + scale_dpi( titlebar_rect.h) ) + vec2( scale_dpi ( style.initial_offset.x), scale_dpi ( style.initial_offset.y ));
}

void sesui::end_window ( ) {
	auto window_entry = globals::window_ctx.find ( globals::cur_window.get( ) );

	if ( window_entry == globals::window_ctx.end ( ) )
		return;

	/* draw tooltip if we have one */
	if ( !window_entry->second.selected_tooltip.empty( ) ) {
		vec2 text_size;
		draw_list.get_text_size ( style.control_font, window_entry->second.selected_tooltip.data( ), text_size );

		rect bounds = rect ( input::mouse_pos.x - scale_dpi( style.padding ), input::mouse_pos.y - scale_dpi (style.padding), unscale_dpi(text_size.x )+ style.padding * 2.0f,  unscale_dpi(text_size.y) + style.padding * 2.0f );

		bounds.y -= scale_dpi( bounds.h);

		draw_list.add_rounded_rect ( bounds, style.rounding, color ( style.window_foreground.r, style.window_foreground.g, style.window_foreground.b, static_cast< uint8_t >( 0 ) ).lerp ( style.window_foreground, window_entry->second.tooltip_anim_time ), true );
		draw_list.add_rounded_rect ( bounds, style.rounding, color ( style.window_borders.r, style.window_borders.g, style.window_borders.b, static_cast< uint8_t >( 0 ) ).lerp ( style.window_borders, window_entry->second.tooltip_anim_time ), false );
		draw_list.add_text ( vec2( bounds .x + scale_dpi(style.padding), bounds .y + scale_dpi(style.padding )), style.control_font, window_entry->second.selected_tooltip.data ( ), false, color ( style.control_text.r, style.control_text.g, style.control_text.b, static_cast< uint8_t >( 0 ) ).lerp ( style.control_text, window_entry->second.tooltip_anim_time ), true );
	}
	else {
		window_entry->second.tooltip_anim_time = 0.0f;
	}

	window_entry->second.selected_tooltip.clear ( );
	window_entry->second.cur_index = 0;
	window_entry->second.cursor = vec2( );
	globals::cur_window = "";
}