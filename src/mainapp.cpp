#include "mainapp.hpp"
#include <SFML/Window/Event.hpp>
#include "collision.hpp"
#include "util.hpp"
#include "room.hpp"
#include <cassert>
#include <iostream>

namespace hallow
{
	MainApp::MainApp():
	window_(sf::VideoMode(320,240),"Hallow"),
	room_("redroom"),
	room_transitioning_(false)
	{
		window_.Create(sf::VideoMode(320,240),"Hallow");
		view_ = window_.GetDefaultView();
		player_.set_position(room_.player_spawn_point());
	}
	MainApp::~MainApp()
	{
	}
	void MainApp::exec()
	{
		window_.SetFramerateLimit(60);
		while(window_.IsOpened())
		{
			poll_events();
			update(1000 * window_.GetFrameTime());
			draw();
		}
	}
	void MainApp::poll_events()
	{
		sf::Event e;
		while(window_.GetEvent(e))
		{
			handle_event(e);
		}	
	}
	void MainApp::handle_event(const sf::Event& e)
	{
		switch(e.Type)
		{
		case sf::Event::Closed:
			window_.Close();
			break;
		default:
			break;
		}

		player_.handle_event(e);
	}
	void MainApp::update(sf::Uint32 dt)
	{
		player_.update(dt);
		transition_sequencer_.update(dt);

		if (!player_.transitioning_state() && !room_transitioning_state())
		{
			size_t window_index;
			const sf::FloatRect* wrect = room_.maybe_collide(player_.bounds_relative(), &window_index);
			if (wrect)
			{
				player_.set_passage(*wrect,window_index);
			}
			else
			{
				player_.close_passage();
			}
		}

		if (player_.transitioning_state() && !room_transitioning_state()) {
			room_transitioning_ = true;

			auto fade_black = std::make_shared<sf::Shape>(sf::Shape::Rectangle(0,0,window_.GetWidth(), window_.GetHeight(),sf::Color::White));
			fade_black->SetColor(sf::Color(255,255,255,0));
			auto scope = transient_drawable_sequence(fade_black);

			scope->append(std::make_shared<FunctionSeqItem>(
				std::bind(&Player::set_locked_input,&player_,true)));

			scope->append(std::make_shared<DrawableColorTweenSeqItem>(
				sf::Color(0,0,0,0), sf::Color::Black, 2000, *fade_black));

			scope->append(std::make_shared<WaitUntilSeqItem>(
				not_function(std::bind(&Player::transitioning_state,&player_))));

			auto next_room_door_index = std::make_shared<size_t>();
			scope->append(std::make_shared<FunctionSeqItem>(
				std::bind(
					[next_room_door_index](const std::function<size_t ()>& index_fn)
					{
						*next_room_door_index = index_fn();
					},
					[this]
					{
						size_t retval;
						return *(this->room_.output_index_of_window(
							this->player_.maybe_current_passage()->second,&retval));
							
					})));

			scope->append(std::make_shared<FunctionSeqItem>(
				curry<void,const std::string>(std::bind(&Room::switch_to_room, &room_, std::placeholders::_1),
					curry<const std::string,size_t>(std::bind(&Room::room_behind_passage, &room_, std::placeholders::_1),
					std::bind(
						[](Player* p)
						{
							return p->maybe_current_passage()->second;
						}, &player_)))));

			scope->append(std::make_shared<FunctionSeqItem>(
				curry<void,size_t>(
					std::bind(&MainApp::set_player_position_to_index,this,std::placeholders::_1),
					[next_room_door_index]{ return *next_room_door_index; })));
				

			scope->append(std::make_shared<DrawableColorTweenSeqItem>(
				sf::Color::Black, sf::Color(0,0,0,0), 1000, *fade_black));

			scope->append(std::make_shared<FunctionSeqItem>(
				std::bind(&MainApp::set_room_transitioning_state,this,false)));

			scope->append(std::make_shared<FunctionSeqItem>(
				std::bind(&Player::set_locked_input,&player_,false)));

			transition_sequencer_.append(scope);
		}

		// I hate this block of code. a lot. compact it sometime.
		sf::Vector2f playercenter = player_.position() + rect_size(player_.bounds())/2.f;
		sf::Vector2f camtopleft = playercenter - view_.GetHalfSize();
		sf::FloatRect camrect(camtopleft.x, camtopleft.y, camtopleft.x + view_.GetRect().GetWidth(), camtopleft.y + view_.GetRect().GetHeight());
		sf::FloatRect background_animrect = room_.relative_bounds();
		camtopleft = CollisionResolver::resolve_inside_bounds(camrect, sf::FloatRect(background_animrect.Left, background_animrect.Top, background_animrect.Right, background_animrect.Bottom));

		view_.SetCenter(camtopleft + sf::Vector2f(static_cast<float>(window_.GetWidth()/2), static_cast<float>(window_.GetHeight()/2)));
		window_.SetView(view_);
	}
	void MainApp::draw()
	{
		window_.Clear(sf::Color::Black);

		room_.draw(window_);
		player_.draw(window_);

		for (size_t i = 0; i < overlay_transients_.size(); i++)
		{
			overlay_transients_[i]->SetPosition(rect_topleft(view_.GetRect()));
			window_.Draw(*overlay_transients_[i]);
		}

		window_.Display();
	}
	bool MainApp::room_transitioning_state() const
	{
		return room_transitioning_;
	}
	void MainApp::set_room_transitioning_state(bool state)
	{
		room_transitioning_ = state;
	}
	std::shared_ptr<ScopedSequenceSeqItem> MainApp::transient_drawable_sequence(const std::shared_ptr<sf::Drawable>& drawable)
	{
		auto scope =
			std::make_shared<ScopedSequenceSeqItem>(
				std::bind(
					[drawable](std::vector<std::shared_ptr<sf::Drawable>>* transient_list)
					{ 
						transient_list->push_back(drawable);
					},&overlay_transients_)
				,std::bind(
					[drawable](std::vector<std::shared_ptr<sf::Drawable>>* transient_list)
					{
						transient_list->erase(std::find(transient_list->begin(), transient_list->end(), drawable));
					},&overlay_transients_));
		return scope;
	}
	void MainApp::set_player_position_to_index(size_t index)
	{
		sf::Vector2f new_player_pos = 	
			rect_center(*room_.maybe_passage_by_index(index));

		player_.set_center_position(new_player_pos);
	}
}
