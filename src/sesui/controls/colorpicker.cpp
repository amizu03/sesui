//sesui_packer:ignore
#include <windows.h>
#include "../sesui.hpp"

//sesui_packer:resume
sesui::color color_clipboard = sesui::color( 1.0f, 1.0f, 1.0f, 1.0f );

std::array< sesui::color, 7 > hue_colors = {
	sesui::color ( 1.0f, 0, 0, 1.0f ),
	sesui::color ( 1.0f, 1.0f, 0, 1.0f ),
	sesui::color ( 0, 1.0f, 0 , 1.0f ),
	sesui::color ( 0, 1.0f, 1.0f , 1.0f ),
	sesui::color ( 0, 0, 1.0f, 1.0f ),
	sesui::color ( 1.0f, 0, 1.0f , 1.0f ),
	sesui::color ( 1.0f, 0, 0, 1.0f )
};

std::array< std::basic_string < sesui::ses_char >, 2 > color_options {
	L"Copy", L"Paste"
};

void sesui::colorpicker ( const ses_string& name, color& option ) {
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

	rect check_rect { window->second.cursor_stack.back ( ).x + scale_dpi ( style.button_size.x - style.inline_button_size.x ), window->second.cursor_stack.back ( ).y, style.inline_button_size.x, style.inline_button_size.y };

	if ( window->second.same_line )
		check_rect = { window->second.cursor_stack.back ( ).x, window->second.cursor_stack.back ( ).y, style.inline_button_size.x, style.inline_button_size.y };

	const auto frametime = draw_list.get_frametime ( );

	/* don't draw objects we don't need so our fps doesnt go to shit */
	auto should_draw = true;

	if ( !window->second.group_ctx.empty ( )
		&& !input::in_clip ( vec2 ( window->second.cursor_stack.back ( ).x + 1.0f, window->second.cursor_stack.back ( ).y ) )
		&& !input::in_clip ( vec2 ( window->second.cursor_stack.back ( ).x + 1.0f, window->second.cursor_stack.back ( ).y + scale_dpi ( 60.0f ) ) ) )
		should_draw = false;

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
		const auto calculated_height = style.button_size.y * color_options.size ( );
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
	window->second.anim_time [ window->second.cur_index + 6 ] = std::clamp< float > ( window->second.anim_time [ window->second.cur_index + 6 ] + ( window->second.anim_time [ window->second.cur_index + 2 ] ? frametime : -frametime ) * style.animation_speed, 0.0f, 1.0f );

	rect color_picker_region { check_rect.x, check_rect.y - scale_dpi ( style.color_popup.y + style.padding ), style.color_popup.x, style.color_popup.y };

	/* searching for input */
	if ( window->second.anim_time [ window->second.cur_index + 2 ] ) {
		const auto original_input = input::enabled;

		input::enable_input ( false );

		const auto backup_clip_enabled = globals::clip_enabled;
		globals::clip_enabled = false;

		const auto in_color_picker = input::mouse_in_region( color_picker_region, true );

		globals::clip_enabled = backup_clip_enabled;

		if ( !original_input && !in_color_picker && input::key_state [ VK_LBUTTON ] && !input::old_key_state [ VK_LBUTTON ] ) {
			window->second.anim_time [ window->second.cur_index + 2 ] = 0.0f;
			window->second.anim_time [ window->second.cur_index ] = 1.0f;
			input::enable_input ( true );
		}
	}

	if ( should_draw ) {
		if ( !window->second.same_line )
			draw_list.add_text ( window->second.cursor_stack.back ( ), style.control_font, title, false, style.control_text.lerp ( style.control_text_hovered, window->second.anim_time [ window->second.cur_index + 1 ] ) );

		const auto square_side_len = check_rect.h * 0.5f;
		auto alpha_clr_flip = false;
		const auto remaining_len = std::fmodf ( check_rect.w, square_side_len );

		for ( auto i = 0.0f; i < check_rect.w; i += square_side_len ) {
			const auto calc_len = i > check_rect.w - square_side_len ? remaining_len : square_side_len;

			draw_list.add_rect ( rect ( check_rect.x + scale_dpi ( i ), check_rect.y, calc_len, calc_len ), alpha_clr_flip ? color ( 1.0f, 1.0f, 1.0f, 1.0f ) : color ( 0.82f, 0.82f, 0.82f, 1.0f ), true );
			draw_list.add_rect ( rect ( check_rect.x + scale_dpi ( i ), check_rect.y + scale_dpi ( calc_len ), calc_len, calc_len ), alpha_clr_flip ? color ( 0.82f, 0.82f, 0.82f, 1.0f ) : color ( 1.0f, 1.0f, 1.0f, 1.0f ), true );

			alpha_clr_flip = !alpha_clr_flip;
		}

		draw_list.add_rect ( check_rect, option, true );
		draw_list.add_rect ( check_rect, style.control_borders.lerp ( active ? style.control_accent_borders : style.control_accent, window->second.anim_time [ window->second.cur_index + 1 ] ), false );
		
		if ( window->second.anim_time [ window->second.cur_index + 6 ] ) {
			auto hue_bar_region = rect ( color_picker_region.x + scale_dpi ( color_picker_region.h - style.padding - style.color_bar_width ), color_picker_region.y + scale_dpi ( style.padding ), style.color_bar_width, color_picker_region.h - style.padding * 2.0f - style.color_bar_width - style.padding );
			auto alpha_bar_region = rect ( color_picker_region.x + scale_dpi ( style.padding ), color_picker_region.y + scale_dpi ( color_picker_region.h - style.color_bar_width - style.padding ), color_picker_region.w - style.padding * 2.0f - style.color_bar_width - style.padding, style.color_bar_width );
			auto color_square_region = rect ( color_picker_region.x + scale_dpi ( style.padding ), color_picker_region.y + scale_dpi ( style.padding ), color_picker_region.w - style.color_bar_width - style.padding * 3.0f, color_picker_region.h - style.color_bar_width - style.padding * 3.0f );
			auto preview_color = rect ( color_picker_region.x + scale_dpi( color_picker_region.w - style.color_bar_width - style.padding ), color_picker_region.y + scale_dpi ( color_picker_region.h - style.color_bar_width - style.padding ), style.color_bar_width, style.color_bar_width );

			const auto backup_clip_enabled = globals::clip_enabled;
			globals::clip_enabled = false;

			/* clicking logic */
			/* color square */
			if ( input::click_in_region ( color_square_region, true ) && input::key_state [ VK_LBUTTON ] ) {
				auto mouse_delta = input::mouse_pos - vec2 ( color_square_region.x, color_square_region.y );

				mouse_delta.x = std::clamp < float > ( mouse_delta.x, 1.0f, scale_dpi(color_square_region.w) - 1.0f);
				mouse_delta.y = std::clamp < float > ( mouse_delta.y, 0.0f, scale_dpi(color_square_region.h) - 2.0f );

				color as_hsv = option.to_hsv();

				as_hsv.b = 1.0f - ( mouse_delta.y / ( scale_dpi ( color_square_region.h ) - 1.0f ) );
				as_hsv.g = mouse_delta.x / ( scale_dpi ( color_square_region.w ) - 1.0f );

				option = as_hsv.to_rgb ( );
			}

			/* hue bar */
			if ( input::click_in_region ( hue_bar_region, true ) && input::key_state [ VK_LBUTTON ] ) {
				const auto mouse_delta = std::clamp < float > ( input::mouse_pos.y - hue_bar_region.y, 0.0f, scale_dpi ( hue_bar_region.h ) - 1.0f );

				color as_hsv = option.to_hsv ( );
				as_hsv.r = mouse_delta / scale_dpi ( hue_bar_region.h );
				option = as_hsv.to_rgb ( );
			}

			/* alpha bar */
			if ( input::click_in_region ( alpha_bar_region, true ) && input::key_state [ VK_LBUTTON ] ) {
				const auto mouse_delta = std::clamp < float > ( input::mouse_pos.x - alpha_bar_region.x, 0.0f, scale_dpi ( alpha_bar_region.w ) );

				color as_hsv = option.to_hsv ( );
				as_hsv.a = mouse_delta / scale_dpi ( alpha_bar_region.w );
				option = as_hsv.to_rgb ( );
			}

			globals::clip_enabled = backup_clip_enabled;

			/* window rect */
			draw_list.add_rounded_rect ( color_picker_region, style.rounding, color ( style.control_background.r, style.control_background.g, style.control_background.b, 0.0f ).lerp ( style.control_background, window->second.anim_time [ window->second.cur_index + 6 ] ), true, true );
			draw_list.add_rounded_rect ( color_picker_region, style.rounding, color ( style.control_borders.r, style.control_borders.g, style.control_borders.b, 0.0f ).lerp ( style.control_borders, window->second.anim_time [ window->second.cur_index + 6 ] ), false, true );

			for ( auto i = 0; i < 6; i++ ) {
				draw_list.add_rect_gradient (
					rect ( hue_bar_region.x, hue_bar_region.y + scale_dpi ( hue_bar_region.h ) / 6.0f * static_cast< float > ( i ), style.color_bar_width, hue_bar_region.h / 6.0f ),
					color ( ).lerp ( hue_colors [ i ], window->second.anim_time [ window->second.cur_index + 6 ] ),
					color ( ).lerp ( hue_colors [ i + 1 ], window->second.anim_time [ window->second.cur_index + 6 ] ),
					false,
					true,
					true
				);
			}

			/* alpha bar background */
			const auto square_side_len = alpha_bar_region.h * 0.5f;
			auto alpha_clr_flip = false;
			const auto remaining_len = std::fmodf ( alpha_bar_region.w, square_side_len );

			for ( auto i = 0.0f; i < alpha_bar_region.w; i += square_side_len ) {
				const auto calc_len = i > alpha_bar_region.w - square_side_len ? remaining_len : square_side_len;

				draw_list.add_rect ( rect( alpha_bar_region.x + scale_dpi( i ), alpha_bar_region.y, calc_len, calc_len ), color ( ).lerp ( alpha_clr_flip ? color ( 1.0f, 1.0f, 1.0f, 1.0f ) : color ( 0.82f, 0.82f, 0.82f, 1.0f ), window->second.anim_time [ window->second.cur_index + 6 ] ), true, true );
				draw_list.add_rect ( rect ( alpha_bar_region.x + scale_dpi ( i ), alpha_bar_region.y + scale_dpi( calc_len ), calc_len, calc_len ), color ( ).lerp ( alpha_clr_flip ? color ( 0.82f, 0.82f, 0.82f, 1.0f ) : color ( 1.0f, 1.0f, 1.0f, 1.0f ), window->second.anim_time [ window->second.cur_index + 6 ] ), true, true );

				alpha_clr_flip = !alpha_clr_flip;
			}

			const auto preview_square_side_len = preview_color.h * 0.5f;
			auto preview_alpha_clr_flip = false;
			const auto preview_remaining_len = std::fmodf ( preview_color.w, preview_square_side_len );

			for ( auto i = 0.0f; i < preview_color.w; i += preview_square_side_len ) {
				const auto calc_len = i > preview_color.w - preview_square_side_len ? preview_remaining_len : preview_square_side_len;

				draw_list.add_rect ( rect ( preview_color.x + scale_dpi ( i ), preview_color.y, calc_len, calc_len ), color ( ).lerp ( preview_alpha_clr_flip ? color ( 1.0f, 1.0f, 1.0f, 1.0f ) : color ( 0.82f, 0.82f, 0.82f, 1.0f ), window->second.anim_time [ window->second.cur_index + 6 ] ), true, true );
				draw_list.add_rect ( rect ( preview_color.x + scale_dpi ( i ), preview_color.y + scale_dpi ( calc_len ), calc_len, calc_len ), color ( ).lerp ( preview_alpha_clr_flip ? color ( 0.82f, 0.82f, 0.82f, 1.0f ) : color ( 1.0f, 1.0f, 1.0f, 1.0f ), window->second.anim_time [ window->second.cur_index + 6 ] ), true, true );

				preview_alpha_clr_flip = !preview_alpha_clr_flip;
			}

			draw_list.add_rect_gradient (
				alpha_bar_region,
				color ( ).lerp ( color( option.r, option.g, option.b, 0.0f ), window->second.anim_time [ window->second.cur_index + 6 ] ),
				color ( ).lerp ( color ( option.r, option.g, option.b, 1.0f ), window->second.anim_time [ window->second.cur_index + 6 ] ),
				true,
				true,
				true
			);

			draw_list.add_rect ( preview_color, color ( ).lerp ( option, window->second.anim_time [ window->second.cur_index + 6 ] ), true, true );

			const auto step = 5;

			color as_hsv = option.to_hsv();
			as_hsv.a = 1.0f;

			vec2 offset = vec2 ( 0.0f, 0.0f );

			/* credits to ocornut ( ocornut's IMGUI ) */
			for ( auto y = 0; y < step; y++ ) {
				for ( auto x = 0; x < step; x++ ) {
					const auto s0 = ( float ) x / ( float ) step;
					const auto s1 = ( float ) ( x + 1 ) / ( float ) step;
					const auto v0 = 1.0 - ( float ) ( y ) / ( float ) step;
					const auto v1 = 1.0 - ( float ) ( y + 1 ) / ( float ) step;

					color c00 = color ( as_hsv.r, s0, v0, 1.0f ).to_rgb ( );
					color c10 = color ( as_hsv.r, s1, v0, 1.0f ).to_rgb ( );
					color c01 = color ( as_hsv.r, s0, v1, 1.0f ).to_rgb ( );
					color c11 = color ( as_hsv.r, s1, v1, 1.0f ).to_rgb ( );

					draw_list.add_rect_multicolor ( rect( color_square_region.x + scale_dpi( offset.x ), color_square_region.y + scale_dpi( offset.y ), color_square_region.w / step, color_square_region.h / step ), color ( ).lerp ( c00, window->second.anim_time [ window->second.cur_index + 6 ] ), color ( ).lerp ( c10, window->second.anim_time [ window->second.cur_index + 6 ] ), color ( ).lerp ( c01, window->second.anim_time [ window->second.cur_index + 6 ] ), color ( ).lerp ( c11, window->second.anim_time [ window->second.cur_index + 6 ] ), true, true );

					offset.x += color_square_region.w / step;
				}

				offset.x = 0;
				offset.y += color_square_region.h / step;
			}

			/* picker circle */
			draw_list.add_circle ( vec2 ( color_square_region.x + as_hsv.g * scale_dpi ( color_square_region.w ), color_square_region.y + ( 1.0f - as_hsv.b ) * scale_dpi ( color_square_region.h ) ), 8.0f, color ( ).lerp ( color ( 1.0f, 1.0f, 1.0f, 0.6f ), window->second.anim_time [ window->second.cur_index + 6 ] ), false, true );
			draw_list.add_circle ( vec2 ( color_square_region.x + as_hsv.g * scale_dpi ( color_square_region.w ), color_square_region.y + ( 1.0f - as_hsv.b ) * scale_dpi ( color_square_region.h ) ), 8.0f + unscale_dpi ( 1.0f ), color ( ).lerp ( color ( 0.0f, 0.0f, 0.0f, 0.3f ), window->second.anim_time [ window->second.cur_index + 6 ] ), false, true );

			/* hue picker circle */
			draw_list.add_circle ( vec2 ( hue_bar_region.x + scale_dpi ( hue_bar_region.w ) * 0.5f, hue_bar_region.y + scale_dpi ( hue_bar_region.h ) * as_hsv.r ), 8.0f, color ( ).lerp ( color ( 1.0f, 1.0f, 1.0f, 0.6f ), window->second.anim_time [ window->second.cur_index + 6 ] ), false, true );
			draw_list.add_circle ( vec2 ( hue_bar_region.x + scale_dpi ( hue_bar_region.w ) * 0.5f, hue_bar_region.y + scale_dpi ( hue_bar_region.h ) * as_hsv.r ), 8.0f + unscale_dpi ( 1.0f ), color ( ).lerp ( color ( 0.0f, 0.0f, 0.0f, 0.3f ), window->second.anim_time [ window->second.cur_index + 6 ] ), false, true );

			/* alpha picker circle */
			draw_list.add_circle ( vec2 ( alpha_bar_region.x + scale_dpi ( alpha_bar_region.w ) * option.a, alpha_bar_region.y + scale_dpi ( alpha_bar_region.h ) * 0.5f ), 8.0f, color ( ).lerp ( color ( 1.0f, 1.0f, 1.0f, 0.6f ), window->second.anim_time [ window->second.cur_index + 6 ] ), false, true );
			draw_list.add_circle ( vec2 ( alpha_bar_region.x + scale_dpi ( alpha_bar_region.w ) * option.a, alpha_bar_region.y + scale_dpi ( alpha_bar_region.h ) * 0.5f ), 8.0f + unscale_dpi ( 1.0f ), color ( ).lerp ( color ( 0.0f, 0.0f, 0.0f, 0.3f ), window->second.anim_time [ window->second.cur_index + 6 ] ), false, true );
		}

		const auto backup_clip_enabled = globals::clip_enabled;
		globals::clip_enabled = false;

		if ( window->second.anim_time [ window->second.cur_index + 4 ] > 0.0f ) {
			const auto calculated_height = style.button_size.y * color_options.size ( );
			const auto list_rect = rect ( check_rect.x, check_rect.y + scale_dpi ( style.button_size.y + style.padding + style.padding ), style.button_size.x, calculated_height * window->second.anim_time [ window->second.cur_index + 4 ] );

			draw_list.add_rounded_rect ( list_rect, style.control_rounding, color ( style.control_background.r, style.control_background.g, style.control_background.b, 0.0f ).lerp ( style.control_background, window->second.anim_time [ window->second.cur_index + 4 ] ), true, true );
			draw_list.add_rounded_rect ( list_rect, style.control_rounding, color ( style.control_borders.r, style.control_borders.g, style.control_borders.b, 0.0f ).lerp ( style.control_borders, window->second.anim_time [ window->second.cur_index + 4 ] ), false, true );

			for ( auto i = 0; i < color_options.size ( ); i++ ) {
				vec2 text_size;
				draw_list.get_text_size ( style.control_font, color_options [ i ].data ( ), text_size );

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
							color_clipboard = option;
						} break;
						case 1: {
							option = color_clipboard;
						} break;
						}
					}
				}

				const auto lerped_color = style.control_accent.lerp ( style.control_text, std::clamp< float > ( std::fabsf ( static_cast< float > ( i ) - window->second.anim_time [ window->second.cur_index + 5 ] ), 0.0f, 1.0f ) );

				if ( list_rect.y + scale_dpi ( style.button_size.y * 0.5f + i * style.button_size.y ) + text_size.y * 0.5f > list_rect.y + scale_dpi ( list_rect.h ) )
					break;

				draw_list.add_text ( vec2 ( window->second.cursor_stack.back ( ).x + scale_dpi ( style.padding ), list_rect.y + scale_dpi ( style.button_size.y * 0.5f + i * style.button_size.y ) - text_size.y * 0.5f ), style.control_font, color_options [ i ].data ( ), true, color ( lerped_color.r, lerped_color.g, lerped_color.b, 0.0f ).lerp ( lerped_color, window->second.anim_time [ window->second.cur_index + 4 ] ), true );
			}
		}

		globals::clip_enabled = backup_clip_enabled;
	}

	window->second.last_cursor_offset = scale_dpi ( style.padding + style.inline_button_size.y );
	window->second.cursor_stack.back ( ).y += window->second.last_cursor_offset;
	window->second.cur_index += 7;
	window->second.tooltip.clear ( );

	if ( window->second.same_line ) {
		window->second.cursor_stack.back ( ).x = same_line_backup.x;
		window->second.cursor_stack.back ( ).y = std::max< float > ( same_line_backup.y, window->second.cursor_stack.back ( ).y );
		window->second.same_line = false;
	}
}