#pragma once
#include <SFML/Graphics.hpp>
#include "animation.hpp"
#include "animdata.hpp"
#include "sequencer.hpp"
#include <utility>

namespace hallow
{
	class Player
	{
	public:
		Player();

		void update(sf::Uint32 dt);
		void handle_event(const sf::Event& e);
		void draw(sf::RenderTarget& target);

		// used to position the player initially. ffffff
		const sf::Vector2f& position() const;
		void set_position(const sf::Vector2f& position);

		sf::Vector2f center_position() const;
		void set_center_position(const sf::Vector2f& pos);

		const sf::Vector2f& velocity() const;
		void set_velocity(const sf::Vector2f& vel);

		sf::FloatRect bounds() const;
		sf::FloatRect bounds_relative() const;
		
		void close_passage();
		void set_passage(const sf::FloatRect& r, size_t index);
		// rectangle of passage and its index
		const std::pair<sf::FloatRect,size_t>* maybe_current_passage() const;

		bool transitioning_state() const;
		void set_transitioning_state(bool state);

		bool locked_input() const;
		void set_locked_input(bool state);
	private:
		void init();

		sf::Uint32 time_accumulator_;
		sf::Vector2f position_;
		sf::Vector2f velocity_;
		AnimData animdata_;
		Animation animation_;

		std::pair<sf::FloatRect,size_t>* current_passage_;
		std::pair<sf::FloatRect,size_t> current_passage_data_;

		bool locked_input_;
		bool transitioning_;
		Sequencer action_sequencer_;
	};
}
