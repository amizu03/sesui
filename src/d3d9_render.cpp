#include "d3d9_render.hpp"
#include "sesui/sesui.hpp"

struct vertex_t {
	float x, y, z, rhw;
	uint32_t color;
};

struct custom_vertex_t {
	float x, y, z, rhw;
	uint32_t color;
	float tu, tv;
};

float render::frametime = 0.0f;
IDirect3DDevice9* render::device = nullptr;

void render::create_font ( sesui::font& font, bool force ) noexcept {
	if ( force && font.data ) {
		reinterpret_cast< ID3DXFont* > ( sesui::style.control_font.data )->Release ( );
		sesui::style.control_font.data = nullptr;
	}

	if ( font.data )
		return;

	D3DXCreateFontW ( device, static_cast< int > ( static_cast< float >( sesui::style.control_font.size )* sesui::globals::dpi ), 0, sesui::style.control_font.weight, 0, sesui::style.control_font.italic, OEM_CHARSET, OUT_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, sesui::style.control_font.family.get ( ), reinterpret_cast< ID3DXFont** > ( &sesui::style.control_font.data ) );
}

void render::polygon ( const std::vector< sesui::vec2 >& verticies, const sesui::color& color, bool filled ) noexcept {
	vertex_t* vtx = new vertex_t [ filled ? verticies.size ( ) : ( verticies.size ( ) + 1 ) ];

	for ( auto i = 0; i < verticies.size ( ); i++ ) {
		vtx[i].x = verticies [ i ].x;
		vtx[i].y = verticies [ i ].y;
		vtx [ i ].color = D3DCOLOR_RGBA ( static_cast< int >( color.r * 255.0f ), static_cast< int >( color.g * 255.0f ), static_cast< int >( color.b * 255.0f ), static_cast< int >( color.a * 255.0f ) );
		vtx[i].z = 0.0f;
		vtx[i].rhw = 1.0f;
	}

	if ( !filled ) {
		vtx[ verticies.size ( ) ] = vtx [ 0 ];
	}

	device->SetRenderState ( D3DRS_ALPHABLENDENABLE, true );
	device->SetFVF ( D3DFVF_XYZRHW | D3DFVF_DIFFUSE );
	device->SetTexture ( 0, nullptr );
	device->DrawPrimitiveUP ( filled ? D3DPT_TRIANGLEFAN : D3DPT_LINESTRIP, filled ? verticies.size ( ) - 2 : verticies.size ( ), vtx, sizeof( vertex_t ) );

	delete [ ] vtx;
}

void render::multicolor_polygon ( const std::vector< sesui::vec2 >& verticies, const std::vector< sesui::color >& colors, bool filled ) noexcept {
	vertex_t* vtx = new vertex_t [ filled ? verticies.size ( ) : ( verticies.size ( ) + 1 ) ];

	for ( auto i = 0; i < verticies.size ( ); i++ ) {
		vtx [ i ].x = verticies [ i ].x;
		vtx [ i ].y = verticies [ i ].y;
		vtx [ i ].color = D3DCOLOR_RGBA ( static_cast< int >( colors [ i ].r * 255.0f ), static_cast< int >( colors [ i ].g * 255.0f ), static_cast< int >( colors [ i ].b * 255.0f ), static_cast< int >( colors [ i ].a * 255.0f ) );
		vtx [ i ].z = 0.0f;
		vtx [ i ].rhw = 1.0f;
	}

	if ( !filled ) {
		vtx [ verticies.size ( ) ] = vtx [ 0 ];
	}

	device->SetRenderState ( D3DRS_ALPHABLENDENABLE, true );
	device->SetFVF ( D3DFVF_XYZRHW | D3DFVF_DIFFUSE );
	device->SetTexture ( 0, nullptr );
	device->DrawPrimitiveUP ( filled ? D3DPT_TRIANGLEFAN : D3DPT_LINESTRIP, filled ? verticies.size ( ) - 2 : verticies.size ( ), vtx, sizeof ( vertex_t ) );

	delete [ ] vtx;
}

void render::text ( const sesui::vec2& pos, const sesui::font& font, const sesui::ses_string& text, const sesui::color& color ) noexcept {
	if ( !font.data )
		return;

	RECT rect;
	SetRect ( &rect, pos.x, pos.y, pos.x, pos.y );
	reinterpret_cast< ID3DXFont* >( font.data )->DrawTextW ( nullptr, text.get ( ), text.len ( ), &rect, DT_LEFT | DT_NOCLIP, D3DCOLOR_RGBA ( static_cast< int >( color.r * 255.0f ), static_cast< int >( color.g * 255.0f ), static_cast< int >( color.b * 255.0f ), static_cast< int >( color.a * 255.0f ) ) );
}

void render::get_text_size ( const sesui::font& font, const sesui::ses_string& text, sesui::vec2& dim_out ) noexcept {
	RECT rect = { 0, 0, 0, 0 };
	reinterpret_cast< ID3DXFont* >( font.data )->DrawTextW ( nullptr, text.get ( ), text.len ( ), &rect, DT_CALCRECT, 0 );
	dim_out = { rect.right - rect.left, rect.bottom - rect.top };
}

float render::get_frametime ( ) noexcept {
	return frametime;
}

void render::begin_clip ( const sesui::rect& region ) noexcept {
	RECT rect { region.x, region.y, region.x + region.w, region.y + region.h };

	device->SetRenderState ( D3DRS_SCISSORTESTENABLE, true );
	device->SetScissorRect ( &rect );
}

void render::end_clip ( ) noexcept {
	device->SetRenderState ( D3DRS_SCISSORTESTENABLE, false );
}