#include <cstdio>
#include "edi.hpp"


Edi::Edi(const char* t_filename) {
	if(SDL_Init(SDL_INIT_VIDEO) < 0) {
		m_problem_occurred = true;
		return;
	}	

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	m_window = SDL_CreateWindow("Edi!", 0, 0, 830, 600, 
			SDL_WINDOW_SHOWN |
		   	SDL_WINDOW_OPENGL);
			//SDL_WINDOW_RESIZABLE |
			//SDL_WINDOW_MAXIMIZED);	

	if(m_window == nullptr) {
		SDL_Quit();
		fprintf(stderr, "%s\n", SDL_GetError());
		assert(m_window == nullptr);
		m_problem_occurred = true;
		return;
	}

	m_context = SDL_GL_CreateContext(m_window);
	
	if(m_context == NULL) {
		SDL_DestroyWindow(m_window);
		SDL_Quit();
		fprintf(stderr, "%s\n", SDL_GetError());
		assert(m_context == NULL);
		m_problem_occurred = true;
		return;
	}

	SDL_GetWindowSize(m_window, &m_settings.window_width, &m_settings.window_height);
	glViewport(0, 0, m_settings.window_width, m_settings.window_height);

	if(glewInit() != GLEW_OK) {
		fprintf(stderr, "glewInit() Failed!\n");
		m_problem_occurred = true;
		return;
	}

	ImGui::CreateContext();
	ImGui_ImplSDL2_InitForOpenGL(m_window, m_context);
	ImGui_ImplOpenGL3_Init("#version 330");
	
	_configure_imgui();
	m_text_edit.init();
}

Edi::~Edi() {
	_cleanup_and_quit();
}

void Edi::start() {
	SDL_Event event;


	while(!m_settings.quit) {
		int start_tick = SDL_GetTicks();
		const float tpf = 1000.0f/m_settings.fps_limit; // ticks per frame

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplSDL2_NewFrame(m_window);
		ImGui::NewFrame();	
		glClear(GL_COLOR_BUFFER_BIT);

		m_text_edit.render(&m_settings);

		while(SDL_PollEvent(&event)) {
			switch(event.type) {

				case SDL_TEXTINPUT:
					m_text_edit.text_input(event.text.text);
					break;
				
				case SDL_KEYDOWN:
					m_text_edit.key_down(event.key.keysym.sym);
					break;

					/*
				case SDL_MOUSEMOTION:
					m_text_edit.mouse_moved(event.motion.x, event.motion.y);
					break;
					*/


				case SDL_WINDOWEVENT:
					if(event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
						m_settings.window_width = event.window.data1;
						m_settings.window_height = event.window.data2;
						glViewport(0, 0, m_settings.window_width, m_settings.window_height);
					}
					break;


				case SDL_QUIT:
					m_settings.quit = true;
					break;

				default: break;
			}			
		}

		const int now = SDL_GetTicks();
		int t = now-start_tick;
		
		if(tpf-t > 0) {
			SDL_Delay(tpf-t);
		}

		SDL_GL_SwapWindow(m_window);
	}
}

bool Edi::seems_fine() {
	return !m_problem_occurred;
}

void Edi::_cleanup_and_quit() {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();
	SDL_DestroyWindow(m_window);
	SDL_Quit();
	puts("bye!");
}

void Edi::_configure_imgui() {

	ImGuiIO& i = ImGui::GetIO();
	ImGuiStyle& s = ImGui::GetStyle();

	i.Fonts->AddFontFromFileTTF("./src/resource/font.otf", 20.f);

	s.Colors[ImGuiCol_WindowBg] = ImVec4(0.1, 0.1, 0.1, 1.0f);
	s.WindowPadding = ImVec2(3.0f, 2.0f);
	s.FramePadding = ImVec2(3.3f, 3.0f);
	s.ItemSpacing = ImVec2(4.0f, 3.5f);

	
}














