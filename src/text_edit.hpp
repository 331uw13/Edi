#pragma once

#include <vector>
#include <string>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_internal.h"

#include "settings.hpp"


class TextEdit {
public:

	void init();
	void render(Settings* t_settings);
	void text_input(const char* t_chr);
	void key_down(const uint8_t t_key);
	void open_file(const char* t_filename);

private:

	struct Cursor {
		int x { 0 };
		int y { 0 };
		int width_offset { 0 };

		float rounding { 2.05f };
		float width { -1.0f };

		bool fade { false };
		float fade_speed { 4.0f };
		ImVec4 fade_start;
		ImVec4 fade_end;

		bool drop_transparency { false };
		int drop_rate { 50 };

		ImVec4 color;
		//uint32_t color { IM_COL32(30, 200, 50, 200) };
	};


	// settings
	// TODO: make config reader or something
	
	static constexpr uint32_t m_line_num_color      { IM_COL32(100, 100, 100, 255) };
	static constexpr uint32_t m_default_text_color  { IM_COL32(200, 200, 200, 255) };
	static constexpr const char* m_EOF_mark { "~~~~!" };
	static constexpr int m_text_offset { 2 };
	static constexpr int m_tab_size { 3 };

	bool m_menu_open { false };
	int m_timer_start { 0 };

	int m_line_tab_count { 0 };
	int m_max_row { 0 };
	int m_max_column { 0 };
	int m_column_offset { 2 };
	ImVec2 m_char_size { ImVec2(0.0f, 0.0f) };
	ImVec2 m_win_size { ImVec2(0.0f, 0.0f) };

	void update_column_offset();
	void add_char_to_cursor(const char t_chr);
	void clamp_cursor();
	void move_cursor(const int t_x, const int t_y);
	void move_cursor_offset(const int t_off_x, const int t_off_y);
	bool line_is_safe(const int t_y);

	ImVec2 get_screen_pos(const int t_x, const int t_y);
	void draw_text(const ImGuiStyle& t_style, ImDrawList* t_draw, const char* t_text, const uint32_t t_color, const int t_x, const int t_y);

	void handle_enter();
	void handle_backspace();

	Cursor m_cursor;
	std::vector<std::string> m_page;

	static constexpr int m_imgui_window_flags {
		ImGuiWindowFlags_NoScrollbar |
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoSavedSettings |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoCollapse
	};

	ImVec4 _normalize_color(const uint8_t t_r, const uint8_t t_g, const uint8_t t_b, const uint8_t t_a);
	void _imgui_color_edit(const char* t_id, ImVec4& t_color);


};
