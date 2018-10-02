#pragma once

#include "Mode.hpp"

#include "MeshBuffer.hpp"
#include "GL.hpp"

#include <SDL.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <vector>
#include <string>
// The 'GameMode' mode is the main gameplay mode:

struct GameMode : public Mode {
	GameMode();
	virtual ~GameMode();

	//handle_event is called when new mouse or keyboard events are received:
	// (note that this might be many times per frame or never)
	//The function should return 'true' if it handled the event.
	virtual bool handle_event(SDL_Event const &evt, glm::uvec2 const &window_size) override;

	//update is called at the start of a new frame, after events are handled:
	virtual void update(float elapsed) override;

	//draw is called after update:
	virtual void draw(glm::uvec2 const &drawable_size) override;
	// void counter();
	void light_cubes(int idx);
	float camera_spin = 1.0f;
	float spot_spin = 1.0f;
	struct {
		bool blue = false;
		bool red = false;
		bool yellow = false;
		bool green = false;
		bool machine_play = false;
		bool reset = false;
		bool start_play = false;
		bool continue_play = false;
	} controls;
	bool done_sequence = false;
	bool win = false;
	std::vector<int> play_seq;
	std::vector<int> play_seq_cpy;
	enum Color { blue, red, green, yellow };
	std::string color_string[4] = {"blue", "red", "green", "yellow"};
};
