#include "mainapp.hpp"
#include <SFML/Window/Event.hpp>
#include "collision.hpp"
#include <cassert>
#include <iostream>

namespace alone
{
	void MainApp::init()
	{
		shadow_time_accumulator_ = 0;
		end_game_ = false;

		window.Create(sf::VideoMode(320,240),"Alone");
		view = window.GetDefaultView();

		icon_.LoadFromFile("assets/icon.png");
		window.SetIcon(icon_.GetWidth(), icon_.GetHeight(), icon_.GetPixelsPtr());
		
		despair_music_.OpenFromFile("assets/despair.ogg");
		despair_music_.SetLoop(true);
		despair_music_.Play();

		background_data_ = new AnimData("assets/street");
		background_ = new Animation(*background_data_);
		
		overlay_data_ = new AnimData("assets/overlay_white");
		overlay_ = new Animation(*overlay_data_);
		overlay_->set_scale(0.5,0.5);
		overlay_->set_tint(sf::Color::Black);

		player_data_ = new AnimData("assets/player");
		player_ = new Player(*player_data_);
		player_->set_play_area(background_->rect_relative("arenabounds"));
		sf::Vector2i spawn_pos = background_->point_relative("playerspawn");
		player_->set_position(sf::Vector2f(spawn_pos.x, spawn_pos.y));
		add_depth_sorted_character(player_);

		flash_data_ = new AnimData("assets/flash");
		flash_ = new Animation(*flash_data_);
		
		player_->set_transitioning(true);
		flash_->set_alpha(255);
		flash_->set_tint(sf::Color::Black);
		flash_sequencer_.append(std::make_shared<WaitSeqItem>(2000));
		flash_sequencer_.append(std::make_shared<FadeAnimSeqItem>(*flash_,true,1000));
		flash_sequencer_.append(std::make_shared<FunctionSeqItem>(std::bind(&Player::set_transitioning,player_,false)));
		flash_sequencer_.append(std::make_shared<FunctionSeqItem>(std::bind(&Animation::set_tint,flash_,sf::Color::White)));
	}
	void MainApp::exec()
	{
		window.SetFramerateLimit(60);
		while(window.IsOpened())
		{
			poll_events();
			update(1000 * window.GetFrameTime());
			draw();
		}
	}
	void MainApp::poll_events()
	{
		sf::Event e;
		while(window.GetEvent(e))
		{
			handle_event(e);
		}	
	}
	void MainApp::handle_event(const sf::Event& e)
	{
		switch(e.Type)
		{
		case sf::Event::Closed:
			window.Close();
			break;
		default:
			break;
		}
		player_->handle_event(e);
	}
	void MainApp::update(sf::Uint32 dt)
	{
		flash_sequencer_.update(dt);

		// Add shadows if necessary
		shadow_time_accumulator_ += dt;
		if( ( shadow_time_accumulator_ >= 1000 ) || (end_game_ && shadow_time_accumulator_ >= 100) )
		{
			shadow_time_accumulator_ = 0;
			Shadow::Direction dir = (rand()&1) ? Shadow::Left : Shadow::Right;
			sf::Vector2f spawnpos;
			if(dir == Shadow::Left)
			{
				spawnpos.x = static_cast<float>(background_->width() + 100);
			}
			else
			{
				spawnpos.x = static_cast<float>(-100);
			}
			sf::IntRect arenabounds(background_->rect("arenabounds"));
			int horizon = arenabounds.Top; 
			spawnpos.y = static_cast<float>(horizon + rand()%(background_->height() - horizon - player_data_->texture().GetHeight()));
			shadow_list_.push_back(std::make_shared<Shadow>(*player_data_,dir,spawnpos));
			shadow_list_.back()->set_player_target(player_);
			if(end_game_)
			{
				shadow_list_.back()->bring_to_life();
			}
			add_depth_sorted_character(shadow_list_.back().get());
		}

		player_->update(dt);

		if(!end_game_ && player_->rescued() >= 15)
		{
			end_game_ = true;
			for(auto it = shadow_list_.begin(); it != shadow_list_.end();++it)
			{
				(*it)->bring_to_life();
			}
			flash_sequencer_.append(std::make_shared<FadeAnimSeqItem>(*flash_,false,10000));
			flash_sequencer_.append(std::make_shared<FunctionSeqItem>(std::bind(&MainApp::set_transitioning,this,true)));
			flash_sequencer_.append(std::make_shared<FadeMusicSeqItem>(bell_music_,false,2000,true));
			flash_sequencer_.append(std::make_shared<FunctionSeqItem>(std::bind(&sf::Music::Stop,&bell_music_)));
			flash_sequencer_.append(std::make_shared<WaitSeqItem>(5000));
			flash_sequencer_.append(std::make_shared<FunctionSeqItem>(std::bind(&sf::RenderWindow::Close,&window)));
		}

		for(auto it = shadow_list_.begin(); it != shadow_list_.end();)
		{
			(*it)->update(dt);
			if((*it)->queued_for_removal())
			{
				auto current_node = it;
				it++;
				remove_depth_sorted_character(current_node->get());
				shadow_list_.erase(current_node);
			}
			else
			{
				if((*it)->started_vanishing())
				{
					if(!player_->awake())
					{
						overlay_->set_scale(overlay_->scale() + sf::Vector2f(0.05f,0.05f));
						if(overlay_->scale().x * overlay_->constant("radius") >= window.GetWidth()/2)
						{
							player_->wake_up();
							bell_music_.OpenFromFile("assets/bells.ogg");
							bell_music_.SetLoop(true);

							const std::string message = "You are not alone.";
							std::vector<std::string> messages;
							for(unsigned i = 0; i < message.size(); ++i)
							{
								messages.push_back(message.substr(0,i));
							}
							messages.push_back("");

							flash_sequencer_.append(std::make_shared<FunctionSeqItem>(std::bind(&MainApp::set_transitioning,this,true)));
							flash_sequencer_.append(std::make_shared<FunctionSeqItem>(std::bind(&Animation::set_alpha,flash_,255)));
							flash_sequencer_.append(std::make_shared<FadeMusicSeqItem>(despair_music_,false,2000,true));
							flash_sequencer_.append(std::make_shared<StringChangeSeqItem>(title_string_, messages, 600));
							flash_sequencer_.append(std::make_shared<FadeMusicSeqItem>(bell_music_,true,2000,true));
							flash_sequencer_.append(std::make_shared<FunctionSeqItem>(std::bind(&MainApp::set_transitioning,this,false)));
							flash_sequencer_.append(std::make_shared<FadeAnimSeqItem>(*flash_,true,2000));
							overlay_->set_tint(sf::Color::White);
						}
					}
				}
				++it;
			}
		}
		
		sf::Vector2f playercenter = player_->animation().center_relative();
		sf::Vector2f camtopleft = playercenter - view.GetHalfSize();
		sf::FloatRect camrect(camtopleft.x, camtopleft.y, camtopleft.x + view.GetRect().GetWidth(), camtopleft.y + view.GetRect().GetHeight());
		sf::IntRect animrect = background_->anim_rect_relative();
		camtopleft = CollisionResolver::resolve_inside_bounds(camrect,sf::FloatRect(animrect.Left, animrect.Top, animrect.Right, animrect.Bottom));

		view.SetCenter(camtopleft + sf::Vector2f(static_cast<float>(window.GetWidth()/2),static_cast<float>(window.GetHeight()/2)));
		window.SetView(view);

		if(!player_->awake())
		{
			overlay_->set_position(playercenter - sf::Vector2f(overlay_->width()/2.0f,overlay_->height()/2.0f));
		}
		else
		{
			overlay_->set_position(view.GetCenter() - sf::Vector2f(overlay_->width()/2.0f,overlay_->height()/2.0f));
		}
		flash_->set_position(view.GetCenter() -  sf::Vector2f(flash_->width()/2.0f,flash_->height()/2.0f));
	}
	void MainApp::draw()
	{
		window.Clear(sf::Color::Black);

		background_->draw(window);

		sort_characters_by_depth();
		for(auto it = depth_sorted_characters_.begin(); it != depth_sorted_characters_.end(); ++it)
		{
			(*it)->draw(window);
		}

		overlay_->draw(window);

		flash_->draw(window);

		window.Draw(title_string_);

		window.Display();
	}
	void MainApp::add_depth_sorted_character(Character* character)
	{
		depth_sorted_characters_.push_back(character);
	}
	void MainApp::remove_depth_sorted_character(Character* character)
	{
		auto it = std::find(depth_sorted_characters_.begin(),depth_sorted_characters_.end(),character);
		assert(it != depth_sorted_characters_.end());
		depth_sorted_characters_.erase(it);
	}
	void MainApp::sort_characters_by_depth()
	{
		struct functor
		{
			static bool is_depth_deeper(Character* lhs, Character* rhs)
			{
				return lhs->position().y < rhs->position().y;
			}
		};

		depth_sorted_characters_.sort(functor::is_depth_deeper);
	}
	bool MainApp::transitioning() const
	{
		return player_->transitioning();
	}
	void MainApp::set_transitioning(bool state)
	{
		player_->set_transitioning(state);
	}
	void MainApp::exit()
	{
		delete player_;
		delete player_data_;

		delete background_;
		delete background_data_;

		delete overlay_;
		delete overlay_data_;

		delete flash_;
		delete flash_data_;
	}

