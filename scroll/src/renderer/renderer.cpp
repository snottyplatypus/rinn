#include "renderer.h"
#include <libtcod.hpp>
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

	TCODConsole::setCustomFont(font_path.c_str(), TCOD_FONT_LAYOUT_ASCII_INROW, _data_font._col, _data_font._row);
	TCODConsole::initRoot(_data_window._width / _data_font._width, _data_window._height / _data_font._height, "Rinn");
	TCODConsole::root->setFullscreen(_data_window._fullscreen);
	TCODSystem::setFps(60);
}