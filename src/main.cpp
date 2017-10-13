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

// Value used to represent a time delta for a beat which was missed
const float MISSED_BEAT_DELTA = -10000.0f;

const float PRACTICE_DURATION = 7.0f;
const float STAGE_DURATION    = 15.0f;

/// \brief The beats per minute to test in each stage
//const int STAGE_BPMS[] = {
//	60,
	//	90,
	//	120,
	//	150,
	//};
const int STAGE_BPMS[] = {
	90,
};
#define STAGE_COUNT ArrayLength(STAGE_BPMS)

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

/// \brief Represents statistics for a given stage
struct StageStats {
	int bpm;           /// \brief beats per minute of this stage

	int    beat_count; /// \brief number of beats that occured in the stage
	float* beat_times; /// \brief array of times at which a beat occured

	int    key_count;  /// \brief number of times a key was pressed during the stage
	float* key_times;  /// \brief array of times at which a key was pressed
};
StageStats stats[STAGE_COUNT];

/// \brief Loads resources (sounds, fonts) required by
/// the app and allocates memory
bool init(){
	if(!tick_buffer.loadFromFile("tick.ogg")){
		printf("Failed to load tick.ogg\n");
		return false;
	}
	tick.setBuffer(tick_buffer);

	if(!font.loadFromFile("calibri.ttf")){
		printf("Failed to load calibri.tff");
		return false;
	}

	for(unsigned int i = 0; i < STAGE_COUNT; ++i){
		stats[i]     = {0};
		stats[i].bpm = STAGE_BPMS[i];

		// there will be STAGE_DURATION * beats per second beats in the stage
		// + 2 to be safe
		stats[i].beat_times = (float*)malloc((STAGE_DURATION * ((STAGE_BPMS[i] / 60.0)) + 2) * sizeof(float));

		// lets assume player can only press a key 5 times a second
		stats[i].key_times = (float*)malloc(STAGE_DURATION * 5 * sizeof(float));
	}

	return true;
}

void shutdown(){
	for(unsigned int i = 0; i < STAGE_COUNT; ++i){
		free(stats[i].beat_times);
		free(stats[i].key_times);
	}
}

/// \brief Proceeds to the next testing stage
void updateStage(int stage){
	printf("Begin stage %i/%lu\n", stage+1, STAGE_COUNT);

	is_practice = true;
	past_beats = 0;
	bpm = STAGE_BPMS[stage];
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
	if(!init()){ return 1; }

	sf::RenderWindow app(sf::VideoMode(640, 400, 32),
	                     std::string("Cosy Club Reaction Timer")
	                     );
	sf::Event event;

	unsigned int stage = 0;
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
			}
		} else {
			if(now - stage_start > STAGE_DURATION){
				++stage;
				if(stage >= STAGE_COUNT){
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
			case sf::Event::KeyPressed:
				if(!is_practice){
					stats[stage].key_times[stats[stage].key_count] = now;
					++stats[stage].key_count;
				}
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
			if(!is_practice){
				stats[stage].beat_times[stats[stage].beat_count] = now;
				++stats[stage].beat_count;
			}
		}

	} // end of main loop

	printf("Main loop ended, processing and outputting results...\n");

	for(unsigned int i = 0; i < STAGE_COUNT; ++i){
		StageStats* s = &stats[i];

		/*printf("Stage %i\n", i+1);
		printf("  BPM        : %i\n", s->bpm);
		printf("  Key Count  : %i\n", s->key_count);
		printf("  Beat Count : %i\n", s->beat_count);

		printf("-----beats-----\n");
		for(int j = 0; j < s->beat_count; ++j){
			printf("%f\n", s->beat_times[j]);
		}

		printf("-----keys-----\n");
		for(int j = 0; j < s->key_count; ++j){
			printf("%f\n", s->key_times[j]);
			}*/

		int* key_to_beat = (int*)malloc(s->key_count * sizeof(int));

		//////////////////////////////////
		// Find the closest beat to each key press
		int beat = 1;
		for(int key = 0; key < s->key_count; ++key){
			while(beat < s->beat_count-1 && s->beat_times[beat] < s->key_times[key]){
				++beat;
			}

			float diff_0 = fabs(s->beat_times[beat-1] - s->key_times[key]);
			float diff_1 = fabs(s->beat_times[beat  ] - s->key_times[key]);
			float diff_2 = fabs(s->beat_times[beat+1] - s->key_times[key]);

			if(diff_0 <= diff_1 && diff_0 <= diff_2){
				key_to_beat[key] = beat-1;
			} else if(diff_1 <= diff_0 && diff_1 <= diff_2){
				key_to_beat[key] = beat  ;
			} else if(diff_2 <= diff_0 && diff_2 <= diff_1){
				key_to_beat[key] = beat+1;
			} else {
				printf("ERROR: CANT LINE UP KEY WITH BEAT\n");
			}
			//printf("key_to_beat[%i]: %i\n", key, key_to_beat[key]);
		}

		if(s->key_count > s->beat_count &&                     // to many key presses AND
		   key_to_beat[s->key_count-1] == s->beat_count - 1 && // last key was for last beat AND
		   key_to_beat[s->key_count-1] == s->beat_count - 1){    // penultamate key was for last beat
			// then the user pressed a key after the last beat in time for the next, we don't want
			// to penalise for this as user doesn't know when the last beat is going to be and
			// wont react in time to not press a key, so just ignore the last key press
			--s->key_count;
		}

		float* deltas = (float*)malloc(s->beat_count * sizeof(float));
		int last_beat    = -1;
		int hit_beats    =  0;
		int missed_beats =  0;
		int extra_keys   =  0;

		for(int key = 0; key < s->key_count; ++key){
			float new_delta = s->beat_times[key_to_beat[key]] - s->key_times[key];
			int this_beat = key_to_beat[key];
			if(this_beat != last_beat){

				// Skip past any missed beats
				for(int b = last_beat+1; b < this_beat; ++b){
					++missed_beats;
					deltas[b] = MISSED_BEAT_DELTA;
				}

				++hit_beats;
				deltas[this_beat] = new_delta;
				last_beat = this_beat;

			} else {
				// Then two key presses near a beat :(
				if(fabs(new_delta) < fabs(deltas[key])){
					deltas[this_beat] = new_delta;
				}
				++extra_keys;
			}
		}

		float avg_delta =     0;
		float max_delta = -1000;
		float min_delta =  1000;
		printf("BPM: %i, Missed: %i, Hit: %i, Extra Keys: %i\n",
		       s->bpm, missed_beats, hit_beats, extra_keys);
		for(int i = 0; i < s->beat_count; ++i){
			//printf("%f\n", deltas[i]);
			if(deltas[i] <= MISSED_BEAT_DELTA){ continue; }
			float a = fabs(deltas[i]);
			if(a > max_delta){ max_delta = a; }
			if(a < min_delta){ min_delta = a; }
			avg_delta += deltas[i];
		}
		avg_delta /= hit_beats;
		printf("  Deltas, min: %6.4f, avg: %6.4f, max: %6.4f\n",
		       min_delta, avg_delta, max_delta
		       );

		free(key_to_beat);
		free(deltas);
	}

	printf("Done, cleaning up...\n");
	shutdown();

	printf("Exiting\n");
	return 0;
}
