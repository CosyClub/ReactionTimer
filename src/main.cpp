////////////////////////////////////////////////////////////////////////////
/// \file main.cpp
/// \author Jamie Terry
/// \date 2017/10/12
/// \brief Contains entry point for ReactionTimer
////////////////////////////////////////////////////////////////////////////


#include <cstdio>
#include <cmath>
#include <cstdlib>
#include <string>

#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#define ArrayLength(array) (sizeof(array) / sizeof((array)[0]))

const float PRACTICE_DURATION = 7.0f;
const float STAGE_DURATION    = 15.0f;

float stage_bpms[] = {
	60,
	90,
	120,
	150,
};

int bpm; //beats per minute
float beat_delta;

sf::SoundBuffer tick_buffer;
sf::Sound tick;
sf::Font font;

char text_bmp_buffer[32];
sf::Text text_bpm;
sf::Text text_practice;

bool is_practice; // if we're currently in practice mode
int  past_beats;  // number of beats that have occured in this stage

bool loadResources(){
	if(!tick_buffer.loadFromFile("tick.ogg")){
		printf("Failed to load tick.ogg\n");
		return false;
	}
	tick.setBuffer(tick_buffer);

	if(!font.loadFromFile("calibri.ttf")){
		printf("Failed to load calibri.tff");
		return false;
	}

	return true;
}

void updateStage(int stage){
	is_practice = true;
	past_beats = 0;
	bpm = stage_bpms[stage];
	beat_delta = 60.0f / bpm;

	snprintf(text_bmp_buffer, 32, "BPM: %i", bpm);

	text_bpm = sf::Text(text_bmp_buffer, font, 100);
	text_bpm.setFillColor(sf::Color(0,0,255));
	text_bpm.setPosition(30, 80);

	text_practice = sf::Text("Practice", font, 100);
	text_practice.setFillColor(sf::Color(0,0,255));
	text_practice.setPosition(30, 180);
}


int main(int argc, const char** argv){
	if(!loadResources()){ return 1; }


	sf::RenderWindow app(sf::VideoMode(640, 400, 32),
	                     std::string("Cosy Club Reaction Timer")
	                     );
	sf::Event event;

	int stage = 0;
	updateStage(stage);

	sf::Clock clock;
	float last_beat = clock.getElapsedTime().asSeconds();
	float stage_start = last_beat;

	while(app.isOpen()){
		float now = clock.getElapsedTime().asSeconds();

		//////////////////////////////////////////
		// Update stage if enough beats have occured
		if(is_practice){
			if(now - stage_start > PRACTICE_DURATION){
				stage_start = now;
				is_practice = false;
				past_beats  = 0;
			}
		} else {
			if(now - stage_start > STAGE_DURATION){
				++stage;
				if(stage >= ArrayLength(stage_bpms)){
					app.close();
					break;
				}
				stage_start = now;
				updateStage(stage);
			}
		}

		//////////////////////////////////////////
		// Handle user input
		while(app.pollEvent(event)){
			switch(event.type){
			case sf::Event::Closed:
				app.close();
				break;
			default: break;
			}
		}

		//////////////////////////////////////////
		// Render Display
		float color_factor = pow(2, (last_beat - now)*5.0f);
		app.clear(sf::Color((unsigned char)(255 * color_factor * !is_practice),
		                    (unsigned char)(255 * color_factor *  is_practice),
		                    0)
		          );
		app.draw(text_bpm);
		if(is_practice){
			app.draw(text_practice);
		}
		// Rendering code here
		app.display();

		//////////////////////////////////////////
		// Update beat stats and play if required
		if(now - last_beat >= beat_delta){
			printf("Tick\n");
			tick.play();
			last_beat = now;
			++past_beats;
		}

	} // end of main loop
	printf("Main loop ended\n");
	return 0;
}
