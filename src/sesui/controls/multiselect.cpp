//sesui_packer:ignore
#include <windows.h>
#include "../sesui.hpp"

//sesui_packer:resume
void sesui::multiselect ( const ses_string& name, const std::vector< std::pair< ses_string, bool& > >& list ) {
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

	vec2 text_size;
	vec2 text_size1;
	draw_list.get_text_size ( style.control_font, title, text_size );

	/* don't draw objects we don't need so our fps doesnt go to shit */
	auto should_draw = true;

	if ( !window->second.group_ctx.empty ( )
		&& !input::in_clip ( vec2 ( window->second.cursor_stack.back ( ).x + 1.0f, window->second.cursor_stack.back ( ).y ) )
		&& !input::in_clip ( vec2 ( window->second.cursor_stack.back ( ).x + 1.0f, window->second.cursor_stack.back ( ).y + scale_dpi ( 60.0f ) ) ) )
		should_draw = false;

	if ( should_draw ) {
		/* label */
		draw_list.add_text ( window->second.cursor_stack.back ( ), style.control_font, title, false, style.control_text.lerp ( style.control_text_hovered, window->second.anim_time [ window->second.cur_index + 1 ] ) );
	}

	std::basic_string < ses_char > str_items;

	vec2 stopper_text_size;
	draw_list.get_text_size ( style.control_font, L", ...", stopper_text_size );

	/* enumerate selected multibox options and get up to max text size in items before clipping and adding ellipses */
	for ( const auto& option : list ) {
		std::basic_string < ses_char > copy = str_items;

		if ( option.second ) {
			if ( str_items.empty() )
				copy += option.first.get ( );
			else
				copy += L", " + std::basic_string < ses_char > ( option.first.get ( ) );

			draw_list.get_text_size ( style.control_font, copy.data ( ), text_size1 );

			if ( text_size1.x + stopper_text_size.x > scale_dpi ( style.button_size.x - style.padding * 2.0f ) ) {
				str_items += L", ...";
				break;
			}

			str_items = copy;
		}
	}

	draw_list.get_text_size ( style.control_font, str_items.data ( ), text_size1 );

	const auto check_rect = rect( window->second.cursor_stack.back ( ).x, window->second.cursor_stack.back ( ).y + text_size.y + scale_dpi ( style.padding ), style.button_size.x, style.button_size.y );
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
		const auto calculated_height = style.button_size.y * list.size ( );
		const auto list_rect = rect ( window->second.cursor_stack.back ( ).x, window->second.cursor_stack.back ( ).y + text_size.y + scale_dpi ( style.button_size.y + style.padding + style.padding ), style.button_size.x, calculated_height );

		const auto backup_clip_enabled = globals::clip_enabled;
		globals::clip_enabled = false;

		if ( in_region ) {
			if ( !window->second.anim_time [ window->second.cur_index + 2 ] )
				window->second.anim_time [ window->second.cur_index + 2 ] = -1.0f;

			window->second.anim_time [ window->second.cur_index + 2 ] = -window->second.anim_time [ window->second.cur_index + 2 ];

			input::enable_input ( false );
		}
		else if ( !input::mouse_in_region ( list_rect, true ) && window->second.anim_time [ window->second.cur_index + 2 ] > 0.0f ) {
			window->second.anim_time [ window->second.cur_index + 2 ] = -1.0f;
			input::enable_input ( true );
		}

		globals::clip_enabled = backup_clip_enabled;
	}

	window->second.anim_time [ window->second.cur_index ] = std::clamp< float > ( window->second.anim_time [ window->second.cur_index ] + -frametime * style.animation_speed, 0.0f, 1.0f );
	window->second.anim_time [ window->second.cur_index + 1 ] = std::clamp< float > ( window->second.anim_time [ window->second.cur_index + 1 ] + ( in_region ? frametime : -frametime ) * style.animation_speed, 0.0f, 1.0f );
	window->second.anim_time [ window->second.cur_index + 3 ] = std::clamp< float > ( window->second.anim_time [ window->second.cur_index + 3 ] + ( window->second.anim_time [ window->second.cur_index + 2 ] > 0.0f ? frametime : -frametime ) * style.animation_speed, 0.0f, 1.0f );

	if ( should_draw ) {
		draw_list.add_rounded_rect ( check_rect, style.control_rounding, style.control_background.lerp ( style.control_accent, window->second.anim_time [ window->second.cur_index ] ), true );
		draw_list.add_rounded_rect ( check_rect, style.control_rounding, style.control_borders.lerp ( active ? style.control_accent_borders : style.control_accent, window->second.anim_time [ window->second.cur_index + 1 ] ), false );
		draw_list.add_text ( vec2 ( check_rect.x + scale_dpi ( style.button_size.x ) * 0.5f - text_size1.x * 0.5f, check_rect.y + scale_dpi ( style.button_size.y ) * 0.5f - text_size1.y * 0.5f ), style.control_font, str_items.data ( ), false, style.control_text.lerp ( style.control_text_hovered, window->second.anim_time [ window->second.cur_index + 1 ] ) );
		draw_list.add_arrow ( vec2 ( check_rect.x + scale_dpi ( style.button_size.x - style.padding - ( ( 1.0f - window->second.anim_time [ window->second.cur_index + 3 ] ) * 4.0f ) ), check_rect.y + scale_dpi ( style.button_size.y ) * 0.5f - ( ( 1.0f - window->second.anim_time [ window->second.cur_index + 3 ] ) * 4.0f ) * 0.5f ), 4.0f, -90.0f + window->second.anim_time [ window->second.cur_index + 3 ] * 90.0f, style.control_text, false );
	}
	
	const auto backup_clip_enabled = globals::clip_enabled;
	globals::clip_enabled = false;

	if ( should_draw && window->second.anim_time [ window->second.cur_index + 3 ] > 0.0f ) {
		const auto calculated_height = style.button_size.y * list.size ( );
		const auto list_rect = rect ( window->second.cursor_stack.back ( ).x, window->second.cursor_stack.back ( ).y + text_size.y + scale_dpi ( style.button_size.y + style.padding + style.padding ), style.button_size.x, calculated_height * window->second.anim_time [ window->second.cur_index + 3 ] );

		draw_list.add_rounded_rect ( list_rect, style.control_rounding, color ( style.control_background.r, style.control_background.g, style.control_background.b, 0.0f ).lerp( style.control_background, window->second.anim_time [ window->second.cur_index + 3 ] ), true, true );
		draw_list.add_rounded_rect ( list_rect, style.control_rounding, color ( style.control_borders.r, style.control_borders.g, style.control_borders.b, 0.0f ).lerp ( style.control_borders, window->second.anim_time [ window->second.cur_index + 3 ] ), false, true );

		for ( auto i = 0; i < list.size ( ); i++ ) {
			vec2 text_size;
			draw_list.get_text_size ( style.control_font, list[i].first, text_size );

			if ( input::mouse_in_region ( rect ( window->second.cursor_stack.back ( ).x, list_rect.y +scale_dpi( style.button_size.y) * i, style.button_size.x, style.button_size.y ), true ) ) {
				if ( std::fabsf ( window->second.anim_time [ window->second.cur_index + 4 ] - static_cast< float > ( i ) ) > 0.1f )
					window->second.anim_time [ window->second.cur_index + 4 ] += ( static_cast< float > ( i ) - window->second.anim_time [ window->second.cur_index + 4 ] > 0.0f ) ? frametime * style.animation_speed * 3.0f : ( -frametime * style.animation_speed * 3.0f );
				else
					window->second.anim_time [ window->second.cur_index + 4 ] = static_cast< float > ( i );

				if ( input::key_state [ VK_LBUTTON ] && !input::old_key_state [ VK_LBUTTON ] )
					list [ i ].second = !list [ i ].second;
			}

			const auto lerped_color = style.control_accent.lerp ( style.control_text, std::clamp< float > ( std::fabsf ( static_cast< float > ( i ) - window->second.anim_time [ window->second.cur_index + 4 ] ), 0.0f, 1.0f ) );

			if ( list_rect.y + scale_dpi( style.button_size.y * 0.5f + i * style.button_size.y ) + text_size.y * 0.5f > list_rect.y + scale_dpi( list_rect.h ) )
				break;

			if ( list[i].second )
				draw_list.add_text ( vec2 ( window->second.cursor_stack.back ( ).x + scale_dpi ( style.padding ), list_rect.y + scale_dpi ( style.button_size.y * 0.5f + i * style.button_size.y ) - text_size.y * 0.5f ), style.control_font, list [ i ].first, true, color ( style.control_accent.r, style.control_accent.g, style.control_accent.b, 0.0f ).lerp ( style.control_accent, window->second.anim_time [ window->second.cur_index + 3 ] ), true );
			else
				draw_list.add_text ( vec2 ( window->second.cursor_stack.back ( ).x + scale_dpi( style.padding ), list_rect.y + scale_dpi( style.button_size.y * 0.5f + i * style.button_size.y ) - text_size.y * 0.5f ), style.control_font, list [ i ].first, true, color ( lerped_color.r, lerped_color.g, lerped_color.b, 0.0f ).lerp ( lerped_color, window->second.anim_time [ window->second.cur_index + 3 ] ), true );
		}
	}

	globals::clip_enabled = backup_clip_enabled;

	window->second.last_cursor_offset = text_size.y + scale_dpi ( style.button_size.y + style.spacing + style.padding );
	window->second.cursor_stack.back ( ).y += window->second.last_cursor_offset;
	window->second.cur_index += 5;
	window->second.tooltip.clear ( );

	if ( window->second.same_line ) {
		window->second.cursor_stack.back ( ) = same_line_backup;
		window->second.same_line = false;
	}
}