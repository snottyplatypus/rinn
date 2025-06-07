#include "renderer.h"
#include <fstream>

scl::Renderer::Renderer()
{
}

scl::Renderer::~Renderer()
{
	for (auto& [name, sprite] : _sprites) {
		sprite.unload();
	}
	_sprites.clear();
	_sprites_data.clear();

	if (_font.id) {
		UnloadTexture(_font);
		_font = Texture2D{};
	}
}

void scl::Renderer::init()
{
	std::ifstream file_w("../data/config/window.json");
	cereal::JSONInputArchive archive_w(file_w);
	archive_w(cereal::make_nvp("window", _window));

	std::ifstream file_a("../data/config/assets.json");
	cereal::JSONInputArchive archive_a(file_a);
	archive_a(cereal::make_nvp("font", _data_font));
	archive_a(cereal::make_nvp("sprites", _sprites_data));

	std::string font_path = "../data/art/" + _data_font._name;

	InitWindow(_window._width, _window._height, "Rinn");
	load_font(font_path);
	load_sprite_textures();

	if (_window._fullscreen)
		ToggleBorderlessWindowed();
	ClearBackground(BLACK);
	SetTargetFPS(60);
}

void scl::Renderer::load_sprite_textures()
{
	for (const auto& [name, data] : _sprites_data) {
		_sprites[name] = Sprite(data);
		_sprites[name].load();
	}
}

void scl::Renderer::drawFont(char c, float x, float y, float scale, float rotation, Color color) const
{
	int char_idx = (int)c;
	int srcX = (char_idx % _data_font._col) * _data_font._width;
	int srcY = (char_idx / _data_font._row) * _data_font._height;
	Rectangle source = { srcX, srcY, _data_font._width, _data_font._height };
	Rectangle dest = { x - (_data_font._width * scale) / 2, y - (_data_font._height * scale) / 2, _data_font._width * scale, _data_font._height * scale };
	Vector2 origin = { 0, 0 };
	DrawTexturePro(_font, source, dest, origin, rotation, color);
}

void scl::Renderer::drawFont(std::string str, float x, float y, float scale, float rotation, Color color) const
{	
	float start_x = x - (str.size() * _data_font._width * scale) / 2;
	for (size_t i = 0; i < str.size(); i++) {
		drawFont(str[i], start_x + (_data_font._width * scale * i) + (_data_font._width * scale) / 2, y, scale, rotation, color);
	}
}

void scl::Renderer::drawSprite(const std::string& name, float x, float y, float scale, float rotation, Color color) const
{
	auto it = _sprites.find(name);
	if (it != _sprites.end()) {
		const Sprite& sprite = it->second;
		const sprite_data& data = sprite.data();
		Rectangle source = { 0, 0, data._width, data._height };
		Rectangle dest = { x, y, data._width * scale, data._height * scale };
		Vector2 origin = { data._width * scale / 2, data._height * scale / 2 };
		DrawTexturePro(sprite.texture(), source, dest, origin, rotation, color);
	}
}

void scl::Renderer::load_font(const std::string& path)
{
	Image image = LoadImage(path.c_str());
	_font = LoadTextureFromImage(image);
	UnloadImage(image);
}