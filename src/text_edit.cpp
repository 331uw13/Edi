#include <string_view>
#include <SDL2/SDL.h>
#include <cmath>

#include "text_edit.hpp"


void TextEdit::init() {
	m_page.resize(3);

	// TODO: read config file
	
	m_cursor.color = _normalize_color(30, 200, 50, 200);

}

void TextEdit::open_file(const char* t_filename) {
}

void TextEdit::render(Settings* t_settings) {
	if(t_settings == nullptr) { return; }

//	ImGui::ShowDemoWindow();


	ImGui::SetNextWindowSize(ImVec2(t_settings->window_width, t_settings->window_height));
	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::Begin("Edi", (bool*)NULL, m_imgui_window_flags);
	

	ImGuiStyle& s = ImGui::GetStyle();
	ImGuiWindow* window = ImGui::GetCurrentWindow();
	ImGuiContext* g = GImGui;
	ImGuiIO& io = ImGui::GetIO();
	m_win_size = window->Size;
	m_win_size.x -= s.FramePadding.x;
	m_win_size.y -= s.FramePadding.y;
	m_char_size = ImGui::GetFont()->CalcTextSizeA(g->FontSize, FLT_MAX, 0.0f, " ", NULL, NULL);
	m_max_column = m_win_size.x/m_char_size.x;
	m_max_row = m_win_size.y/m_char_size.y;

	if(m_cursor.width < 0) {
		m_cursor.width = m_char_size.x;
	}

	g->MouseCursor = m_menu_open ? ImGuiMouseCursor_Arrow : ImGuiMouseCursor_None;
	const int offsets = m_text_offset+m_column_offset;

	m_line_tab_count = 0;
	for(int i = 0; i < m_cursor.x; i++) {
		if(m_page[m_cursor.y][i] == '\t') {
			m_line_tab_count++;
		}
	}

	ImVec2 cursor_ppos((offsets+m_cursor.x+(m_line_tab_count*m_tab_size))*m_char_size.x, m_cursor.y*m_char_size.y);
	cursor_ppos.x += s.FramePadding.x;
	cursor_ppos.y += s.FramePadding.y;

	
	{
		ImGui::BeginChild("##text_region", m_win_size, false, ImGuiWindowFlags_NoScrollbar);

		// draw cursor	
		window->DrawList->AddRectFilled(cursor_ppos, ImVec2(cursor_ppos.x+m_char_size.x-(m_char_size.x/m_cursor.width), cursor_ppos.y+m_char_size.y),
				ImGui::GetColorU32(m_cursor.color), m_cursor.rounding);

		// draw text
		for(size_t i = 0; i < m_page.size(); i++) {
			char line_num_buf[32];
			sprintf(line_num_buf, "%*li", m_column_offset, i);
			draw_text(s, window->DrawList, line_num_buf, m_line_num_color, 0, i);
			draw_text(s, window->DrawList, m_page[i].c_str(), m_default_text_color, m_column_offset+m_text_offset, i);
		}

		if(!m_menu_open) {
			draw_text(s, window->DrawList, m_EOF_mark, IM_COL32(200, 120, 15, 255), 
					m_max_column - (ImGui::CalcTextSize(m_EOF_mark, NULL, false).x/m_char_size.x), m_page.size());
		}
	}
	
	if(m_menu_open) {
		ImGui::SetWindowFontScale(0.8f);
		ImGui::SetNextWindowPos(ImVec2(m_win_size.x/2, 0));

		ImGui::BeginChild("##menu_region", ImVec2(m_win_size.x/2, m_win_size.y), true);
		ImGui::Separator();

		if(ImGui::BeginTabBar("##settings", ImGuiTabBarFlags_None)) {
			
			if(ImGui::BeginTabItem("Cursor")) {
				
				ImGui::Checkbox("Fade", &m_cursor.fade);
				ImGui::Checkbox("Drop transparency", &m_cursor.drop_transparency);

				ImGui::Separator();
				
				_imgui_color_edit("Cursor color", m_cursor.color);
				if(m_cursor.fade) {
					_imgui_color_edit("Cursor fade  (from)", m_cursor.fade_start);
					_imgui_color_edit("Cursor fade  (to)", m_cursor.fade_end);
					
				}

				ImGui::Separator();
				
				ImGui::SliderFloat("##cursor_width", &m_cursor.width, 1.1f, m_char_size.x, "Width: %1.2f");
				ImGui::SliderFloat("##cursor_rounding", &m_cursor.rounding, 0.0f, 5.0f, "Rounding: %1.2f");
				if(m_cursor.fade) {
					ImGui::SliderFloat("##fade_speed", &m_cursor.fade_speed, 0.01f, 10.f, "Fade speed: %1.2f");
				}
				if(m_cursor.drop_transparency) {
					ImGui::SliderInt("##drop_rate", &m_cursor.drop_rate, 1, 255, "Drop rate: %i");					
				}

				ImGui::EndTabItem();
			}

			/*
			if(ImGui::BeginTabItem("[Debug]")) {
				if(ImGui::Button("[DEBUG] Dump text to stdout")) {
					printf("\033[35m@ START DUMP\033[0m\n");
					for(size_t i = 0; i < m_page.size(); i++) {
						printf("%s\n", m_page[i].c_str());
					}
					printf("\033[35m@ END DUMP\033[0m\n");
				}

				ImGui::EndTabItem();
			}
			*/

			ImGui::EndTabBar();
		}

		ImGui::EndChild();
		ImGui::SetWindowFontScale(1.0f);
	}

	ImGui::EndChild();
	ImGui::End();
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void TextEdit::key_down(const uint8_t t_key) {
	printf("\033[35m [Edi] --> \033[0m%c / 0x%x\n", t_key, t_key);
	
	if(m_page.empty() || m_page.size() == 0) {
		m_page.push_back("");
	}

	switch(t_key) {

		case 0x52: // up
			move_cursor_offset(0, -1);
			break;

		case 0x51: // down
			move_cursor_offset(0, 1);
			break;

		case 0x50: // left
			move_cursor_offset(-1, 0);
			break;

		case 0x4F: // right
			move_cursor_offset(1, 0);
			break;

		case 0x8: // backspace
			handle_backspace();
			break;

		case 0xD: // enter
			handle_enter();
			break;

		case 0x09: // tab
			add_char_to_cursor('\t');
			break;

		case 0x1B:
			m_menu_open =! m_menu_open;
			break;

		default: break;
	}
}

void TextEdit::text_input(const char* t_chr) {
	if(m_page.empty() || m_page.size() == 0) {
		m_page.push_back("");
	}
	add_char_to_cursor(t_chr[0]);
}

void TextEdit::handle_backspace() {
	if(line_is_safe(m_cursor.y)) {
		std::string& line = m_page[m_cursor.y];
		const size_t len = line.length();

		if(m_cursor.x <= 0 && len == 0) {
			// current line is empty and cursor is at left
			// --> erase this line
			if(m_page.size() > 1) {
				m_page.erase(m_page.begin()+m_cursor.y);
				const int up = m_cursor.y-1;
				if(line_is_safe(up)) {
					std::string& line_above = m_page[up];
					move_cursor_offset(line_above.length(), -1);
				}
			}
		}
		else if(m_cursor.x <= 0 && len > 0) {
			// current line is not empty but cursor is at left
			// --> add this line to end of 1 line above, if cursor is not already at the top
			const int up = m_cursor.y-1;
			if(line_is_safe(up)) {
				std::string& line_above = m_page[up];
				const size_t old_length = line_above.length();
				line_above.append(line);
				
				// erase duplicate
				m_page.erase(m_page.begin()+m_cursor.y);
				move_cursor_offset(old_length, -1);
			}
		}
		else if(m_cursor.x > 0) {
			// cursor is not at left so is the line not empty
			// --> erase character at cursor position
			line.erase(line.begin()+m_cursor.x-1);
			move_cursor_offset(-1, 0);
		}
		clamp_cursor();
		update_column_offset();
	}
}

void TextEdit::handle_enter() {
	if(line_is_safe(m_cursor.y)) {
		std::string& line = m_page[m_cursor.y];

		add_char_to_cursor('\n');
		const std::string& part = line.substr(m_cursor.x, line.length());

		line.erase(line.begin()+m_cursor.x, line.end());
		m_page.insert(m_page.begin()+m_cursor.y+1, part);

		move_cursor_offset(0, 1);
		m_cursor.x = 0;

		update_column_offset();
		clamp_cursor();
	}
}


void TextEdit::add_char_to_cursor(const char t_chr) {
	if(((t_chr > 0x1F && t_chr < 0x7F) || t_chr == 0x09) && line_is_safe(m_cursor.y)) {
		std::string& line = m_page[m_cursor.y];
		line.insert(line.begin()+m_cursor.x, t_chr);
		m_cursor.x++;
	}
}

bool TextEdit::line_is_safe(const int t_y) {
	return (!m_page.empty() && m_page.size() > static_cast<size_t>(t_y) && t_y >= 0);
}

void TextEdit::clamp_cursor() {
	const size_t page_size = m_page.size();
	if(m_cursor.y < 0) {
		m_cursor.y = 0;
	}
	else if(static_cast<size_t>(m_cursor.y+1) > page_size) {
		m_cursor.y = page_size-1;
	}

	const size_t line_length = m_page[m_cursor.y].length();
	if(m_cursor.x < 0) {
		m_cursor.x = 0;
	}
	else if(static_cast<size_t>(m_cursor.x+1) > line_length) {
		m_cursor.x = line_length;
	}
}

void TextEdit::update_column_offset() {
	m_column_offset = log10(m_page.size())+2;
}

void TextEdit::move_cursor(const int t_x, const int t_y) {
	m_cursor.x = t_x;
	m_cursor.y = t_y;
	clamp_cursor();
}

void TextEdit::move_cursor_offset(const int t_off_x, const int t_off_y) {
	m_cursor.x += t_off_x;
	m_cursor.y += t_off_y;
	clamp_cursor();
}

void TextEdit::draw_text(const ImGuiStyle& t_style, ImDrawList* t_draw, const char* t_text, const uint32_t t_color, const int t_x, const int t_y) {
	ImGuiContext* g = GImGui;
	t_draw->AddText(g->Font, g->FontSize, ImVec2(t_x*m_char_size.x+t_style.FramePadding.x, t_y*m_char_size.y+t_style.FramePadding.y),
		   	t_color, t_text, NULL, 0.0f, NULL);
}

ImVec4 TextEdit::_normalize_color(const uint8_t t_r, const uint8_t t_g, const uint8_t t_b, const uint8_t t_a) {
	return ImVec4(t_r/255.f, t_g/255.f, t_b/255.f, t_a/255.f);
}

void TextEdit::_imgui_color_edit(const char* t_id, ImVec4& t_color) {
	/*ImGui::ColorEdit4(t_id, (float*)&t_color.x, 
		   	ImGuiColorEditFlags_AlphaBar |
		   	ImGuiColorEditFlags_AlphaPreview |
		   	ImGuiColorEditFlags_NoOptions | 
			ImGuiColorEditFlags_NoInputs |
			ImGuiColorEditFlags_DisplayRGB);
	*/

	if(ImGui::TreeNode(t_id)) {
		ImGui::SetNextItemWidth(300);
		ImGui::ColorPicker4(t_id, (float*)&t_color.x, ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
		ImGui::TreePop();
	}
}









