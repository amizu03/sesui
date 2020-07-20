//sesui_packer:ignore
#include "sesui.hpp"

//sesui_packer:resume
sesui::rect sesui::globals::clip = rect( 0, 0, 0, 0 );
bool sesui::globals::clip_enabled = false;
std::map< std::basic_string< sesui::ses_char >, sesui::globals::window_ctx_t > sesui::globals::window_ctx;
std::basic_string< sesui::ses_char > sesui::globals::cur_window = L"";
float sesui::globals::last_dpi = 1.0f;
float sesui::globals::dpi = 1.0f;

sesui::style_t sesui::style;
sesui::c_draw_list sesui::draw_list;

std::pair< std::basic_string< sesui::ses_char >, std::basic_string< sesui::ses_char > > sesui::split ( std::basic_string< sesui::ses_char > val ) {
	std::basic_string< ses_char > arg;
	const auto pos = val.find ( '#' );

	if ( val.npos != pos ) {
		arg = val.substr ( pos + 1 );
		val = val.substr ( 0, pos );
	}

	return make_pair ( val, arg );
}

void sesui::render ( ) {
	draw_list.render ( );
}