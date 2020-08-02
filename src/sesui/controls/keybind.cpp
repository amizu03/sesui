//sesui_packer:ignore
#include <windows.h>
#include "../sesui.hpp"

//sesui_packer:resume
/* credits to zxvnme */
std::array< std::basic_string < sesui::ses_char >, 256 > key_names {
	L"-", L"Mouse 1", L"Mouse 2", L"Break", L"Mouse 3", L"Mouse 4", L"Mouse 5",
	L"ERROR", L"Backspace", L"TAB", L"ERROR", L"ERROR", L"ERROR", L"Enter", L"ERROR", L"ERROR", L"Shift",
	L"Control", L"ALT", L"Pause", L"Caps Lock", L"ERROR", L"ERROR", L"ERROR", L"ERROR", L"ERROR", L"ERROR",
	L"ERROR", L"ERROR", L"ERROR", L"ERROR", L"ERROR", L"Space Bar", L"Page Up", L"Page Down", L"End", L"Home", L"Left",
	L"Up", L"Right", L"Down", L"ERROR", L"Print", L"ERROR", L"Print Screen", L"Insert", L"Delete", L"ERROR", L"0", L"1",
	L"2", L"3", L"4", L"5", L"6", L"7", L"8", L"9", L"ERROR", L"ERROR", L"ERROR", L"ERROR", L"ERROR", L"ERROR",
	L"ERROR", L"A", L"B", L"C", L"D", L"E", L"F", L"G", L"H", L"I", L"J", L"K", L"L", L"M", L"N", L"O", L"P", L"Q", L"R", L"S", L"T", L"U",
	L"V", L"W", L"X", L"Y", L"Z", L"Left Windows", L"Right Windows", L"ERROR", L"ERROR", L"ERROR", L"NUM 0", L"NUM 1",
	L"NUM 2", L"NUM 3", L"NUM 4", L"NUM 5", L"NUM 6", L"NUM 7", L"NUM 8", L"NUM 9", L"*", L"+", L"_", L"-", L".", L"/", L"F1", L"F2", L"F3",
	L"F4", L"F5", L"F6", L"F7", L"F8", L"F9", L"F10", L"F11", L"F12", L"F13", L"F14", L"F15", L"F16", L"F17", L"F18", L"F19", L"F20", L"F21",
	L"F22", L"F23", L"F24", L"ERROR", L"ERROR", L"ERROR", L"ERROR", L"ERROR", L"ERROR", L"ERROR", L"ERROR",
	L"NUM Lock", L"Scroll Lock", L"ERROR", L"ERROR", L"ERROR", L"ERROR", L"ERROR", L"ERROR", L"ERROR",
	L"ERROR", L"ERROR", L"ERROR", L"ERROR", L"ERROR", L"ERROR", L"ERROR", L"Left Shift", L"Right Shift", L"Left Control",
	L"Right Control", L"Left Menu", L"Right Menu", L"ERROR", L"ERROR", L"ERROR", L"ERROR", L"ERROR", L"ERROR", L"ERROR",
	L"ERROR", L"ERROR", L"ERROR", L"Next Track", L"Previous Track", L"Stop", L"Play / Pause", L"ERROR", L"ERROR",
	L"ERROR", L"ERROR", L"ERROR", L"ERROR", L";", L"+", L",", L"-", L".", L"/?", L"~", L"ERROR", L"ERROR",
	L"ERROR", L"ERROR", L"ERROR", L"ERROR", L"ERROR", L"ERROR", L"ERROR", L"ERROR", L"ERROR",
	L"ERROR", L"ERROR", L"ERROR", L"ERROR", L"ERROR", L"ERROR", L"ERROR", L"ERROR", L"ERROR",
	L"ERROR", L"ERROR", L"ERROR", L"ERROR", L"ERROR", L"ERROR", L"[{", L"\\|", L"}]", L"'\"", L"ERROR",
	L"ERROR", L"ERROR", L"ERROR", L"ERROR", L"ERROR", L"ERROR", L"ERROR", L"ERROR", L"ERROR",
	L"ERROR", L"ERROR", L"ERROR", L"ERROR", L"ERROR", L"ERROR", L"ERROR", L"ERROR", L"ERROR",
	L"ERROR", L"ERROR", L"ERROR", L"ERROR", L"ERROR", L"ERROR", L"ERROR", L"ERROR", L"ERROR",
	L"ERROR", L"ERROR"
};

