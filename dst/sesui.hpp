#pragma once
#include <cstdint>
#include <string>
#include <string_view>
#include <vector>
#include <stdexcept>
#include <algorithm>
#include <cmath>
#include <array>
#include <map>
#include <unordered_map>

/* do not modify */
#define SESUI_API __declspec( dllexport )

/* do not modify */
#define SESUI_VER "0.3.0"

namespace sesui {
	namespace globals {
		extern float dpi;
		extern float last_dpi;
	}

	inline float scale_dpi ( float x ) {
		return x * globals::dpi;
	}

	inline float unscale_dpi ( float x ) {
		return x / globals::dpi;
	}

	/* constants */
	constexpr auto pi = 3.14159265358979f;

	namespace math {
		template < typename type >
		constexpr type rad2deg ( type rad ) {
			return rad * ( 180.0f / pi );
		}

		template < typename type >
		constexpr type deg2rad ( type deg ) {
			return deg * ( pi / 180.0f );
		}

		template < typename type >
		constexpr type sin ( type x ) {
			return std::sinf ( x );
		}

		template < typename type >
		constexpr type cos ( type x ) {
			return std::cosf ( x );
		}
	}

	/* ----------------------------- */
	/* -- SESUI DEFINITIONS BELOW -- */
	/* ----------------------------- */

	/* character type */
	using ses_char = wchar_t;

	/* string type using the character type we chose */
	class ses_string {
		std::basic_string_view< ses_char > str;

	public:
		/* create string using the new string type we made (this is so we don't have to change the string type everywhere depending on what format we chose) */
		ses_string ( const char* data ) {
			str = reinterpret_cast< const ses_char* > ( data );
		}

		ses_string ( char* data ) {
			str = reinterpret_cast< ses_char* > ( data );
		}

		ses_string ( const wchar_t* data ) {
			str = reinterpret_cast< const ses_char* > ( data );
		}

		ses_string ( wchar_t* data ) {
			str = reinterpret_cast< ses_char* > ( data );
		}

		ses_string ( ) {
			*this = ses_string ( "" );
		}

		auto get ( ) const {
			return str.data ( );
		}

		auto len ( ) const {
			return str.length ( );
		}

		bool empty ( )const {
			return !len ( );
		}

		ses_string& operator=( const ses_string& other ) {
			str = other.str;
			return *this;
		}

		/* pls dont look at this ghetto ass shit ;-; */
		ses_string operator+( const ses_string& other ) const {
			return ses_string( ( std::basic_string< ses_char > ( str ) + std::basic_string< ses_char > ( other.str ) ).c_str( ) );
		}
	};

	/* AUTOMATICALLY SCALE SHAPES USING DPI TO AVOID HEADACHES LATER */

	/* 2d vector type used for storing positions */
	struct vec2 {
		float x, y;

		template < typename type >
		vec2 ( type x, type y ) {
			this->x = static_cast < float > ( x );
			this->y = static_cast < float > ( y );
		}

		vec2 ( ) {
			this->x = 0.0f;
			this->y = 0.0f;
		}

		vec2 operator+( const vec2& other ) const {
			return vec2 ( x + other.x, y + other.y );
		}

		vec2 operator-( const vec2& other ) const {
			return vec2 ( x - other.x, y - other.y );
		}

		vec2& operator=( const vec2& other ) {
			x = other.x;
			y = other.y;
			return *this;
		}
	};

	/* rectangle type used for storing rectanglular shapes */
	struct rect {
		float x, y, w, h;

		template < typename type >
		rect ( type x, type y, type w, type h ) {
			this->x = static_cast < float > ( x );
			this->y = static_cast < float > ( y );
			this->w = static_cast < float > ( w );
			this->h = static_cast < float > ( h );
		}

		rect ( ) {
			this->x = 0.0f;
			this->y = 0.0f;
			this->w = 0.0f;
			this->h = 0.0f;
		}

		rect& operator=( const rect& other ) {
			x = other.x;
			y = other.y;
			w = other.w;
			h = other.h;
			return *this;
		}
	};

	/* color type capable of storing *most* colors that we would ever need */
	struct color {
		uint8_t r, g, b, a;

		template < typename type >
		color ( type r, type g, type b, type a ) {
			/* if they input color as floating points, they are most likely using floating point representations of colors (0 to 1) */
			if constexpr ( std::is_floating_point< type >::value ) {
				this->r = static_cast< uint8_t > ( r * 255.0f );
				this->g = static_cast< uint8_t > ( g * 255.0f );
				this->b = static_cast< uint8_t > ( b * 255.0f );
				this->a = static_cast< uint8_t > ( a * 255.0f );
				return;
			}

			this->r = static_cast< uint8_t > ( r );
			this->g = static_cast< uint8_t > ( g );
			this->b = static_cast< uint8_t > ( b );
			this->a = static_cast< uint8_t > ( a );
		}

