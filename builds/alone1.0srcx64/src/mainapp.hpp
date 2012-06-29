#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "player.hpp"
#include <list>
#include "shadow.hpp"
#include <functional>

namespace alone
{
	class MainApp
	{
	public:
		void init();
		void exec();
		void exit();
	private:
		void poll_events();
		void handle_event(const sf::Event& e);
		void update(sf::Uint32 dt);
		void draw();

		void add_depth_sorted_character(Character* character);
		void remove_depth_sorted_character(Character* character);
		void sort_characters_by_depth();

		bool transitioning() const;
		void set_transitioning(bool state);

		sf::RenderWindow window;
		sf::Image icon_;
		sf::View view;

		AnimData* background_data_;
		Animation* background_;

		AnimData* player_data_;
		Player* player_;

		AnimData* overlay_data_;
		Animation* overlay_;

		sf::Uint32 shadow_time_accumulator_;
		std::list<std::shared_ptr<Shadow>> shadow_list_;

		AnimData* flash_data_;
		Animation* flash_;
		Sequencer flash_sequencer_;

		std::list<Character*> depth_sorted_characters_;
		bool end_game_;

		sf::Music bell_music_;
		sf::Music despair_music_;
	};
	
	class FadeAnimSeqItem : public SequencerItem
	{
	public:
		FadeAnimSeqItem(Animation& animation, bool fade_out, sf::Uint32 duration);
		void init();
		void update(sf::Uint32 dt);
	private:
		Animation& animation_;
		bool fade_out_;
		sf::Uint32 duration_;
		float current_alpha_;
	};

	class WaitSeqItem : public SequencerItem
	{
	public:
		WaitSeqItem(sf::Uint32 time);
		void update(sf::Uint32 dt);
	private:
		sf::Uint32 current_time_;
		sf::Uint32 time_limit_;
	};

	class FunctionSeqItem : public SequencerItem
	{
	public:
		FunctionSeqItem(const std::function<void ()>& function);
		void init();
	private:
		std::function<void ()> function_;
	};

	class FadeMusicSeqItem : public SequencerItem
	{
	public:
		FadeMusicSeqItem(sf::Music& music, bool fade_in, sf::Uint32 duration, bool make_start_or_stop);
		void init();
		void update(sf::Uint32 dt);
	private:
		sf::Music& music_;
		bool fade_in_;
		sf::Uint32 duration_;
		bool make_start_or_stop_;
		float current_volume_;
	};

	class TitleTextChangeSeqItem : public SequencerItem
	{
	public:
		TitleTextChangeSeqItem(sf::Window& window, const std::vector<std::string>& textlist, sf::Uint32 time_between_texts);
		void update(sf::Uint32 dt);
	private:
		sf::Window& window_;
		const std::vector<std::string> textlist_;
		sf::Uint32 time_between_texts_;
		int current_text_;
		sf::Uint32 time_accumulator_;
	};
}
