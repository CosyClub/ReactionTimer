////////////////////////////////////////////////////////////////////////////
/// \file main.cpp
/// \author Jamie Terry
/// \date 2017/10/12
/// \brief Contains entry point for ReactionTimer
////////////////////////////////////////////////////////////////////////////

#include <cstdio>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

int main(int argc, const char** argv){
	sf::Clock clock;

	while(clock.getElapsedTime().asSeconds() < 2.5){
		printf("Launching window in: %6.3fs\n", 2.5f - clock.getElapsedTime().asSeconds());
	}

	// :TODO: If you're using pure OpenGl just use a sf::Window
	sf::RenderWindow app(sf::VideoMode(640, 400, 32), "ReactionTimer");

	sf::Event event;
	int frame_counter = 0;
	while(app.isOpen()){

		while(app.pollEvent(event)){
			switch(event.type){
			case sf::Event::Closed:
				app.close();
				break;
			default: break;
			}
		}

		// :TODO: if you're using a pure OpenGl need to do glClear instead
		app.clear();
		// Rendering code here
		app.display();

		++frame_counter;
		printf("Done frame: %i\n", frame_counter);
	} // end of main loop
	printf("Main loop ended\n");
	return 0;
}
