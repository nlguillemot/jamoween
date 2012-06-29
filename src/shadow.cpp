#include "shadow.hpp"
#include <cassert>

namespace alone
{
	Shadow::Shadow(AnimData& anim_data, Shadow::Direction direction, const sf::Vector2f& startpos)
		: Character(anim_data),
		queued_for_removal_(false),
		direction_(direction),
		player_(0),
		started_vanishing_(false),
		vanishing_(false)
	{
		// pixels per second
		const int base_movement_speed = 25;
		const int extra_movement_speed = 100;
		float base_velocity = static_cast<float>(base_movement_speed + rand() % extra_movement_speed );
		if(direction_ == Right)
		{
			animation().fliph(true);
			velocity_.x = base_velocity;
		}
		else
		{
			velocity_.x = -base_velocity;
		}
		animation().set_position(startpos);
		animation().play("walk");
		animation().set_tint(sf::Color::Black);
		animation().set_fps(animation().fps()/2);
	}
	void Shadow::update(sf::Uint32 dt)
	{
		if(player_)
		{
			if(vanishing_)
			{
				started_vanishing_ = false;
				vanish_sequencer_.update(dt);
			}
			else if(!player_->transitioning())
			{
				if(player_->animation().anim_rect_relative().Intersects(animation().anim_rect_relative()))
				{
					if(!player_->awake())
					{
						started_vanishing_ = true;
						vanishing_ = true;
						vanish_sequencer_.append(std::make_shared<ShadowVanishSeqItem>(*this));
						vanish_sequencer_.append(std::make_shared<ShadowRemoveSelfFromShadowListSeqItem>(shared_from_this()));
					}
					else
					{
						bring_to_life();
					}
				}
			}
		}
		sf::Vector2f new_pos(animation().position());
		new_pos.x += dt * velocity_.x / 1000.0f;
		animation().set_position(new_pos);
		animation().update(dt);
	}
	void Shadow::draw(sf::RenderTarget& target)
	{
		if(!queued_for_removal())
		{
			if(direction_ == Left)
			{
				if(position().x < -animation().width())
				{
					set_queued_for_removal(true);
				}
			}
			else
			{
				// Assumes the world is twice the screen size
				if(position().x > target.GetWidth()*2 + animation().width())
				{
					set_queued_for_removal(true);
				}
			}
		}

		animation().draw(target);
	}
	void Shadow::set_player_target(Player* player)
	{
		player_ = player;
	}
	bool Shadow::queued_for_removal() const
	{
		return queued_for_removal_;
	}
	void Shadow::set_queued_for_removal(bool state)
	{
		queued_for_removal_ = state;
	}
	void Shadow::bring_to_life()
	{
		if(!vanishing_ && !player_->transitioning())
		{
			started_vanishing_ = true;
			vanishing_ = true;
			player_->add_rescue();
			vanish_sequencer_.append(std::make_shared<ShadowTweenToLifeSeqItem>(*this));
		}
	}
	bool Shadow::started_vanishing() const
	{
		return started_vanishing_;
	}

	ShadowVanishSeqItem::ShadowVanishSeqItem(Shadow& shadow)
		: shadow_(shadow)
	{
	}
	void ShadowVanishSeqItem::init()
	{
		current_alpha_ = static_cast<float>(shadow_.animation().alpha());
	}
	void ShadowVanishSeqItem::update(sf::Uint32 dt)
	{
		const float alpha_per_second = 255.0f;
		current_alpha_ -= alpha_per_second * dt / 1000.0f;
		if(current_alpha_ <= -1)
		{
			set_complete(true);
			return;
		}
		else
		{
			shadow_.animation().set_alpha(static_cast<sf::Uint8>(current_alpha_));
		}
	}
	
	ShadowRemoveSelfFromShadowListSeqItem::ShadowRemoveSelfFromShadowListSeqItem(std::shared_ptr<Shadow> shadow)
		: shadow_(shadow)
	{
	}
	void ShadowRemoveSelfFromShadowListSeqItem::init()
	{
		shadow_->set_queued_for_removal(true);
	}

	ShadowTweenToLifeSeqItem::ShadowTweenToLifeSeqItem(Shadow& shadow)
		: shadow_(shadow)
	{
	}
	void ShadowTweenToLifeSeqItem::init()
	{
		current_whiteness_ = 0.0f;
	}
	void ShadowTweenToLifeSeqItem::update(sf::Uint32 dt)
	{
		const float whiteness_per_second = 255.0f;
		current_whiteness_ += whiteness_per_second * dt / 1000.0f;
		if(current_whiteness_ >= 256.0f)
		{
			set_complete(true);
			return;
		}
		else
		{
			shadow_.animation().set_tint(sf::Color(static_cast<sf::Uint8>(current_whiteness_),static_cast<sf::Uint8>(current_whiteness_),static_cast<sf::Uint8>(current_whiteness_),255));
		}
	}
}
