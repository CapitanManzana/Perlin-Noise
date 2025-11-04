//
// Tecnología de la Programación de Videojuegos 1
// Facultad de Informática UCM
//
// Plantilla de proyecto con SDL
//

#include <iostream>

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include "PerlinNoise.h"
#include <algorithm>

using namespace std;

using uint = unsigned int; // alias para el tipo unsigned int

// Dimensiones de la ventana
constexpr uint WIN_WIDTH = 800;
constexpr uint WIN_HEIGHT = 600;

void firstTest()
{
	SDL_Window* window = nullptr;
	SDL_Renderer* renderer = nullptr;
	PerlinNoise noise;

	SDL_Init(SDL_INIT_VIDEO); // activa SDL (añadir SDL_INIT_AUDIO, etc. si procede)

	// Crea la ventana y el renderizador
	if (!SDL_CreateWindowAndRenderer("First test with SDL",
		WIN_WIDTH, WIN_HEIGHT, 0,
		&window, &renderer))
		cout << "Error cargando SDL: " << SDL_GetError() << endl;
	else {
		// Fondo negro
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);

		for (int x = 0; x < WIN_WIDTH; x++) {
			for (int y = 0; y < WIN_HEIGHT; y++) {

				float scale = 0.02f; // ajusta este valor para hacer el ruido más visible
				float n = noise.fractalNoise(x * scale, y * scale, 3, 0.5f, 2.0f, 1.0f, 1.0f);
				float color = (n + 1.0f) * 0.5f * 255.0f;
				color = clamp(color, 0.0f, 255.0f);

				SDL_SetRenderDrawColor(renderer, color, color, color, 255);
				SDL_RenderPoint(renderer, x, y);
			}
		}
		SDL_RenderPresent(renderer);

		// Espera 5 segundos
		SDL_Delay(5000);
	}

	// Libera los recursos y desactiva SDL
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

int main(int argc, char* argv[])
{
	firstTest();
	return 0;
}