	FadeAnimSeqItem::FadeAnimSeqItem(Animation& animation, bool fade_out, sf::Uint32 duration)
		: animation_(animation),
		fade_out_(fade_out),
		duration_(duration)
	{
		if(fade_out_)
		{
			current_alpha_ = 255.0f;
		}
		else
		{
			current_alpha_ = 0.0f;
		}
	}
	void FadeAnimSeqItem::init()
	{
		animation_.set_alpha(static_cast<sf::Uint8>(current_alpha_));
	}
	void FadeAnimSeqItem::update(sf::Uint32 dt)
	{
		const float delta_alpha = 256 * (static_cast<float>(dt) / duration_);
		if(fade_out_)
		{
			current_alpha_ -= delta_alpha;
		}
		else
		{
			current_alpha_ += delta_alpha;
		}
		if( (fade_out_ && current_alpha_ <= -1) || (!fade_out_ && current_alpha_ >= 256) )
		{
			if(fade_out_)
			{
				animation_.set_alpha(0);
			}
			else
			{
				animation_.set_alpha(255);
			}
			set_complete(true);
			return;
		}
		else
		{
			animation_.set_alpha(static_cast<sf::Uint8>(current_alpha_));
		}
	}

	WaitSeqItem::WaitSeqItem(sf::Uint32 time)
		: current_time_(0),
		time_limit_(time)
	{
	}
	void WaitSeqItem::update(sf::Uint32 dt)
	{
		current_time_ += dt;
		if(current_time_ >= time_limit_)
		{
			set_complete(true);
		}
	}

