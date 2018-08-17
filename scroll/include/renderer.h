#ifndef HELLO_H
#define HELLO_H

#include <iostream>
#include <string>
#include <cereal/archives/json.hpp>

namespace scl
{
	class Renderer
	{
	public:
		Renderer();
		~Renderer();

		void init();

	private:
		struct data_window
		{
			int _width;
			int _height;
			bool _fullscreen;

			template <class Archive> void serialize(Archive & ar)
			{
				ar(_width, _height, _fullscreen);
			}
		} _data_window;

		struct data_font
		{
			std::string _name;
			int _width;
			int _height;
			int _col;
			int _row;

			template <class Archive> void serialize(Archive & ar)
			{
				ar(_name, _width, _height, _col, _row);
			}
		} _data_font;
	};
}

#endif