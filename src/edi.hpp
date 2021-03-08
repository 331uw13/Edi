#pragma once

#include <GL/glew.h>
#include <SDL2/SDL.h>
#include "text_edit.hpp"


class Edi {
public:

	Edi(const char* t_filename);
	~Edi();

	void start();
	bool seems_fine();

private:


	Settings m_settings;
	TextEdit m_text_edit;

	void _cleanup_and_quit();
	void _configure_imgui();
	
	bool m_problem_occurred { false };
	SDL_Window* m_window { nullptr };
	SDL_GLContext m_context { NULL };

};