	FunctionSeqItem::FunctionSeqItem(const std::function<void ()>& function)
		: function_(function)
	{
	}
	void FunctionSeqItem::init()
	{
		function_();
		set_complete(true);
	}

	FadeMusicSeqItem::FadeMusicSeqItem(sf::Music& music, bool fade_in, sf::Uint32 duration, bool make_start_or_stop)
		: music_(music),
		fade_in_(fade_in),
		duration_(duration),
		make_start_or_stop_(make_start_or_stop)
	{
	}
	void FadeMusicSeqItem::init()
	{
		if(fade_in_)
		{
			current_volume_ = 0.0f;
			if(!(music_.GetStatus() == sf::Sound::Playing))
			{
				music_.SetVolume(current_volume_);
				if(make_start_or_stop_)
				{
					music_.Play();
				}
			}
		}
		else
		{
			current_volume_ = 100.0f;
		}
	}
	void FadeMusicSeqItem::update(sf::Uint32 dt)
	{
		const float delta_volume = 100 * (static_cast<float>(dt) / duration_);
		if(fade_in_)
		{
			current_volume_ += delta_volume;
			if(current_volume_ >= 100.0f)
			{
				current_volume_ = 100.0f;
				set_complete(true);
			}
		}
		else
		{
			current_volume_ -= delta_volume;
			if(current_volume_ <= 0.0f)
			{
				current_volume_ = 0;
				if(make_start_or_stop_)
				{
					music_.Stop();
				}
				set_complete(true);
			}
		}
		music_.SetVolume(current_volume_);
	}

	StringChangeSeqItem::StringChangeSeqItem(sf::String& str, const std::vector<std::string>& textlist, sf::Uint32 time_between_texts)
		: str_(str),
		textlist_(textlist),
		time_between_texts_(time_between_texts),
		current_text_(-1), // will move to 0 when the first text is displayed
		time_accumulator_(time_between_texts) // will display first text immediately
	{
	}
	void StringChangeSeqItem::update(sf::Uint32 dt)
	{
		time_accumulator_ += dt;
		if(time_accumulator_ >= time_between_texts_)
		{
			time_accumulator_ = 0;
			++current_text_;
			if(current_text_ >= static_cast<int>(textlist_.size()))
			{
				set_complete(true);
			}
			else
			{
				str_.SetText(textlist_[current_text_]);
			}
		}
	}
}
