#include "renderer.h"
#include <fstream>

scl::Renderer::Renderer()
{
}

scl::Renderer::~Renderer()
{
}

void scl::Renderer::init()
{
	std::ifstream file("../data/config/window.json");
	cereal::JSONInputArchive archive(file);
	archive(cereal::make_nvp("window", _data_window));
	archive(cereal::make_nvp("font", _data_font));
	std::string font_path = "../data/art/" + _data_font._name;

	InitWindow(_data_window._width, _data_window._height, "Rinn");
	load_font(font_path);
	if (_data_window._fullscreen)
		ToggleBorderlessWindowed();
	ClearBackground(BLACK);
	SetTargetFPS(60);
}

void scl::Renderer::drawFont(char c, float x, float y, Color color = WHITE) const
{
	int charIndex = (int)c;
	int srcX = (charIndex % _data_font._col) * _data_font._width;
	int srcY = (charIndex / _data_font._row) * _data_font._height;
	Rectangle source = { srcX, srcY, _data_font._width, _data_font._height };
	Rectangle dest = { x, y, _data_font._width, _data_font._height };
	DrawTexturePro(_font, source, dest, { 0, 0 }, 0.0f, color);
}

void scl::Renderer::drawFont(std::string str, float x, float y, Color color = WHITE) const
{
	for (size_t i = 0; i < str.size(); ++i)
		drawFont(str[i], x + i * _data_font._width, y, color);
}

void scl::Renderer::load_font(const std::string& path)
{
	Image image = LoadImage(path.c_str());
	_font = LoadTextureFromImage(image);
}