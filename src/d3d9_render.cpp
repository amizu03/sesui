#include "d3d9_render.hpp"

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

void render::polygon ( const std::vector< sesui::vec2 >& verticies, const sesui::color& color, bool filled ) noexcept {
	vertex_t* vtx = new vertex_t [ filled ? verticies.size ( ) : ( verticies.size ( ) + 1 ) ];

	for ( auto i = 0; i < verticies.size ( ); i++ ) {
		vtx[i].x = verticies [ i ].x - 0.5f;
		vtx[i].y = verticies [ i ].y - 0.5f;
		vtx [ i ].color = D3DCOLOR_RGBA ( color.r, color.g, color.b, color.a );
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

void render::text ( const sesui::vec2& pos, const sesui::font& font, const sesui::ses_string& text, const sesui::color& color ) noexcept {
	if ( !font.data )
		return;

	RECT rect;
	SetRect ( &rect, pos.x - 0.5f, pos.y - 0.5f, pos.x - 0.5f, pos.y - 0.5f );

	const auto as_str = text.get ( );

	if constexpr ( std::is_same< char, std::remove_pointer< decltype( as_str ) >::type >::value )
		reinterpret_cast< ID3DXFont* >( font.data )->DrawTextA ( nullptr, reinterpret_cast< const char* >( as_str ), text.len ( ), &rect, DT_LEFT | DT_NOCLIP, D3DCOLOR_RGBA ( color.r, color.g, color.b, color.a ) );
	else if constexpr ( std::is_same< wchar_t, std::remove_pointer< decltype( as_str ) >::type >::value )
		reinterpret_cast< ID3DXFont* >( font.data )->DrawTextW ( nullptr, reinterpret_cast< const wchar_t* >( as_str ), text.len ( ), &rect, DT_LEFT | DT_NOCLIP, D3DCOLOR_RGBA ( color.r, color.g, color.b, color.a ) );
}

void render::get_text_size ( const sesui::font& font, const sesui::ses_string& text, sesui::vec2& dim_out ) noexcept {
	RECT rect = { 0, 0, 0, 0 };

	const auto as_str = text.get ( );
	
	if constexpr ( std::is_same< char, std::remove_pointer< decltype( as_str ) >::type >::value )
		reinterpret_cast< ID3DXFont* >( font.data )->DrawTextA ( nullptr, reinterpret_cast< const char* >( as_str ), text.len ( ), &rect, DT_CALCRECT, 0 );
	else if constexpr ( std::is_same< wchar_t, std::remove_pointer< decltype( as_str ) >::type >::value )
		reinterpret_cast< ID3DXFont* >( font.data )->DrawTextW ( nullptr, reinterpret_cast< const wchar_t* >( as_str ), text.len ( ), &rect, DT_CALCRECT, 0 );

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