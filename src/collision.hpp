#pragma once
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Rect.hpp>

namespace alone
{
	struct CollisionResolver
	{
		// for use when you want one rectangle to stay within another rectangle
		static sf::Vector2f resolve_inside_bounds(const sf::FloatRect& within, const sf::FloatRect& bounds);
	};
}