		color& operator=( const color& other ) {
			r = other.r;
			g = other.g;
			b = other.b;
			a = other.a;
			return *this;
		}

		color lerp ( const color& to, float fraction ) {
			const auto clamped_fraction = std::clamp < float > ( fraction, 0.0f, 1.0f );

			return color (
				static_cast< int > ( static_cast< float >( to.r - r ) * clamped_fraction + static_cast< float >( r ) ),
				static_cast< int > ( static_cast< float >( to.g - g ) * clamped_fraction + static_cast< float >( g ) ),
				static_cast< int > ( static_cast< float >( to.b - b ) * clamped_fraction + static_cast< float >( b ) ),
				static_cast< int > ( static_cast< float >( to.a - a ) * clamped_fraction + static_cast< float >( a ) )
			);
		}
	};

	/* font type, placeholder pointer to actual font type you will actually use (temporary for now) */
	struct font {
		void* data;
		ses_string family;
		int size, weight;
		bool italic;

		font ( const ses_string& family, int size, int weight, bool italic ) {
			this->family = family;
			this->size = size;
			this->weight = weight;
			this->italic = italic;
		}

		font ( ) {

		}
	};

	enum window_flags : uint32_t {
		none = ( 1 << 0 ),
		no_resize = ( 1 << 1 ),
		no_move = ( 1 << 2 ),
		no_title = ( 1 << 3 ),
		no_closebutton = ( 1 << 4 )
	};

	using layer = uint8_t;

	enum class layer_constants : layer {
		topmost = 0xff
	};
	
	namespace input {
		extern std::array< bool, 256 > key_state;
		extern std::array< bool, 256 > old_key_state;
		extern vec2 start_click_pos;
		extern vec2 mouse_pos;
		extern bool enabled;
		extern bool queue_enable;
		extern float scroll_amount;

		/* call before any drawing is handled to gather input */
		SESUI_API void enable_input ( bool enabled );

		SESUI_API void get_scroll_amount ( const float scroll_amount );

		SESUI_API void get_input ( const ses_string& window );

		SESUI_API bool key_pressed ( int key );

		SESUI_API bool key_down ( int key );

		SESUI_API bool key_released ( int key );

		SESUI_API bool in_clip ( const vec2& pos );

		SESUI_API bool mouse_in_region ( const rect& bounds, bool force = false );

		SESUI_API bool click_in_region ( const rect& bounds, bool force = false );
	}

	namespace globals {
		struct group_ctx_t {
			float calc_height = 0.0f;
			float scroll_amount = 0.0f;
			float scroll_amount_target = 0.0f;
		};

		struct window_ctx_t {
			rect bounds;
			layer layer;
			bool moving;
			bool resizing;
			vec2 click_offset;
			std::array< float, 256 > anim_time;
			std::array< float, 256 > hover_time;
			std::basic_string < ses_char > selected_tooltip;
			std::basic_string < ses_char > tooltip;
			std::vector< vec2 > cursor_stack;
			std::map< std::basic_string < ses_char >, group_ctx_t > group_ctx;
			rect main_area;
			float last_cursor_offset = 0.0f;
			float tooltip_anim_time = 0.0f;
			float tab_width = 0.0f;
			float selected_tab_offset = 0.0f;
			int tab_count;
			int cur_index;
			int cur_tab_index;
			bool same_line = false;
			std::basic_string < ses_char > cur_group;
		};

		extern rect clip;
		extern bool clip_enabled;
		extern std::map< std::basic_string< ses_char >, window_ctx_t > window_ctx;
		extern std::basic_string< ses_char > cur_window;
	}

	struct style_t {
		color window_background = color ( 45, 50, 56, 255 );
		color window_foreground = color ( 66, 70, 77, 255 );
		color window_borders = color ( 89, 92, 99, 255 );
		color window_accent = color ( 255, 0, 77, 255 );
		color window_accent_borders = color ( 255, 0 + 125, 77 + 125, 255 );

		vec2 window_min_size = vec2 ( 300.0f, 300.0f );

		/* percentage */
		float titlebar_height = 0.1f /* 1/10 */;
		float group_titlebar_height = 0.05f /* 1/20 */;

