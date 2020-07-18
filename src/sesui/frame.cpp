//sesui_packer:ignore
#include <windows.h>
#include "sesui.hpp"

//sesui_packer:resume
void sesui::begin_frame ( const ses_string& window ) {
	sesui::input::get_input ( window );
}

/* ends ui frame */
void sesui::end_frame ( ) {
	globals::cur_window = "";
}