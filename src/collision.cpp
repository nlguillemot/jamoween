#include "collision.hpp"

namespace hallow
{
	sf::Vector2f CollisionResolver::resolve_inside_bounds(const sf::FloatRect& within, const sf::FloatRect& bounds)
	{
		sf::Vector2f resolved_position(within.Left,within.Top);
		if(within.Left < bounds.Left)
		{
			resolved_position.x = bounds.Left;
		}
		if(within.Left + within.GetWidth() > bounds.Left + bounds.GetWidth())
		{
			resolved_position.x = bounds.Left + bounds.GetWidth() - within.GetWidth();
		}
		if(within.Top < bounds.Top)
		{
			resolved_position.y = bounds.Top;
		}
		if(within.Top + within.GetHeight() > bounds.Top + bounds.GetHeight())
		{
			resolved_position.y = bounds.Top + bounds.GetHeight() - within.GetHeight();
		}
		return resolved_position;
	}
}
