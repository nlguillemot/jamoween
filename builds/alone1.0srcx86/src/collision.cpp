#include "collision.hpp"

namespace alone
{
	sf::Vector2f CollisionResolver::resolve_inside_bounds(const sf::FloatRect& within, const sf::FloatRect& bounds)
	{
		sf::Vector2f resolved_position(within.Left,within.Top);
		if(within.Left < bounds.Left)
		{
			resolved_position.x = bounds.Left;
		}
		if(within.Left + within.Width > bounds.Left + bounds.Width)
		{
			resolved_position.x = bounds.Left + bounds.Width - within.Width;
		}
		if(within.Top < bounds.Top)
		{
			resolved_position.y = bounds.Top;
		}
		if(within.Top + within.Height > bounds.Top + bounds.Height)
		{
			resolved_position.y = bounds.Top + bounds.Height - within.Height;
		}
		return resolved_position;
	}
}
