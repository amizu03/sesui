//sesui_packer:ignore
#include "sesui.hpp"

//sesui_packer:resume
std::map< std::basic_string_view< sesui::ses_char >, sesui::globals::window_ctx_t > sesui::globals::window_ctx;
sesui::ses_string sesui::globals::cur_window = "";
float sesui::globals::dpi = 1.0f;

sesui::style_t sesui::style;
sesui::c_draw_list sesui::draw_list;

void sesui::render ( ) {
	draw_list.render ( );
}