		vec2 initial_offset = vec2 ( 22.0f, 22.0f );
		float spacing = 8.0f;
		float padding = 6.0f;
		float resize_grab_radius = 6.0f;
		float same_line_offset = 100.0f;
		float scroll_arrow_height = 12.0f;

		float animation_speed = 4.0f;

		/* control colors */
		color control_background = color ( 66, 70, 77, 255 );
		color control_borders = color ( 89, 92, 99, 255 );
		color control_text = color ( 205, 205, 205, 255 );
		color control_text_hovered = color ( 255, 255, 255, 255 );
		color control_accent = color ( 255, 0, 77, 255 );
		color control_accent_borders = color ( 255, 0 + 125, 77 + 125, 255 );

		float rounding = 4.0f;
		float control_rounding = 4.0f;
		float tooltip_hover_time = 1.25f;
		
		/* control stuff */
		vec2 checkbox_size = vec2( 14.0f, 14.0f );
		vec2 slider_size = vec2 ( 160.0f, 8.0f );
		vec2 button_size = vec2 ( 160.0f, 22.0f );
		vec2 colorpicker_size = vec2 ( 14.0f, 14.0f );
		vec2 colorpicker_square_size = vec2 ( 200.0f, 200.0f );

		font control_font = font( L"Comfortaa Regular", 16, 400, false );
	};

	extern style_t style;

	namespace fonts {
		extern std::array< uint8_t, 140136 > font_default;
	}

	/* draw list object, keeps all polygons and text that will need to be drawn by the ui framework */
	class c_draw_list {
		enum class object_type : uint8_t {
			polygon = 0,
			text,
			clip,
		};

		enum class clip_mode : uint8_t {
			none = 0,
			begin,
			end
		};

		struct object_t {
			/* stores type of objects we are trying to draw (all types stored in object_type enum)*/
			object_type type;

			/* color applies to both polygons and text */
			color color;

			/* clip region */
			rect clip_rect;
			clip_mode clip_mode;

			/* polygon information */
			std::vector< vec2 > verticies;
			bool filled;

			/* text information */
			vec2 text_pos;
			font font;
			std::basic_string < ses_char > text;
			bool text_shadow;
		};

		/* list of objects to draw*/
		std::array < std::vector< object_t >, static_cast< int > ( layer_constants::topmost ) > objects { };

	public:
		/* mostly geometric primitives */
		using draw_polygon_fn = std::add_pointer_t< void ( const std::vector< vec2 > & verticies, const color & color, bool filled ) noexcept >;

		/* text rendering */
		using draw_text_fn = std::add_pointer_t< void ( const vec2 & pos, const font & font, const ses_string & text, const color & color ) noexcept >;
		using get_text_size_fn = std::add_pointer_t< void ( const font & font, const ses_string & text, vec2 & dim_out ) noexcept >;

		/* other stuff */
		using get_frametime_fn = std::add_pointer_t< float ( ) noexcept >;
		
		using begin_clip_fn = std::add_pointer_t< void ( const rect& region ) noexcept >;
		using end_clip_fn = std::add_pointer_t< void ( ) noexcept >;

		/* create font */
		using create_font_fn = std::add_pointer_t< void ( font& font, bool force ) noexcept >;

		/* these methods must be defined in order for sesui to render any objects */
		draw_polygon_fn draw_polygon;
		draw_text_fn draw_text;
		get_text_size_fn get_text_size;
		get_frametime_fn get_frametime;
		begin_clip_fn begin_clip;
		end_clip_fn end_clip;
		create_font_fn create_font;
		
		void add_rounded_rect ( const rect& rectangle, float rad, const color& color, bool filled, bool topmost = false ) {
			const auto scaled_rad = scale_dpi ( rad );
			const auto scaled_resolution = static_cast< int > ( scaled_rad * 0.666f + 0.5f );

			const auto scaled_w = scale_dpi ( rectangle.w );
			const auto scaled_h = scale_dpi ( rectangle.h );

			std::vector< vec2 > verticies ( 4 * scaled_resolution );

			for ( auto i = 0; i < 4; i++ ) {
				const auto x = rectangle.x + ( ( i < 2 ) ? ( scaled_w - scaled_rad ) : scaled_rad );
				const auto y = rectangle.y + ( ( i % 3 ) ? ( scaled_h - scaled_rad ) : scaled_rad );
				const auto a = 90.0f * i;

				for ( auto j = 0; j < scaled_resolution; j++ ) {
					const auto a1 = math::deg2rad ( a + ( j / ( float ) ( scaled_resolution - 1 ) ) * 90.0f );
					
					verticies [ i * scaled_resolution + j ] = { x + scaled_rad * math::sin ( a1 ), y - scaled_rad * math::cos ( a1 ) };
				}
			}

			objects [ topmost ? static_cast < int >( layer_constants::topmost ) - 1 : static_cast < int >( globals::window_ctx [ globals::cur_window ].layer ) ].push_back ( object_t {
				object_type::polygon,
				color,
				{},
				clip_mode::none,
				verticies,
				filled,
				{},
				{},
				{},
				false
				} );
		}

