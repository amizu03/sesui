//sesui_packer:ignore
#include <windows.h>
#include "sesui.hpp"

//sesui_packer:resume
void sesui::begin_frame ( const ses_string& window ) {
	/* create our fonts */
	sesui::draw_list.create_font ( style.control_font, globals::dpi != globals::last_dpi );

	globals::last_dpi = globals::dpi;

	/* poll input */
	sesui::input::get_input ( window );
}

/* ends ui frame */
void sesui::end_frame ( ) {
	globals::cur_window = "";
}