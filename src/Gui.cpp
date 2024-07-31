#include "Gui.h"

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

GUI::GUI(GLFWwindow* window, std::shared_ptr<Program> program, std::vector<std::string> camera_names, std::vector<std::shared_ptr<Texture>> camera_logos, float near_plane, float far_plane)
	: program{ program },
	  cameras_labels { camera_names },
	  camera_logos { camera_logos }
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls

	ImGui::StyleColorsDark();

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");

	output.camera_near_plane = near_plane;
	output.camera_far_plane = far_plane;
}

GUI::~GUI()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void GUI::render()
{
	imgui_frame_start();

	ImGui::Begin("Camera Placement GUI");

	// select camera type
	{
		static ImGuiComboFlags flags = 0;
		
		// General BeginCombo() API, you have full control over your selection data and display type.
		// (your selection data could be an index, a pointer to the object, an id for the object, a flag stored in the object itself, etc.)
		
		const char* item_current = cameras_labels.at(output.selected_camera_type).c_str();            // Here our selection is a single pointer stored outside the object.
		if (ImGui::BeginCombo("Select camera", item_current, flags)) // The second parameter is the label previewed before opening the combo.
		{
			for (unsigned int i = 0; i < cameras_labels.size(); i++)
			{
				std::string i_label = cameras_labels.at(i);
				bool is_selected = (item_current == i_label.c_str());

				ImGui::SetNextItemAllowOverlap();
				if (ImGui::Selectable(i_label.c_str(), is_selected))
				{
					item_current = i_label.c_str();
					output.selected_camera_type = i;
				}
				ImVec2 rect_min = ImGui::GetItemRectMin();
				ImVec2 rect_max = ImGui::GetItemRectMax();
				rect_max.x = rect_min.x + 32;

				std::shared_ptr<Texture> texture = camera_logos.at(i);
				ImGui::SameLine(); 
				ImGui::Image((void*)texture->get_id(), ImVec2(texture->get_width() / 64, texture->get_height() / 64));

				if (is_selected)
					ImGui::SetItemDefaultFocus();   // Set the initial focus when opening the combo (scrolling + for keyboard navigation support in the upcoming navigation branch)
			}
			ImGui::EndCombo();
		}

	}

	ImGui::SliderFloat("Placement distance", &output.placement_distance, 0.0f, 0.5f);
	

	if (ImGui::CollapsingHeader("Settings"))
	{
		if (ImGui::CollapsingHeader("Camera movement")) {
			ImGui::SliderFloat("Camera movement speed", &output.camera_movement_speed, 1.0f, 25.0f);
			ImGui::SliderFloat("Camera rotation speed", &output.camera_rotation_speed, 0.0005, 0.005);
		}

		if (ImGui::CollapsingHeader("Camera Near and Far plane")) {
			ImGui::SliderFloat("Camera near plane", &output.camera_near_plane, 0.01f, 0.1f);
			ImGui::SliderFloat("Camera far plane", &output.camera_far_plane, 10.0f, 2500.0f);
		}

		ImGui::Checkbox("Perform frustum culling", &output.toggle_frustum_culling);
		
		if (ImGui::CollapsingHeader("Anti-alisiang")) {
			ImGui::Checkbox("Perform msaa (recommended)", &output.toggle_msaa);
			ImGui::Checkbox("Perform fxaa", &output.toggle_fxaa);
		}
#ifdef DEBUG
		ImGui::Checkbox("Show bounding boxes", &output.toggle_aabb_drawing);
#endif
	}

	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);

	ImGui::End();

	imgui_frame_end();
}

inline void GUI::imgui_frame_start()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

inline void GUI::imgui_frame_end()
{
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}