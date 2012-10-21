#pragma once
#include "animation.hpp"
#include "animdata.hpp"
#include <string>
#include <vector>

namespace hallow
{
	class Room
	{
	public:
		Room(const std::string& room_name);
		~Room();
		void switch_to_room(const std::string& room_name);
		void draw(sf::RenderTarget& target);

		sf::FloatRect bounds() const;
		sf::FloatRect relative_bounds() const;
		sf::Vector2f player_spawn_point() const;

		// returns "" if passage does not exist
		std::string room_behind_passage(size_t passage_index);

		const sf::FloatRect* maybe_collide(const sf::FloatRect& player_rect, size_t* window_index=0);
		const sf::FloatRect* maybe_passage_by_index(size_t window_index);

		// retval stores value if it exists and NULL is returned otherwise
		size_t* output_index_of_window(size_t window_index, size_t* retval) const;
	private:
		AnimData* background_data_;
		Animation* background_;
		std::vector<sf::FloatRect> passages_;
	};
}
