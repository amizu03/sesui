//sesui_packer:ignore
#include <windows.h>
#include "../sesui.hpp"

//sesui_packer:resume
bool sesui::begin_group ( const ses_string& name, const rect& fraction, const rect& extra ) {
	const auto window = globals::window_ctx.find ( globals::cur_window );

	if ( window == globals::window_ctx.end ( ) )
		throw "Current window context not valid.";

	const auto parts = split ( name.get ( ) );
	const auto title = ses_string ( parts.first.data ( ) );
	const auto& id = parts.second;

	window->second.cur_group = parts.first + id;

	const auto same_line_backup_x = window->second.cursor_stack.back ( ).x;

	if ( window->second.same_line ) {
		window->second.cursor_stack.back ( ).y -= window->second.last_cursor_offset;
		window->second.cursor_stack.back ( ).x += style.same_line_offset;
	}

	auto bounds = rect( window->second.cursor_stack.back ( ).x + scale_dpi( extra.x ), window->second.cursor_stack.back ( ).y + scale_dpi ( extra.y ), window->second.main_area.w * fraction.w + extra.w, window->second.main_area.h * fraction.h + extra.h );

	bounds.x += scale_dpi( window->second.main_area.w * fraction.x );
	bounds.y += scale_dpi( window->second.main_area.h * fraction.y );

	auto titlebar_rect = rect ( bounds.x, bounds.y, bounds.w, bounds.h * style.group_titlebar_height + 6.0f );

	titlebar_rect = rect ( bounds.x, bounds.y, bounds.w, bounds.h * style.group_titlebar_height + 6.0f );

	const auto window_rect = rect ( bounds.x, bounds.y + scale_dpi ( titlebar_rect.h - 6.0f ), bounds.w, bounds.h - titlebar_rect.h + 6.0f );
	const auto remove_rounding_rect = rect ( titlebar_rect.x, titlebar_rect.y + scale_dpi ( titlebar_rect.h - 6.0f ), titlebar_rect.w, 6.0f * 2.0f );
	const auto remove_rounding_rect_filler = rect ( remove_rounding_rect.x + 1.0f, remove_rounding_rect.y + 1.0f, remove_rounding_rect.w - unscale_dpi ( 2.0f ), remove_rounding_rect.h + 6.0f );
	const auto exit_rect = rect ( bounds.x + scale_dpi ( bounds.w - 16.0f - 8.0f ), bounds.y + scale_dpi ( titlebar_rect.h - 6.0f ) * 0.5f - scale_dpi ( 8.0f ), 16.0f, 16.0f );

	const auto calc_height1 = window->second.group_ctx [ window->second.cur_group ].calc_height + scale_dpi( style.initial_offset.y * 2.0f );
	const auto max_height = window_rect.h > calc_height1 ? window_rect.h : calc_height1;

	if ( input::mouse_in_region ( bounds ) && input::scroll_amount != 0.0f ) {
		window->second.group_ctx [ window->second.cur_group ].scroll_amount_target -= input::scroll_amount * 18.0f;
		window->second.group_ctx [ window->second.cur_group ].scroll_amount_target = std::clamp< float > ( window->second.group_ctx [ window->second.cur_group ].scroll_amount_target, 0.0f, max_height - window_rect.h );
	}

	const auto delta = window->second.group_ctx [ window->second.cur_group ].scroll_amount_target - window->second.group_ctx [ window->second.cur_group ].scroll_amount;
	window->second.group_ctx [ window->second.cur_group ].scroll_amount += delta * style.animation_speed * 2.0f * draw_list.get_frametime ( );

	const auto percentage_scrolled = window->second.group_ctx [ window->second.cur_group ].scroll_amount / ( max_height - window_rect.h );

	/* window rect */
	draw_list.add_rounded_rect ( titlebar_rect, style.rounding, style.window_accent, true );
	draw_list.add_rounded_rect ( titlebar_rect, style.rounding, style.window_accent_borders, false );
	draw_list.add_rounded_rect ( window_rect, style.rounding, style.window_background, true );
	draw_list.add_rounded_rect ( window_rect, style.rounding, style.window_borders, false );

	/* covering rounding for top part of main window */
	draw_list.add_rect ( remove_rounding_rect, style.window_background, true );
	draw_list.add_rect ( remove_rounding_rect, style.window_borders, false );
	draw_list.add_rect ( remove_rounding_rect_filler, style.window_background, true );

	vec2 text_size;
	draw_list.get_text_size ( style.control_font, title, text_size );
	draw_list.add_text ( vec2 ( bounds.x + scale_dpi ( bounds.w ) * 0.5f - text_size.x * 0.5f, bounds.y + scale_dpi ( titlebar_rect.h - 6.0f ) * 0.5f - text_size.y * 0.5f ), style.control_font, title, true, color ( 200, 200, 200, 255 ) );

	//window->second.cursor_stack.back ( ).x += window->second.main_area.w * fraction.w + style.spacing;
	//window->second.last_cursor_offset = window->second.main_area.h * fraction.h + style.spacing;
	//window->second.cursor_stack.back ( ).y += window->second.last_cursor_offset;

	window->second.cursor_stack.push_back ( vec2 ( bounds.x + scale_dpi ( style.initial_offset.x ), bounds.y + scale_dpi ( titlebar_rect.h + style.initial_offset.y - window->second.group_ctx [ window->second.cur_group ].scroll_amount ) ) );
	window->second.cursor_stack.push_back ( vec2 ( bounds.x + scale_dpi ( style.initial_offset.x ), bounds.y + scale_dpi ( titlebar_rect.h + style.initial_offset.y - window->second.group_ctx [ window->second.cur_group ].scroll_amount ) ) );

	if ( window->second.same_line ) {
		window->second.cursor_stack.back ( ).x = same_line_backup_x;
		window->second.same_line = false;
	}

	const auto clip_area = rect ( window_rect.x + scale_dpi ( style.initial_offset.x ), window_rect.y + scale_dpi( style.initial_offset.y ), window_rect.w - style.initial_offset.x * 2.0f, window_rect.h - style.initial_offset.y * 2.0f );

	const auto viewable_ratio = scale_dpi ( clip_area.h ) / scale_dpi( window->second.group_ctx [ window->second.cur_group ].calc_height );
	const auto scrollbar_area = scale_dpi ( clip_area.h ) - scale_dpi( style.scroll_arrow_height ) * 2.0f;
	const auto thumb_height = scrollbar_area * viewable_ratio;
	const auto track_space = window->second.group_ctx [ window->second.cur_group ].calc_height - scale_dpi ( clip_area.h );
	const auto thumb_space = scale_dpi ( clip_area.h ) - thumb_height;
	const auto scroll_jump = track_space / thumb_space;

	draw_list.add_rect ( rect ( clip_area.x + scale_dpi ( clip_area.w ) - 2.0f, clip_area.y, unscale_dpi ( 2.0f ), clip_area.h ), color( 0, 0, 0, 50 ), true );
	draw_list.add_rect ( rect( clip_area.x + scale_dpi( clip_area.w ) - 2.0f, clip_area.y + percentage_scrolled * ( scale_dpi ( clip_area.h ) - thumb_height ), unscale_dpi( 2.0f ), unscale_dpi( thumb_height )), style.control_accent, true );

	draw_list.add_clip ( clip_area );

	return true;
}

void sesui::end_group ( ) {
	const auto window = globals::window_ctx.find ( globals::cur_window );

	if ( window == globals::window_ctx.end ( ) )
		throw "Current window context not valid.";

	window->second.group_ctx [ window->second.cur_group ].calc_height = unscale_dpi( window->second.cursor_stack.at ( window->second.cursor_stack.size ( ) - 1 ).y - window->second.cursor_stack.at ( window->second.cursor_stack.size ( ) - 2 ).y);
	window->second.cur_group = L"";
	window->second.cursor_stack.pop_back ( );
	window->second.cursor_stack.pop_back ( );

	draw_list.remove_clip ( );
}