#pragma once

#include "character.hpp"
#include "player.hpp"
#include "sequencer.hpp"
#include <list>
#include <memory>

namespace alone
{
	class Shadow : public Character, public std::enable_shared_from_this<Shadow>
	{
	public:
		enum Direction
		{
			Left,
			Right,
		};
		Shadow(AnimData& anim_data, Direction direction, const sf::Vector2f& startpos);
		void update(sf::Uint32 dt);
		void draw(sf::RenderTarget& target);
		void set_player_target(Player* player);
		
		bool queued_for_removal() const;
		void set_queued_for_removal(bool state);
		
		void bring_to_life();
		bool started_vanishing() const;
	private:
		bool allowed_to_vanish() const;

		bool queued_for_removal_;
		Direction direction_;
		Player* player_;
		bool started_vanishing_;
		bool vanishing_;
		sf::Vector2f velocity_;
		Sequencer vanish_sequencer_;
	};
	class ShadowVanishSeqItem : public SequencerItem
	{
	public:
		ShadowVanishSeqItem(Shadow& shadow);
		void init();
		void update(sf::Uint32 dt);
	private:
		Shadow& shadow_;
		float current_alpha_;
	};
	class ShadowRemoveSelfFromShadowListSeqItem : public SequencerItem
	{
	public:
		ShadowRemoveSelfFromShadowListSeqItem(std::shared_ptr<Shadow> shadow);
		void init();
	private:
		std::shared_ptr<Shadow> shadow_;
	};
	class ShadowTweenToLifeSeqItem : public SequencerItem
	{
	public:
		ShadowTweenToLifeSeqItem(Shadow& shadow);
		void init();
		void update(sf::Uint32 dt);
	private:
		Shadow& shadow_;
		float current_whiteness_;
	};
}
