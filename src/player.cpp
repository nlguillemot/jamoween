#include "player.hpp"
#include "util.hpp"
#include <cmath>
#include <iostream>

namespace hallow
{
	class GoIntoDoorSeqItem : public SequencerItem
	{
	public:
		GoIntoDoorSeqItem(Player& player, const sf::Vector2f& doorcenter):
		player_(player),
		target_(doorcenter),
		time_limit_(1000),
		time_accumulator_(0)
		{
		}
		void update(sf::Uint32 dt)
		{
			time_accumulator_ += dt;
			if (time_accumulator_ > time_limit_)
			{
				set_complete(true);
			}
			else
			{
				sf::Vector2f lerped = vector_LERP(player_.center_position(), target_, 0.05);
				player_.set_center_position(lerped);
			}
		}
		void exit()
		{
			player_.set_center_position(target_);
		}
	private:
		Player& player_;
		sf::Vector2f target_;
		sf::Uint32 time_limit_;
		sf::Uint32 time_accumulator_;
	};

	Player::Player():
	animdata_("assets/ethereal"),
	animation_(animdata_),
	current_passage_(0),
	locked_input_(false),
	transitioning_(false)
	{
		init();
	}
	void Player::init()
	{
		animation_.play("idle");
		animation_.set_scale(3.f,3.f);
	}
	void Player::update(sf::Uint32 dt)
	{
		action_sequencer_.update(dt);

		time_accumulator_ += dt;
		position_ += (velocity_ * static_cast<float>((dt/1000.0)));

		float hover_period = 1500.f;

		sf::Vector2f animpos = position_;
		animpos.y += animation_.height()/4.f * sin(time_accumulator_/hover_period * 3.14);

		animation_.set_position(animpos);
		animation_.update(dt);
	}
	void Player::handle_event(const sf::Event& e)
	{
		sf::Key::Code key_left = sf::Key::Left;
		sf::Key::Code key_right = sf::Key::Right;
		sf::Key::Code key_up = sf::Key::Up;
		const float walkspeed = 175.f;

		// short circuit
		if (transitioning_state() || locked_input())
		{
			return;
		}

		if (e.Type == sf::Event::KeyPressed)
		{
			if (e.Key.Code == key_left)
			{
				velocity_.x = -walkspeed;
			}
			else if (e.Key.Code == key_right)
			{
				velocity_.x = walkspeed;
			}
			else if (e.Key.Code == key_up)
			{
				if (current_passage_)
				{
					set_transitioning_state(true);
					
					action_sequencer_.append(
						std::make_shared<FunctionSeqItem>(std::bind(&Player::set_velocity, this, sf::Vector2f(0,0))));
					action_sequencer_.append(
						std::make_shared<GoIntoDoorSeqItem>(*this, rect_center(current_passage_->first)));
					action_sequencer_.append(
						std::make_shared<FunctionSeqItem>(std::bind(&Player::set_transitioning_state, this, false)));
				}
			}
		}
		else if (e.Type == sf::Event::KeyReleased)
		{
			if (e.Key.Code == key_left && velocity_.x < 0)
			{
				velocity_.x = 0.f;
			}
			else if (e.Key.Code == key_right && velocity_.x > 0)
			{
				velocity_.x = 0.f;
			}
		}
	}
	void Player::draw(sf::RenderTarget& target)
	{
		animation_.draw(target);
	}
	const sf::Vector2f& Player::position() const
	{
		return position_;
	}
	void Player::set_position(const sf::Vector2f& position)
	{
		position_ = position;
	}
	sf::Vector2f Player::center_position() const
	{
		return position() + (rect_size(bounds())/2.f);
	}
	void Player::set_center_position(const sf::Vector2f& pos)
	{
		set_position(pos - (rect_size(bounds())/2.f));
	}
	const sf::Vector2f& Player::velocity() const
	{
		return velocity_;
	}
	void Player::set_velocity(const sf::Vector2f& vel)
	{
		velocity_ = vel;
	}
	sf::FloatRect Player::bounds() const
	{
		return animation_.anim_rect();
	}
	sf::FloatRect Player::bounds_relative() const
	{
		return animation_.anim_rect_relative();
	}
	void Player::close_passage()
	{
		current_passage_ = 0;
	}
	void Player::set_passage(const sf::FloatRect& r, size_t index)
	{
		current_passage_data_ = std::pair<sf::FloatRect,size_t>(r,index);
		current_passage_ = &current_passage_data_;
	}
	const std::pair<sf::FloatRect,size_t>* Player::maybe_current_passage() const
	{
		return current_passage_;
	}
	bool Player::transitioning_state() const
	{
		return transitioning_;
	}
	void Player::set_transitioning_state(bool state)
	{
		transitioning_ = state;
	}
	bool Player::locked_input() const
	{
		return locked_input_;
	}
	void Player::set_locked_input(bool state)
	{
		locked_input_ = state;
	}
}
