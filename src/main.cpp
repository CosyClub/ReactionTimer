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

int main(int argc, const char** argv){
	sf::SoundBuffer tick_buffer;
	if(!tick_buffer.loadFromFile("tick.ogg")){
		printf("Failed to load tick.ogg\n");
		return 1;
	}
	sf::Sound tick;
	tick.setBuffer(tick_buffer);

	int beats_per_minute = 150;
	float beat_delta = 60.0 / beats_per_minute;


	sf::Clock clock;

	sf::RenderWindow app(sf::VideoMode(640, 400, 32),
	                     std::string("Cosy Club Reaction Timer")
	                     );
	sf::Event event;
	float last_beat = clock.getElapsedTime().asSeconds();
	while(app.isOpen()){
		float now = clock.getElapsedTime().asSeconds();

		while(app.pollEvent(event)){
			switch(event.type){
			case sf::Event::Closed:
				app.close();
				break;
			default: break;
			}
		}

		printf("%f\n", now);

		// :TODO: if you're using a pure OpenGl need to do glClear instead
		app.clear(sf::Color((unsigned char)(255 * pow(2, (last_beat - now)*5.0f)),0,0));
		// Rendering code here
		app.display();


		if(now - last_beat >= beat_delta){
			printf("Tick\n");
			tick.play();
			last_beat = now;
		}

	} // end of main loop
	printf("Main loop ended\n");
	return 0;
}