std::array< std::basic_string < sesui::ses_char >, 4 > key_modes {
	L"Disabled", L"On Hold", L"On Toggle", L"Always On"
};

void sesui::keybind ( const ses_string& name, int& key, int& mode ) {
	const auto window = globals::window_ctx.find ( globals::cur_window );

	if ( window == globals::window_ctx.end ( ) )
		throw L"Current window context not valid.";

	const auto same_line_backup = window->second.cursor_stack.back ( );

	if ( window->second.same_line ) {
		window->second.cursor_stack.back ( ).y -= window->second.last_cursor_offset;
		window->second.cursor_stack.back ( ).x += style.same_line_offset;
	}

	const auto parts = split ( name.get ( ) );
	auto title = ses_string ( parts.first.data ( ) );
	const auto& id = parts.second;

	vec2 title_size;
	draw_list.get_text_size ( style.control_font, title, title_size );

	rect check_rect { window->second.cursor_stack.back ( ).x, window->second.cursor_stack.back ( ).y + title_size.y + scale_dpi ( style.padding ), style.button_size.x, style.button_size.y };
	
	if ( window->second.same_line )
		check_rect = { window->second.cursor_stack.back ( ).x, window->second.cursor_stack.back ( ).y, style.inline_button_size.x, style.inline_button_size.y };

	const auto frametime = draw_list.get_frametime ( );

	/* don't draw objects we don't need so our fps doesnt go to shit */
	auto should_draw = true;

	if ( !window->second.group_ctx.empty ( )
		&& !input::in_clip ( vec2 ( window->second.cursor_stack.back ( ).x + 1.0f, window->second.cursor_stack.back ( ).y ) )
		&& !input::in_clip ( vec2 ( window->second.cursor_stack.back ( ).x + 1.0f, window->second.cursor_stack.back ( ).y + scale_dpi ( 60.0f ) ) ) )
		should_draw = false;

	auto key_name = ses_string ( key_names [ key ].data ( ) );

	vec2 key_name_size;
	draw_list.get_text_size ( style.control_font, key_name, key_name_size );

	const auto max_height = std::max< float > ( scale_dpi ( check_rect.h ), key_name_size.y );
	const auto in_region = input::mouse_in_region ( check_rect );

	if ( in_region && !window->second.tooltip.empty ( ) )
		window->second.hover_time [ window->second.cur_index ] += frametime;
	else
		window->second.hover_time [ window->second.cur_index ] = 0.0f;

	if ( window->second.hover_time [ window->second.cur_index ] > style.tooltip_hover_time ) {
		window->second.tooltip_anim_time += frametime * style.animation_speed;
		window->second.selected_tooltip = window->second.tooltip;
	}

	const auto active = in_region && input::key_down ( VK_LBUTTON );

	if ( in_region && input::key_pressed ( VK_LBUTTON ) ) {
		window->second.anim_time [ window->second.cur_index ] = 1.0f;

		/* searching for input */
		window->second.anim_time [ window->second.cur_index + 2 ] = 1.0f;
	}

	if ( ( input::key_pressed ( VK_RBUTTON ) || ( window->second.anim_time [ window->second.cur_index + 4 ] > 0.0f && input::key_state [ VK_LBUTTON ] && !input::old_key_state [ VK_LBUTTON ] ) ) ) {
		const auto calculated_height = style.button_size.y * key_modes.size ( );
		const auto list_rect = rect ( check_rect.x, check_rect.y + scale_dpi ( style.button_size.y + style.padding + style.padding ), style.button_size.x, calculated_height );

		if ( in_region ) {
			if ( !window->second.anim_time [ window->second.cur_index + 3 ] )
				window->second.anim_time [ window->second.cur_index + 3 ] = -1.0f;

			window->second.anim_time [ window->second.cur_index + 3 ] = -window->second.anim_time [ window->second.cur_index + 3 ];

			input::enable_input ( false );
		}
		else if ( !input::mouse_in_region ( list_rect, true ) && window->second.anim_time [ window->second.cur_index + 3 ] > 0.0f ) {
			window->second.anim_time [ window->second.cur_index + 3 ] = -1.0f;
			input::enable_input ( true );
		}
	}

	window->second.anim_time [ window->second.cur_index ] = std::clamp< float > ( window->second.anim_time [ window->second.cur_index ] + -frametime * style.animation_speed, 0.0f, 1.0f );
	window->second.anim_time [ window->second.cur_index + 1 ] = std::clamp< float > ( window->second.anim_time [ window->second.cur_index + 1 ] + ( in_region ? frametime : -frametime ) * style.animation_speed, 0.0f, 1.0f );
	window->second.anim_time [ window->second.cur_index + 4 ] = std::clamp< float > ( window->second.anim_time [ window->second.cur_index + 4 ] + ( window->second.anim_time [ window->second.cur_index + 3 ] > 0.0f ? frametime : -frametime )* style.animation_speed, 0.0f, 1.0f );

	/* searching for input */
	if ( window->second.anim_time [ window->second.cur_index + 2 ] ) {
		input::enable_input ( false );

		if ( !in_region && input::key_state [ VK_LBUTTON ] && !input::old_key_state [ VK_LBUTTON ] ) {
			key = 0;
			window->second.anim_time [ window->second.cur_index + 2 ] = 0.0f;
			window->second.anim_time [ window->second.cur_index ] = 1.0f;
			input::enable_input ( true );
		}
		else {
			/* if we get some input, then bind the key and stop searching for input */
			for ( auto i = 0; i < 256; i++ ) {
				if ( i == VK_ESCAPE && input::key_state [ i ] && !input::old_key_state [ i ] ) {
					key = 0;
					window->second.anim_time [ window->second.cur_index + 2 ] = 0.0f;
					window->second.anim_time [ window->second.cur_index ] = 1.0f;
					input::enable_input ( true );
					break;
				}

				if ( i == VK_LBUTTON || i == VK_RBUTTON )
					continue;

				if ( input::key_state [ i ] && !input::old_key_state [ i ] ) {
					if ( key_names [ i ] != L"ERROR" )
						key = i;

					window->second.anim_time [ window->second.cur_index + 2 ] = 0.0f;
					window->second.anim_time [ window->second.cur_index ] = 1.0f;
					input::enable_input ( true );
					break;
				}
			}
		}

		key_name = L"...";
		draw_list.get_text_size ( style.control_font, key_name, key_name_size );
	}
	else {
		key_name = ses_string( key_names [ key ].data ( ) );
		draw_list.get_text_size ( style.control_font, key_name, key_name_size );
	}

	if ( should_draw ) {
		if ( !window->second.same_line )
			draw_list.add_text ( window->second.cursor_stack.back ( ), style.control_font, title, false, style.control_text.lerp ( style.control_text_hovered, window->second.anim_time [ window->second.cur_index + 1 ] ) );

		draw_list.add_rounded_rect ( check_rect, style.control_rounding, style.control_background.lerp ( style.control_accent, window->second.anim_time [ window->second.cur_index ] ), true );
		draw_list.add_rounded_rect ( check_rect, style.control_rounding, style.control_borders.lerp ( active ? style.control_accent_borders : style.control_accent, window->second.anim_time [ window->second.cur_index + 1 ] ), false );
		draw_list.add_text ( vec2 ( check_rect.x + scale_dpi ( check_rect.w ) * 0.5f - key_name_size.x * 0.5f, check_rect.y + scale_dpi ( check_rect.h ) * 0.5f - key_name_size.y * 0.5f ), style.control_font, key_name, false, style.control_text.lerp ( style.control_text_hovered, window->second.anim_time [ window->second.cur_index + 1 ] ) );

		const auto backup_clip_enabled = globals::clip_enabled;
		globals::clip_enabled = false;

		if ( window->second.anim_time [ window->second.cur_index + 4 ] > 0.0f ) {
			const auto calculated_height = style.button_size.y * key_modes.size ( );
			const auto list_rect = rect ( check_rect.x, check_rect.y + scale_dpi ( style.button_size.y + style.padding + style.padding ), style.button_size.x, calculated_height * window->second.anim_time [ window->second.cur_index + 4 ] );

			draw_list.add_rounded_rect ( list_rect, style.control_rounding, color ( style.control_background.r, style.control_background.g, style.control_background.b, 0.0f ).lerp ( style.control_background, window->second.anim_time [ window->second.cur_index + 4 ] ), true, true );
			draw_list.add_rounded_rect ( list_rect, style.control_rounding, color ( style.control_borders.r, style.control_borders.g, style.control_borders.b, 0.0f ).lerp ( style.control_borders, window->second.anim_time [ window->second.cur_index + 4 ] ), false, true );

			for ( auto i = 0; i < key_modes.size ( ); i++ ) {
				vec2 text_size;
				draw_list.get_text_size ( style.control_font, key_modes [ i ].data(), text_size );

				if ( input::mouse_in_region ( rect ( window->second.cursor_stack.back ( ).x, list_rect.y + scale_dpi ( style.button_size.y ) * i, style.button_size.x, style.button_size.y ), true ) ) {
					if ( std::fabsf ( window->second.anim_time [ window->second.cur_index + 5 ] - static_cast< float > ( i ) ) > 0.1f )
						window->second.anim_time [ window->second.cur_index + 5 ] += ( static_cast< float > ( i ) - window->second.anim_time [ window->second.cur_index + 5 ] > 0.0f ) ? frametime * style.animation_speed * 3.0f : ( -frametime * style.animation_speed * 3.0f );
					else
						window->second.anim_time [ window->second.cur_index + 5 ] = static_cast< float > ( i );

					if ( input::key_state [ VK_LBUTTON ] && !input::old_key_state [ VK_LBUTTON ] ) {
						window->second.anim_time [ window->second.cur_index + 3 ] = -1.0f;
						input::enable_input ( true );

						switch ( i ) {
						case 0: {
							key = 0;
							window->second.anim_time [ window->second.cur_index + 2 ] = 0.0f;
							window->second.anim_time [ window->second.cur_index ] = 1.0f;
						} break;
						case 1: {
							mode = 1;
						} break;
						case 2: {
							mode = 2;
						} break;
						case 3: {
							mode = 3;
						} break;
						}
					}
				}

				const auto lerped_color = style.control_accent.lerp ( style.control_text, std::clamp< float > ( std::fabsf ( static_cast< float > ( i ) - window->second.anim_time [ window->second.cur_index + 5 ] ), 0.0f, 1.0f ) );

				if ( list_rect.y + scale_dpi ( style.button_size.y * 0.5f + i * style.button_size.y ) + text_size.y * 0.5f > list_rect.y + scale_dpi ( list_rect.h ) )
					break;

				draw_list.add_text ( vec2 ( window->second.cursor_stack.back ( ).x + scale_dpi ( style.padding ), list_rect.y + scale_dpi ( style.button_size.y * 0.5f + i * style.button_size.y ) - text_size.y * 0.5f ), style.control_font, key_modes [ i ].data(), true, color ( lerped_color.r, lerped_color.g, lerped_color.b, 0.0f ).lerp ( lerped_color, window->second.anim_time [ window->second.cur_index + 4 ] ), true );
			}
		}

		globals::clip_enabled = backup_clip_enabled;
	}

	window->second.last_cursor_offset = scale_dpi ( style.button_size.y + style.spacing + style.padding ) + title_size.y;
	window->second.cursor_stack.back ( ).y += window->second.last_cursor_offset;
	window->second.cur_index += 6;
	window->second.tooltip.clear ( );

	if ( window->second.same_line ) {
		window->second.cursor_stack.back ( ) = same_line_backup;
		window->second.same_line = false;
	}
}