		void add_rect ( const rect& rectangle, const color& color, bool filled, bool topmost = false ) {
			const auto scaled_w = scale_dpi ( rectangle.w );
			const auto scaled_h = scale_dpi ( rectangle.h );

			std::vector< vec2 > verticies {
				vec2 ( rectangle.x, rectangle.y ),
				vec2 ( rectangle.x + scaled_w, rectangle.y ),
				vec2 ( rectangle.x + scaled_w, rectangle.y + scaled_h ),
				vec2 ( rectangle.x, rectangle.y + scaled_h )
			};

			objects [ topmost ? static_cast < int >( layer_constants::topmost ) - 1 : static_cast < int >( globals::window_ctx [ globals::cur_window ].layer ) ].push_back ( object_t {
				object_type::polygon,
				color,
				{},
				clip_mode::none,
				verticies,
				filled,
				{},
				{},
				{},
				false
			} );
		}

		void add_line ( const vec2& p1, const vec2& p2, const color& color, bool topmost = false ) {
			std::vector< vec2 > verticies {
				p1,
				p2
			};

			objects [ topmost ? static_cast < int >( layer_constants::topmost ) - 1 : static_cast < int >( globals::window_ctx [ globals::cur_window ].layer ) ].push_back ( object_t {
				object_type::polygon,
				color,
				{},
				clip_mode::none,
				verticies,
				false,
				{},
				{},
				{},
				false
				} );
		}

		void add_arrow ( const vec2& pos, float size, float rotation, const color& color, bool filled, bool topmost = false ) {
			const auto scaled_size = scale_dpi ( size );

			const auto ang1 = math::deg2rad ( -90.0f + rotation );
			const auto ang2 = math::deg2rad ( 90.0f + rotation );
			const auto ang3 = math::deg2rad ( 0.0f + rotation );

			std::vector< vec2 > verticies;

			if ( !filled ) {
				verticies.push_back ( vec2 ( pos.x - math::cos ( ang2 ) * scaled_size, pos.y - math::sin ( ang2 ) * scaled_size ) );
				verticies.push_back ( vec2 ( pos.x - math::cos ( ang3 ) * scaled_size, pos.y - math::sin ( ang3 ) * scaled_size ) );
				verticies.push_back ( vec2 ( pos.x - math::cos ( ang1 ) * scaled_size, pos.y - math::sin ( ang1 ) * scaled_size ) );
				verticies.push_back ( vec2 ( pos.x - math::cos ( ang3 ) * scaled_size, pos.y - math::sin ( ang3 ) * scaled_size ) );
			}
			else {
				verticies.push_back ( vec2 ( pos.x - math::cos ( ang2 ) * scaled_size, pos.y - math::sin ( ang2 ) * scaled_size ) );
				verticies.push_back ( vec2 ( pos.x - math::cos ( ang3 ) * scaled_size, pos.y - math::sin ( ang3 ) * scaled_size ) );
				verticies.push_back ( vec2 ( pos.x - math::cos ( ang1 ) * scaled_size, pos.y - math::sin ( ang1 ) * scaled_size ) );
			}

			objects [ topmost ? static_cast < int >( layer_constants::topmost ) - 1 : static_cast < int >( globals::window_ctx [ globals::cur_window ].layer ) ].push_back ( object_t {
				object_type::polygon,
				color,
				{},
				clip_mode::none,
				verticies,
				filled,
				{},
				{},
				{},
				false
				} );
		}

		void add_text ( const vec2& pos, const font& font, const ses_string& text, bool text_shadow, const color& color, bool topmost = false ) {
			if ( !font.data )
				throw "Attempted to add text using invalid font to draw list.";

			objects [ topmost ? static_cast < int >( layer_constants::topmost ) - 1 : static_cast < int >( globals::window_ctx [ globals::cur_window ].layer ) ].push_back ( object_t {
				object_type::text,
				color,
				{},
				clip_mode::none,
				{},
				false,
				pos,
				font,
				text.get(),
				text_shadow
			} );
		}

