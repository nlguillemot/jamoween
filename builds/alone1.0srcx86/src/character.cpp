#include "character.hpp"

namespace alone
{
	Character::Character(AnimData& anim_data)
		: anim_data_(anim_data),
		animation_(anim_data_)
	{
	}
	void Character::set_position(const sf::Vector2f& p)
	{
		animation_.set_position(p);
	}
	sf::Vector2f Character::position() const
	{
		return animation_.position();
	}
	Animation& Character::animation()
	{
		return animation_;
	}
	const Animation& Character::animation() const
	{
		return animation_;
	}
	void Character::update(sf::Uint32 dt)
	{
		animation_.update(dt);
	}
	bool Character::handle_event(const sf::Event& e)
	{
		return false;
	}
	void Character::draw(sf::RenderTarget& target)
	{
		animation_.draw(target);
	}
}
