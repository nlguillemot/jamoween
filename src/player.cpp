#include "player.hpp"
#include "collision.hpp"
#include <cassert>
#include <iostream>

namespace alone
{
	Player::Player(AnimData& anim_data)
		: Character(anim_data),
		woke_up_(false),
		num_rescued_(0),
		transitioning_(false)
	{
		animation().hold("idle");
	}
	bool Player::handle_event(const sf::Event& e)
	{
		// pixels per second
		const float velocity_magnitude = 100;
		using namespace sf;

		if(e.Type == sf::Event::KeyPressed)
		{
			switch(e.Key.Code)
			{
			case Key::Left:
				set_vnx(-velocity_magnitude);
				break;
			case Key::Right:
				set_vpx(velocity_magnitude);
				break;
			case Key::Up:
				set_vny(-velocity_magnitude);
				break;
			case Key::Down:
				set_vpy(velocity_magnitude);
				break;
			default:
				break;
			}
			return true;
		}
		else if(e.Type == sf::Event::KeyReleased)
		{
			switch(e.Key.Code)
			{
			case Key::Left:
				if(vn().x < 0) set_vnx(0);
				break;
			case Key::Right:
				if(vp().x > 0) set_vpx(0);
				break;
			case Key::Up:
				if(vn().y < 0) set_vny(0);
				break;
			case Key::Down:
				if(vp().y > 0) set_vpy(0);
				break;
			default:
				break;
			}
			return true;
		}
		else if(e.Type == sf::Event::LostFocus)
		{
			stop_movement();
			return true;
		}
		return false;
	}
	void Player::update(sf::Uint32 dt)
	{
		sf::Vector2f vel(velocity());
		if(vel != sf::Vector2f(0,0))
		{
			if(!animation().playing())
			{
				animation().play("walk");
			}
			sf::Vector2f pos = animation().position();
			sf::Vector2f previous_pos = pos;
			pos.x += vel.x * dt / 1000;
			pos.y += vel.y * dt / 1000;
			
			// restrict movement to play area
			sf::IntRect feetRect = animation().rect("feetrect");
			sf::FloatRect absfeetrect(feetRect.Left, feetRect.Top, feetRect.Right, feetRect.Bottom);
			sf::FloatRect feetrect(pos.x + absfeetrect.Left, pos.y + absfeetrect.Top,absfeetrect.GetWidth(),absfeetrect.GetHeight());

			pos = CollisionResolver::resolve_inside_bounds(feetrect,sf::FloatRect(play_area_.Left, play_area_.Top, play_area_.Right, play_area_.Bottom));
			pos -= sf::Vector2f(absfeetrect.Left,absfeetrect.Top);
			
			animation().set_position(pos);

			if(vel.x < 0)
			{
				animation().fliph(false);
			}
			else if(vel.x > 0)
			{
				animation().fliph(true);
			}
		}
		else
		{
			if(animation().playing())
			{
				animation().hold("idle");
			}
		}
		animation().update(dt);
	}
	void Player::stop_movement()
	{
		set_vp(sf::Vector2f(0,0));
		set_vn(sf::Vector2f(0,0));
	}
	sf::Vector2f Player::velocity() const
	{
		return vn() + vp();
	}
	void Player::set_play_area(const sf::IntRect& rect)
	{
		play_area_ = rect;
	}
	sf::IntRect Player::collision_rect() const
	{
		return animation().rect_relative("feetrect");
	}
	bool Player::awake() const
	{
		return woke_up_;
	}
	void Player::wake_up()
	{
		woke_up_ = true;
	}
	void Player::add_rescue()
	{
		++num_rescued_;
	}
	int Player::rescued() const
	{
		return num_rescued_;
	}
	bool Player::transitioning() const
	{
		return transitioning_;
	}
	void Player::set_transitioning(bool state)
	{
		transitioning_ = state;
	}
	void Player::set_vp(const sf::Vector2f& vp)
	{
		velocity_positive_ = vp;
	}
	void Player::set_vpx(float vpx)
	{
		set_vp(sf::Vector2f(vpx,vp().y));
	}
	void Player::set_vpy(float vpy)
	{
		set_vp(sf::Vector2f(vp().x,vpy));
	}
	void Player::set_vn(const sf::Vector2f& vn)
	{
		velocity_negative_ = vn;
	}
	void Player::set_vnx(float vnx)
	{
		set_vn(sf::Vector2f(vnx,vn().y));
	}
	void Player::set_vny(float vny)
	{
		set_vn(sf::Vector2f(vn().x,vny));
	}
	sf::Vector2f Player::vn() const
	{
		return velocity_negative_;
	}
	sf::Vector2f Player::vp() const
	{
		return velocity_positive_;
	}
}