		void add_clip ( const rect& area, bool topmost = false ) {
			globals::clip_enabled = true;
			globals::clip = area;

			objects [ topmost ? static_cast < int >( layer_constants::topmost ) - 1 : static_cast < int >( globals::window_ctx [ globals::cur_window ].layer ) ].push_back ( object_t {
				object_type::clip,
				color ( 0,0,0,0 ),
				rect( area.x, area.y, scale_dpi ( area.w ), scale_dpi ( area.h ) ),
				clip_mode::begin,
				{},
				false,
				{},
				{ },
				L"",
				false
				} );
		}

		void remove_clip ( bool topmost = false ) {
			globals::clip_enabled = false;

			objects [ topmost ? static_cast < int >( layer_constants::topmost ) - 1 : static_cast < int >( globals::window_ctx [ globals::cur_window ].layer ) ].push_back ( object_t {
				object_type::clip,
				color ( 0,0,0,0 ),
				{},
				clip_mode::end,
				{},
				false,
				{},
				{ },
				L"",
				false
				} );
		}

		void render ( ) {
			if ( !draw_polygon
				|| !draw_text
				|| !get_text_size
				|| !get_frametime )
				throw "One or more of the methods required to render the draw list were undefined.";

			/* organized in layer order */
			for ( auto& layer : objects ) {
				if ( layer.empty ( ) )
					continue;

				/* loop through all objects, sort by layer, and draw all at once */
				for ( const auto& object : layer ) {
					switch ( object.type ) {
					case object_type::polygon:
						draw_polygon ( object.verticies, object.color, object.filled );
						break;
					case object_type::text:
						if ( !object.font.data )
							throw "Attempted to draw text using invalid font.";

						if ( object.text_shadow )
							draw_text ( object.text_pos + vec2 ( 1.0f, 1.0f ), object.font, object.text.data ( ), color ( 0, 0, 0, static_cast< int >( object.color.a ) ) );

						draw_text ( object.text_pos, object.font, object.text.data ( ), object.color );
						break;
					case object_type::clip:
						if ( object.clip_mode == clip_mode::begin )
							begin_clip ( object.clip_rect );
						else if ( object.clip_mode == clip_mode::end )
							end_clip ( );
						break;
					}
				}

				/* clear draw list after we already rendered all the objects! */
				layer.clear ( );
			}
		}
	};

	extern c_draw_list draw_list;

	/* ------------------------------- */
	/* -- SESUI API FUNCTIONS BELOW -- */
	/* ------------------------------- */

	/* splits text into 2 parts: 1. label 2. hidden id*/
	std::pair< std::basic_string< sesui::ses_char >, std::basic_string< sesui::ses_char > > split ( std::basic_string< sesui::ses_char > val );

	/* begins ui frame */
	SESUI_API void begin_frame ( const ses_string& window );

	/* ends ui frame */
	SESUI_API void end_frame ( );

	/* creates new window */
	SESUI_API bool begin_window ( const ses_string& name, const rect& bounds, bool& opened, uint32_t flags = window_flags::none );

	SESUI_API void end_window ( );

	SESUI_API void same_line ( );

	/* groups */
	SESUI_API bool begin_group ( const ses_string& name, const rect& fraction, const rect& extra );

	SESUI_API void end_group ( );

	/* tabs */
	SESUI_API bool begin_tabs ( int count, float width = 0.2f );

	SESUI_API void tab ( const ses_string& name, int& selected );

	SESUI_API void end_tabs ( );

	/* other gui stuff */
	SESUI_API void tooltip ( const ses_string& tooltip );

	/* gui controls */
	SESUI_API void checkbox ( const ses_string& name, bool& option );

	SESUI_API bool button ( const ses_string& name );

	SESUI_API void colorpicker ( const ses_string& name, color& option );

	SESUI_API void combobox ( const ses_string& name, int& option, const std::vector< ses_string >& list );
	
	SESUI_API void multiselect ( const ses_string& name, const std::vector< std::pair< ses_string, bool& > >& list );

	SESUI_API void slider_ex ( const ses_string& name, float& option, float min, float max, const ses_string& value_str );

	template < typename type >
	inline void slider ( const ses_string& name, type& option, type min, type max, const ses_string fmt = L"" ) {
		ses_string final_fmt = L"%d";

		if constexpr ( std::is_floating_point< type >::value )
			final_fmt = L"%.1f";

		if ( !fmt.empty ( ) )
			final_fmt = fmt;

		wchar_t value_str [ 32 ];
		swprintf_s ( value_str, final_fmt.get ( ), option );

		float tmp = static_cast < float > ( option );
		slider_ex ( name, tmp, min, max, value_str );
		option = tmp;
	}

	/* call function after all ui logic to render all ui objects */
	SESUI_API void render ( );
}