#include "room.hpp"
#include <sstream>
#include <iostream>

namespace hallow
{
	Room::Room(const std::string& room_name):
	background_data_(0),
	background_(0)
	{
		switch_to_room(room_name);
	}
	Room::~Room()
	{
		delete background_;
		delete background_data_;
	}
	void Room::switch_to_room(const std::string& room_name)
	{
		delete background_;
		delete background_data_;

		background_data_ = new AnimData("assets/" + room_name);
		background_ = new Animation(*background_data_);

		passages_.clear();
		for (size_t i = 0; ; i++)
		{
			std::stringstream ss;
			ss << "window" << i;
			const sf::FloatRect* r = background_data_->maybe_rect(ss.str());
			if (!r)
			{
				break;
			}
			else
			{
				passages_.push_back(*r);
			}
		}
	}
	void Room::draw(sf::RenderTarget& target)
	{
		if (background_)
		{
			background_->draw(target);
		}
	}
	sf::FloatRect Room::bounds() const
	{	
		return background_->anim_rect();
	}
	sf::FloatRect Room::relative_bounds() const
	{
		return background_->anim_rect_relative();
	}
	sf::Vector2f Room::player_spawn_point() const
	{
		return background_->point_relative("playerspawn");
	}
	std::string Room::room_behind_passage(size_t passage_index)
	{
		if (passage_index >= passages_.size())
		{
			return "";
		}
		else
		{
			std::stringstream ss;
			ss << "pathof" << passage_index;
			const std::string* roomname = background_data_->maybe_string(ss.str());
			if (!roomname)
			{
				return "";
			}
			else
			{
				return *roomname;
			}
		}
	}
	const sf::FloatRect* Room::maybe_collide(const sf::FloatRect& player_rect, size_t* window_index)
	{
		for (size_t i = 0; i < passages_.size(); i++)
		{
			if (player_rect.Intersects(passages_[i]))
			{
				if (window_index)
				{
					*window_index = i;
				}
				return &passages_[i];
			}
		}
		return 0;
	}
	const sf::FloatRect* Room::maybe_passage_by_index(size_t window_index)
	{
		if (window_index >= passages_.size())
		{
			return 0;
		}
		else
		{
			return &passages_[window_index];
		}
	}
	size_t* Room::output_index_of_window(size_t window_index, size_t* retval) const
	{
		std::stringstream ss;
		ss << "doorof" << window_index;
		const float* c = background_data_->maybe_constant(ss.str());
		if (c)
		{
			*retval = static_cast<size_t>(*c);
			return retval;
		}
		else
		{
			return 0;
		}
	}
}
