#include "mainapp.hpp"
#include <SFML/Graphics.hpp>
#include <string>
#include <fstream>
#include <cassert>
#include <sstream>
#include <iostream>

int main()
{
	std::string gametitle;
	sf::Vector2i screensize;
	std::string firstroom;

	std::ifstream ifs("assets/config.dat");

	if (!ifs)
	{
		std::cout << "Could not find config.csv" << std::endl;
		return -1;
	}
	
	for (;;)
	{
		std::string current_line;
		std::getline(ifs,current_line);

		if (!current_line.empty())
		{
			std::istringstream line_stream(current_line);
			std::string property_name;

			line_stream >> property_name;

			if (property_name == "gametitle")
			{
				line_stream >> gametitle;
			}
			else if (property_name == "screensize")
			{
				line_stream >> screensize.x;
				line_stream >> screensize.y;
			}
			else if (property_name == "firstroom")
			{
				line_stream >> firstroom;
			}
		}
		if (ifs.eof())
		{
			break;
		}
		else if (ifs.bad())
		{
			assert(false);
			break;
		}
	}

	ifs.close();

	hallow::MainApp app(sf::VideoMode(screensize.x,screensize.y), gametitle, firstroom);
	app.exec();
}
