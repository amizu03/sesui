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
#define SESUI_VER "0.0.1"

namespace sesui {
	namespace globals {
		extern float dpi;
	}

	inline float scale_dpi ( float x ) {
		return x * globals::dpi;
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

	using layer = uint8_t;

	enum class layer_constants : uint8_t {
		topmost = 0xff
	};
	
	namespace input {
		extern std::array< bool, 256 > key_state;
		extern std::array< bool, 256 > old_key_state;
		extern vec2 mouse_pos;

		/* call before any drawing is handled to gather input */
		SESUI_API void get_input ( const ses_string& window );

		SESUI_API bool key_pressed ( int key );

		SESUI_API bool key_down ( int key );

		SESUI_API bool key_released ( int key );

		SESUI_API bool mouse_in_region ( const rect& bounds );
	}

	namespace globals {
		struct window_ctx_t {
			rect bounds;
			layer layer;
			bool moving;
			vec2 click_offset;
		};

		extern std::map< std::basic_string_view< ses_char >, window_ctx_t > window_ctx;
		extern ses_string cur_window;
	}

	struct style_t {
		color window_background = color ( 45, 50, 56, 255 );
		color window_foreground = color ( 66, 70, 77, 255 );
		color window_borders = color ( 89, 92, 99, 255 );
		color window_accent = color ( 255, 0, 77, 255 );
		color window_accent_borders = color ( 255, 0 + 125, 77 + 125, 255 );

		/* percentage */
		float titlebar_height = 0.1f /* 1/10 */;

		float rounding = 4.0f;
	};

	extern style_t style;

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

			/* layer to rendor object on (lowere layer will be drawn first) */
			layer layer;

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
			ses_string text;
			bool text_shadow;
		};

		/* list of objects to draw*/
		std::vector< object_t > objects { };

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

		/* these methods must be defined in order for sesui to render any objects */
		draw_polygon_fn draw_polygon;
		draw_text_fn draw_text;
		get_text_size_fn get_text_size;
		get_frametime_fn get_frametime;
		begin_clip_fn begin_clip;
		end_clip_fn end_clip;
		
		void add_rounded_rect ( const rect& rectangle, float rad, const color& color, bool filled ) {
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

			objects.push_back ( object_t {
				object_type::polygon,
				globals::window_ctx [ globals::cur_window.get ( ) ].layer,
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

		void add_rect ( const rect& rectangle, const color& color, bool filled ) {
			const auto scaled_w = scale_dpi ( rectangle.w );
			const auto scaled_h = scale_dpi ( rectangle.h );

			std::vector< vec2 > verticies {
				vec2 ( rectangle.x, rectangle.y ),
				vec2 ( rectangle.x + scaled_w, rectangle.y ),
				vec2 ( rectangle.x + scaled_w, rectangle.y + scaled_h ),
				vec2 ( rectangle.x, rectangle.y + scaled_h )
			};

			objects.push_back ( object_t {
				object_type::polygon,
				globals::window_ctx [ globals::cur_window.get ( ) ].layer,
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

		void add_text ( const vec2& pos, const font& font, const ses_string& text, bool text_shadow, const color& color ) {
			if ( !font.data )
				throw "Attempted to add text using invalid font to draw list.";

			objects.push_back ( object_t {
				object_type::text,
				globals::window_ctx [ globals::cur_window.get ( ) ].layer,
				color,
				{},
				clip_mode::none,
				{},
				false,
				pos,
				font,
				text,
				text_shadow
			} );
		}

		void render ( ) {
			if ( !draw_polygon
				|| !draw_text
				|| !get_text_size
				|| !get_frametime )
				throw "One or more of the methods required to render the draw list were undefined.";

			if ( objects.empty ( ) )
				return;

			/* sort objects by layer, so we draw object on bottom layers before ones on top */
			std::sort ( objects.begin ( ), objects.end ( ), [ ] ( const object_t& lhs, const object_t& rhs ) {
				return lhs.layer < rhs.layer;
			} );

			/* loop through all objects, sort by layer, and draw all at once */
			for ( const auto& object : objects ) {
				switch ( object.type ) {
				case object_type::polygon:
					draw_polygon ( object.verticies, object.color, object.filled );
					break;
				case object_type::text:
					if ( !object.font.data )
						throw "Attempted to draw text using invalid font.";

					if ( object.text_shadow )
						draw_text ( object.text_pos + vec2 ( 2.0f, 2.0f ), object.font, object.text, object.color );

					draw_text ( object.text_pos, object.font, object.text, object.color );
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
			objects.clear ( );
		}
	};

	extern c_draw_list draw_list;

	/* ------------------------------- */
	/* -- SESUI API FUNCTIONS BELOW -- */
	/* ------------------------------- */

	/* begins ui frame */
	SESUI_API void begin_frame ( const ses_string& window );

	/* ends ui frame */
	SESUI_API void end_frame ( );

	/* creates new window */
	SESUI_API void begin_window ( const ses_string& title, const rect& bounds );
	SESUI_API void end_window ( );

	/* call function after all ui logic to render all ui objects */
	SESUI_API void render ( );
}