//sesui_packer:ignore
#include <windows.h>
#include "../sesui.hpp"

//sesui_packer:resume
void sesui::slider_ex ( const ses_string& name, float& option, float min, float max, const ses_string& value_str ) {
	const auto window = globals::window_ctx.find ( globals::cur_window );

	if ( window == globals::window_ctx.end ( ) )
		throw "Current window context not valid.";

	const auto same_line_backup_x = window->second.cursor_stack.back ( ).x;

	if ( window->second.same_line ) {
		window->second.cursor_stack.back ( ).y -= window->second.last_cursor_offset;
		window->second.cursor_stack.back ( ).x += style.same_line_offset;
	}

	const auto parts = split ( name.get ( ) );
	const auto title = ses_string ( parts.first.data ( ) );
	const auto& id = parts.second;
	
	vec2 text_size;
	draw_list.get_text_size ( style.control_font, title, text_size );

	const rect slider_rect { window->second.cursor_stack.back ( ).x, window->second.cursor_stack.back ( ).y + text_size.y + scale_dpi ( style.padding ), style.slider_size.x, style.slider_size.y };
	const rect slider_rect_max { window->second.cursor_stack.back ( ).x, window->second.cursor_stack.back ( ).y, style.slider_size.x, style.slider_size.y + style.spacing + style.padding + unscale_dpi( text_size.y )};
	const auto frametime = draw_list.get_frametime ( );
	auto in_region = input::mouse_in_region ( slider_rect );
	auto in_region_active = input::mouse_in_region ( slider_rect_max );
	auto clicking = false;

	/* don't draw objects we don't need so our fps doesnt go to shit */
	auto should_draw = true;

	if ( !window->second.group_ctx.empty ( )
		&& !input::in_clip ( vec2 ( window->second.cursor_stack.back ( ).x + 1.0f, window->second.cursor_stack.back ( ).y ) )
		&& !input::in_clip ( vec2 ( window->second.cursor_stack.back ( ).x + 1.0f, window->second.cursor_stack.back ( ).y + scale_dpi ( 60.0f ) ) ) )
		should_draw = false;

	if ( in_region_active && !window->second.tooltip.empty ( ) )
		window->second.hover_time [ window->second.cur_index ] += frametime;
	else
		window->second.hover_time [ window->second.cur_index ] = 0.0f;

	if ( window->second.hover_time [ window->second.cur_index ] > style.tooltip_hover_time ) {
		window->second.tooltip_anim_time += frametime * style.animation_speed;
		window->second.selected_tooltip = window->second.tooltip;
	}

	if ( input::click_in_region ( slider_rect_max ) && input::key_down ( VK_LBUTTON ) ) {
		const auto mouse_delta = std::clamp< float > ( input::mouse_pos.x - slider_rect.x, 0.0f, scale_dpi ( slider_rect.w ) );
		option = static_cast< float >( mouse_delta / scale_dpi( slider_rect.w ) * ( static_cast< float > ( max ) - static_cast< float > ( min ) ) + static_cast< float > ( min ) );
		in_region = true;
		clicking = true;
	}

	float bar_width = ( static_cast< float >( option ) - static_cast< float > ( min ) ) / ( static_cast< float > ( max ) - static_cast< float > ( min ) ) * slider_rect.w;
	bar_width = std::clamp< float > ( bar_width, style.rounding * 2.0f, slider_rect.w );
	rect bar_rect = rect ( window->second.cursor_stack.back ( ).x, window->second.cursor_stack.back ( ).y + text_size.y + scale_dpi( style.padding ), bar_width, style.slider_size.y );

	window->second.anim_time [ window->second.cur_index ] = std::clamp< float > ( window->second.anim_time [ window->second.cur_index ] + ( in_region_active ? frametime : -frametime ) * style.animation_speed, 0.0f, 1.0f );
	
	if ( std::fabsf ( window->second.anim_time [ window->second.cur_index + 1 ] - bar_width / slider_rect.w ) <= frametime * style.animation_speed )
		window->second.anim_time [ window->second.cur_index + 1 ] = bar_width / slider_rect.w;
	else
		window->second.anim_time [ window->second.cur_index + 1 ] = std::clamp< float > ( window->second.anim_time [ window->second.cur_index + 1 ] + ( ( window->second.anim_time [ window->second.cur_index + 1 ] < bar_width / slider_rect.w ) ? frametime : -frametime ) * style.animation_speed, 0.0f, 1.0f );

	bar_rect.w = slider_rect.w * window->second.anim_time [ window->second.cur_index + 1 ];

	if ( should_draw ) {
		draw_list.add_rounded_rect ( slider_rect, style.control_rounding, style.control_background, true );
		draw_list.add_rounded_rect ( slider_rect, style.control_rounding, style.control_borders.lerp ( style.control_accent_borders, window->second.anim_time [ window->second.cur_index ] ), false );
		draw_list.add_rounded_rect ( bar_rect, style.control_rounding, style.control_accent, true );
		draw_list.add_rounded_rect ( bar_rect, style.control_rounding, style.control_accent_borders, false );

		/* label */
		draw_list.add_text ( window->second.cursor_stack.back ( ), style.control_font, title, false, style.control_text.lerp ( style.control_text_hovered, window->second.anim_time [ window->second.cur_index ] ) );

	}
	
	/* value with formatting */
	vec2 value_size;
	draw_list.get_text_size ( style.control_font, value_str, value_size );

	if ( should_draw ) {
		draw_list.add_text ( vec2 ( window->second.cursor_stack.back ( ).x + scale_dpi ( slider_rect.w ) - value_size.x, window->second.cursor_stack.back ( ).y ), style.control_font, value_str, false, style.control_text.lerp ( style.control_text_hovered, window->second.anim_time [ window->second.cur_index ] ) );
	}
	
	window->second.last_cursor_offset = scale_dpi ( style.slider_size.y + style.spacing + style.padding ) + text_size.y;
	window->second.cursor_stack.back ( ).y += window->second.last_cursor_offset;
	window->second.cur_index += 2;
	window->second.tooltip.clear ( );

	if ( window->second.same_line ) {
		window->second.cursor_stack.back ( ).x = same_line_backup_x;
		window->second.same_line = false;
	}
}