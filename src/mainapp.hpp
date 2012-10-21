#pragma once
#include <SFML/Graphics.hpp>
#include "player.hpp"
#include "sequencer.hpp"
#include "room.hpp"
#include "util.hpp"
#include <memory>
#include <vector>

namespace hallow
{
	class MainApp
	{
	public:
		MainApp(const sf::VideoMode& mode, const std::string& gametitle, const std::string& firstroom);
		~MainApp();
		void exec();
	private:
		void poll_events();
		void handle_event(const sf::Event& e);
		void update(sf::Uint32 dt);
		void draw();

		bool room_transitioning_state() const;
		void set_room_transitioning_state(bool state);

		std::shared_ptr<ScopedSequenceSeqItem> transient_drawable_sequence(const std::shared_ptr<sf::Drawable>& drawable);

		void set_player_position_to_index(size_t index);

		sf::RenderWindow window_;
		sf::View view_;

		bool room_transitioning_;
		Room room_;

		Player player_;

		Sequencer transition_sequencer_;

		std::vector<std::shared_ptr<sf::Drawable>> overlay_transients_;
	};
} // end namespace hallow
