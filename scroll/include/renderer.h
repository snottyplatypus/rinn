#ifndef RENDERER_H
#define RENDERER_H

#include <iostream>
#include <string>
#include <map>
#include <unordered_map>
#include <raylib.h>
#include <cereal/archives/json.hpp>
#include <cereal/types/map.hpp>

namespace scl
{
	class Renderer
	{
	public:
		Renderer();
		~Renderer();

		void init();
		void load_sprite_textures();
		void drawFont(char c, float x, float y, float scale = 1.0f, float rotation = 0.0f, Color color = WHITE) const;
		void drawFont(std::string str, float x, float y, float scale = 1.0f, float rotation = 0.0f, Color color = WHITE) const;
		void drawSprite(const std::string& name, float x, float y, float scale = 1.0f, float rotation = 0.0f, Color color = WHITE) const;

		struct data_window
		{
			float _width;
			float _height;
			bool _fullscreen;

			template <class Archive> void serialize(Archive & ar)
			{
				ar(_width, _height, _fullscreen);
			}
		} _window;

		struct data_font
		{
			std::string _name;
			float _width;
			float _height;
			int _col;
			int _row;

			template <class Archive> void serialize(Archive& ar)
			{
				ar(_name, _width, _height, _col, _row);
			}
		} _data_font;

	private:
		Texture2D _font;
		void load_font(const std::string& path);

		struct sprite_data {
			std::string _filename;
			float _width;
			float _height;

			template <class Archive> void serialize(Archive & ar)
			{
				ar(_filename, _width, _height);
			}
		};

		class Sprite {
		public:
			Sprite() = default;
			Sprite(const sprite_data& data) : _data(data) {}
			~Sprite() { unload(); }

			void load() {
				if (!_texture.id) {
					std::string sprite_path = "../data/art/" + _data._filename;
					Image image = LoadImage(sprite_path.c_str());
					_texture = LoadTextureFromImage(image);
					UnloadImage(image);
				}
			}

			void unload() {
				if (_texture.id) {
					UnloadTexture(_texture);
					_texture = Texture2D{};
				}
			}

			const Texture2D& texture() const { return _texture; }
			const sprite_data& data() const { return _data; }

		private:
			sprite_data _data;
			Texture2D _texture{};
		};

		std::map<std::string, sprite_data> _sprites_data;
		std::map<std::string, Sprite> _sprites;
	};
}

#endif