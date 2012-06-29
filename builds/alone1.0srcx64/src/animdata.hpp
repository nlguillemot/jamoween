#pragma once
#include <string>
#include <vector>
#include <map>
#include <SFML/Graphics.hpp>

namespace alone
{
	class AnimSequence
	{
	public:
		AnimSequence(const std::string& sequence_name, int start_frame, int end_frame)
			: name_(sequence_name), start_(start_frame), end_(end_frame) { }
		std::string name() const { return name_; }
		int start() const { return start_; }
		int end() const { return end_; }
	private:
		std::string name_;
		int start_;
		int end_;
	};

	class AnimData
	{
	public:
		AnimData(const std::string& file_name);
		sf::Texture& texture();
		int fps() const;
		int total_frames() const;
		const AnimSequence& sequence(const std::string& name) const;
		const sf::Vector2i& point(const std::string& name) const;
		const sf::IntRect& rect(const std::string& name) const;
		float constant(const std::string& name) const;
	private:
		void load_texture(const std::string& file_name);
		void load_properties(const std::string& file_name);
		void handle_property(std::string prop, std::istream& property_stream);
		sf::Texture texture_;
		int fps_;
		int total_frames_;
		std::vector<AnimSequence> sequences_;
		std::map<std::string,sf::Vector2i> points_;
		std::map<std::string,sf::IntRect> rects_;
		std::map<std::string,float> constants_;
	};
}
