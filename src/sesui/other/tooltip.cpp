//sesui_packer:ignore
#include "../sesui.hpp"

//sesui_packer:resume
void sesui::tooltip ( const ses_string& tooltip ) {
	const auto window = globals::window_ctx.find ( globals::cur_window );

	if ( window == globals::window_ctx.end ( ) )
		throw "Current window context not valid.";

	window->second.tooltip = tooltip.get( );
}