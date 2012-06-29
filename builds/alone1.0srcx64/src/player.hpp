#pragma once
#include "character.hpp"

namespace alone
{
	class Player : public Character
	{
	public:
		Player(AnimData& anim_data);

		bool handle_event(const sf::Event& e);
		void update(sf::Uint32 dt);

		void stop_movement();
		sf::Vector2f velocity() const;

		void set_play_area(const sf::IntRect& rect);

		sf::IntRect collision_rect() const;

		bool awake() const;
		void wake_up();

		void add_rescue();
		int rescued() const;

		bool transitioning() const;
		void set_transitioning(bool state);
	private:
		void set_vp(const sf::Vector2f& vp);
		void set_vpx(float vpx);
		void set_vpy(float vpy);
		void set_vn(const sf::Vector2f& vn);
		void set_vnx(float vnx);
		void set_vny(float vny);
		sf::Vector2f vn() const;
		sf::Vector2f vp() const;

		// These vectors decompose opposing movement velocities to make them possible to combine properly
		// These are made to handle key presses. Need to add more vectors for other forces.
		// covers -x and -y magnitudes
		sf::Vector2f velocity_negative_;
		// covers +x and +y magnitudes
		sf::Vector2f velocity_positive_;

		// player not allowed to exit this rectangle
		sf::IntRect play_area_;

		bool woke_up_;
		int num_rescued_;

		bool transitioning_;
	};
}
