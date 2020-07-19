//sesui_packer:ignore
#include <windows.h>
#include "../sesui.hpp"

//sesui_packer:resume
void sesui::checkbox ( const ses_string& title, bool& option ) {
	const auto window = globals::window_ctx.find ( globals::cur_window.get ( ) );

	if ( window == globals::window_ctx.end ( ) )
		throw "Current window context not valid.";

	const rect check_rect { window->second.cursor.x, window->second.cursor.y, style.checkbox_size.x, style.checkbox_size.y };
	const auto frametime = draw_list.get_frametime ( );

	vec2 text_size;
	draw_list.get_text_size ( style.control_font, title, text_size );

	const auto max_height = std::max< float > ( scale_dpi ( check_rect.h ), text_size.y );
	const auto max_height_unscaled = std::max< float > ( check_rect.h, unscale_dpi( text_size.y ) );
	const auto in_region = input::mouse_in_region ( rect ( check_rect.x, check_rect.y + scale_dpi ( style.checkbox_size.y ) * 0.5f - max_height * 0.5f, style.checkbox_size.x + style.padding + unscale_dpi ( text_size.x), max_height_unscaled )  );

	if ( in_region && !window->second.tooltip.empty ( ) )
		window->second.hover_time [ window->second.cur_index ] += frametime;
	else
		window->second.hover_time [ window->second.cur_index ] = 0.0f;

	if ( window->second.hover_time [ window->second.cur_index ] > style.tooltip_hover_time ) {
		window->second.tooltip_anim_time += frametime * style.animation_speed;
		window->second.selected_tooltip = window->second.tooltip;
	}

	if ( in_region && input::key_pressed ( VK_LBUTTON ) )
		option = !option;

	window->second.anim_time [ window->second.cur_index ] = std::clamp< float > ( window->second.anim_time [ window->second.cur_index ] + ( option ? frametime : -frametime ) * style.animation_speed, 0.0f, 1.0f );
	window->second.anim_time [ window->second.cur_index + 1 ] = std::clamp< float > ( window->second.anim_time [ window->second.cur_index + 1 ] + ( in_region ? frametime : -frametime ) * style.animation_speed, 0.0f, 1.0f );

	draw_list.add_rounded_rect ( check_rect, style.control_rounding, style.control_background.lerp ( style.control_accent, window->second.anim_time [ window->second.cur_index ] ), true );
	draw_list.add_rounded_rect ( check_rect, style.control_rounding, style.control_borders.lerp( option ? style.control_accent_borders : style.control_accent, window->second.anim_time [ window->second.cur_index + 1 ] ), false );
	draw_list.add_text ( vec2( check_rect.x + scale_dpi( style.checkbox_size.x + style.padding ), check_rect.y + scale_dpi( style.checkbox_size.y ) * 0.5f - text_size.y * 0.5f ), style.control_font, title, false, style.control_text.lerp( style.control_text_hovered, window->second.anim_time [ window->second.cur_index + 1 ] ) );

	window->second.cursor.y += scale_dpi ( style.checkbox_size.y + style.spacing );
	window->second.cur_index += 2;
	window->second.tooltip.clear ( );
}