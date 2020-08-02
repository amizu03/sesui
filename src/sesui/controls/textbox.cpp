//sesui_packer:ignore
#include <windows.h>
#include "../sesui.hpp"

//sesui_packer:resume
std::map< int, std::pair<wchar_t, wchar_t> > special_characters {
	{48,  {L'0',  L')'}},
	{49,  {L'1',  L'!'}},
	{50,  {L'2',  L'@'}},
	{51,  {L'3',  L'#'}},
	{52,  {L'4',  L'$'}},
	{53,  {L'5',  L'%'}},
	{54,  {L'6',  L'^'}},
	{55,  {L'7',  L'&'}},
	{56,  {L'8',  L'*'}},
	{57,  {L'9',  L'('}},
	{32,  {L' ',  L' '}},
	{192, {L'`',  L'~'}},
	{189, {L'-',  L'_'}},
	{187, {L'=',  L'+'}},
	{219, {L'[',  L'{'}},
	{220, {L'\\', L'|'}},
	{221, {L']',  L'}'}},
	{186, {L';',  L':'}},
	{222, {L'\'', L'"'}},
	{188, {L',',  L'<'}},
	{190, {L'.',  L'>'}},
	{191, {L'/',  L'?'}}
};

void sesui::textbox ( const ses_string& name, std::basic_string< ses_char >& option ) {
	const auto window = globals::window_ctx.find ( globals::cur_window );

	if ( window == globals::window_ctx.end ( ) )
		throw "Current window context not valid.";

	const auto same_line_backup = window->second.cursor_stack.back ( );

	if ( window->second.same_line ) {
		window->second.cursor_stack.back ( ).y -= window->second.last_cursor_offset;
		window->second.cursor_stack.back ( ).x += style.same_line_offset;
	}

	const auto parts = split ( name.get ( ) );
	const auto title = ses_string ( parts.first.data ( ) );
	const auto& id = parts.second;

	/* don't draw objects we don't need so our fps doesnt go to shit */
	auto should_draw = true;

	if ( !window->second.group_ctx.empty ( )
		&& !input::in_clip ( vec2 ( window->second.cursor_stack.back ( ).x + 1.0f, window->second.cursor_stack.back ( ).y ) )
		&& !input::in_clip ( vec2 ( window->second.cursor_stack.back ( ).x + 1.0f, window->second.cursor_stack.back ( ).y + scale_dpi ( 60.0f ) ) ) )
		should_draw = false;

	vec2 text_size;
	draw_list.get_text_size ( style.control_font, title, text_size );

	if ( should_draw ) {
		/* label */
		draw_list.add_text ( window->second.cursor_stack.back ( ), style.control_font, title, false, style.control_text.lerp ( style.control_text_hovered, window->second.anim_time [ window->second.cur_index + 1 ] ) );
	}

	vec2 option_size;
	draw_list.get_text_size ( style.control_font, option.data(), option_size );

	vec2 char_size;
	draw_list.get_text_size ( style.control_font, L"F", char_size );

	const auto check_rect = rect ( window->second.cursor_stack.back ( ).x, window->second.cursor_stack.back ( ).y + text_size.y + scale_dpi ( style.padding ), style.button_size.x, style.button_size.y );
	const auto frametime = draw_list.get_frametime ( );

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

	if ( input::key_pressed ( VK_LBUTTON ) || ( window->second.anim_time [ window->second.cur_index + 2 ] > 0.0f && input::key_state [ VK_LBUTTON ] && !input::old_key_state [ VK_LBUTTON ] ) ) {
		if ( in_region ) {
			if ( !window->second.anim_time [ window->second.cur_index + 2 ] )
				window->second.anim_time [ window->second.cur_index + 2 ] = -1.0f;

			window->second.anim_time [ window->second.cur_index + 2 ] = -window->second.anim_time [ window->second.cur_index + 2 ];

			input::enable_input ( false );
		}
		else if ( !input::mouse_in_region ( check_rect, true ) && window->second.anim_time [ window->second.cur_index + 2 ] > 0.0f ) {
			window->second.anim_time [ window->second.cur_index + 2 ] = -1.0f;
			input::enable_input ( true );
		}
	}

	window->second.anim_time [ window->second.cur_index ] = std::clamp< float > ( window->second.anim_time [ window->second.cur_index ] + -frametime * style.animation_speed, 0.0f, 1.0f );
	window->second.anim_time [ window->second.cur_index + 1 ] = std::clamp< float > ( window->second.anim_time [ window->second.cur_index + 1 ] + ( in_region ? frametime : -frametime ) * style.animation_speed, 0.0f, 1.0f );
	window->second.anim_time [ window->second.cur_index + 3 ] = std::clamp< float > ( window->second.anim_time [ window->second.cur_index + 3 ] + ( window->second.anim_time [ window->second.cur_index + 2 ] > 0.0f ? frametime : -frametime )* style.animation_speed, 0.0f, 1.0f );
	
	if ( window->second.anim_time [ window->second.cur_index + 6 ] + frametime * style.animation_speed * 0.33f > 1.0f )
		window->second.anim_time [ window->second.cur_index + 5 ] = -1.0f;
	else if ( window->second.anim_time [ window->second.cur_index + 6 ] - frametime * style.animation_speed * 0.33f < 0.0f )
		window->second.anim_time [ window->second.cur_index + 5 ] = 1.0f;

	window->second.anim_time [ window->second.cur_index + 6 ] = std::clamp< float > ( window->second.anim_time [ window->second.cur_index + 6 ] + ( window->second.anim_time [ window->second.cur_index + 5 ] > 0.0f ? frametime : -frametime )* style.animation_speed * 0.33f, 0.0f, 1.0f );

	if ( should_draw ) {
		draw_list.add_rounded_rect ( check_rect, style.control_rounding, style.control_background.lerp ( style.control_accent, window->second.anim_time [ window->second.cur_index ] ), true );
		draw_list.add_rounded_rect ( check_rect, style.control_rounding, style.control_borders.lerp ( active ? style.control_accent_borders : style.control_accent, std::max< float>(window->second.anim_time [ window->second.cur_index + 1 ], window->second.anim_time [ window->second.cur_index + 3 ] ) ), false );
		draw_list.add_text ( vec2 ( check_rect.x + scale_dpi ( style.padding ), check_rect.y + scale_dpi ( style.button_size.y ) * 0.5f - char_size.y * 0.5f ), style.control_font, option.data(), false, style.control_text.lerp ( style.control_text_hovered, window->second.anim_time [ window->second.cur_index + 1 ] ) );

		if ( window->second.anim_time [ window->second.cur_index + 2 ] > 0.0f )
			draw_list.add_rect ( rect ( check_rect.x + scale_dpi ( style.padding ) + option_size.x + 2.0f, check_rect.y + scale_dpi ( style.button_size.y ) * 0.5f - option_size.y * 0.5f, unscale_dpi ( 1.0f ), option_size.y ), color ( ).lerp ( color ( 1.0f, 1.0f, 1.0f, 1.0f ), window->second.anim_time [ window->second.cur_index + 6 ] ), true );
	}

	if ( window->second.anim_time [ window->second.cur_index + 2 ] > 0.0f ) {
		sesui::input::enabled = true;

		if ( input::key_pressed ( VK_BACK ) || input::key_pressed ( VK_DELETE ) ) {
			if ( !option.empty ( ) )
				option.pop_back ( );
		}
		else if ( input::key_pressed ( VK_ESCAPE ) || input::key_pressed ( VK_RETURN ) ) {
			window->second.anim_time [ window->second.cur_index + 2 ] = -1.0f;
			input::enable_input ( true );
		}
		else {
			for ( auto i = 32; i < 222; i++ ) {
				if ( ( i > 32 && i < 48 ) || ( i > 57 && i < 65 ) || ( i > 90 && i < 186 ) )
					continue;

				if ( !input::key_pressed ( i ) )
					continue;

				if ( i > 57 && i <= 90 ) {
					option.push_back ( i + ( input::key_down ( VK_SHIFT ) ? 0 : 0x20 ) );
				}
				else {
					if ( special_characters.find ( i ) != special_characters.end ( ) )
						option.push_back ( input::key_down ( VK_SHIFT ) ? special_characters [ i ].second : special_characters [ i ].first );
				}
			}
		}

		sesui::input::enabled = false;
	}

	window->second.last_cursor_offset = text_size.y + scale_dpi ( style.button_size.y + style.spacing + style.padding );
	window->second.cursor_stack.back ( ).y += window->second.last_cursor_offset;
	window->second.cur_index += 7;
	window->second.tooltip.clear ( );

	if ( window->second.same_line ) {
		window->second.cursor_stack.back ( ) = same_line_backup;
		window->second.same_line = false;
	}
}