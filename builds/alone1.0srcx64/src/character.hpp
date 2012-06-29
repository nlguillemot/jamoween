#pragma once
#include "animation.hpp"
#include "animdata.hpp"
#include <string>

namespace alone
{
	class Character
	{
	public:
		Character(AnimData& anim_data);
		
		virtual void set_position(const sf::Vector2f& p);
		virtual sf::Vector2f position() const;
		
		Animation& animation();
		const Animation& animation() const;

		virtual void update(sf::Uint32 dt);
		virtual bool handle_event(const sf::Event& e);
		virtual void draw(sf::RenderTarget& target);
	private:
		AnimData& anim_data_;
	protected:
		Animation animation_;
	};
}
