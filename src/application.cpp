#include "application.hpp"

#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "ImGuiFileDialog.h"

#include <cstdio>
#include <string>

static void glfw_error_callback(int error, const char *description)
{
	fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

Application::Application()
{
	glfwSetErrorCallback(glfw_error_callback);
	if (!glfwInit())
		return;

	// GL 3.0 + GLSL 130
	const char *glsl_version = "#version 130";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

	// Create window with graphics context
	m_window = glfwCreateWindow(1280, 720, "Dear ImGui GLFW+OpenGL3 example", nullptr, nullptr);
	if (m_window == nullptr)
		return;
	glfwMakeContextCurrent(m_window);
	glfwSwapInterval(1); // Enable vsync
	glfwMaximizeWindow(m_window);

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO &io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls
	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	// ImGui::StyleColorsLight();

	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(m_window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);

	io.Fonts->AddFontFromFileTTF("assets/fonts/Roboto.ttf", 20);
	m_video.open(0, cv::CAP_DSHOW);
	m_video.set(cv::CAP_PROP_FRAME_HEIGHT, 1080);
	m_video.set(cv::CAP_PROP_FRAME_WIDTH, 1920);
}

Application::~Application()
{
	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(m_window);
	glfwTerminate();
}

void Application::run()
{
	while (!glfwWindowShouldClose(m_window))
	{
		// Poll and handle events (inputs, window resize, etc.)
		// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
		// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
		// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
		// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
		glfwPollEvents();

		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		// Rendering
		update();
		render();
	}
}

void Application::update()
{

	// defining img window
	{
		ImGuiIO &io = ImGui::GetIO();
		ImGui::SetNextWindowSize(io.DisplaySize);
		ImGui::SetNextWindowPos(ImGui::GetMainViewport()->Pos);
		ImGui::Begin("Image", NULL, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);

		if (ImGui::MenuItem("Open an image", "Ctrl+O", true, true))
		{
			ImGuiFileDialog::Instance()->OpenDialog(
				"ImgPicker",
				"Choose an image",
				"Image files (.bmp *.dib *.jpeg *.jpg *.jpe *.jp2 *.png *.webp *.pbm *.pgm *.ppm *.pxm *.pnm *.sr *.ras *.tiff *.tif){.bmp,.dib,.jpeg,.jpg,.jpe,.jp2,.png,.webp,.pbm,.pgm,.ppm,.pxm,.pnm,.sr,.ras,.tiff,.tif}",
				"/");
		}
		if (ImGui::MenuItem("Take a shot", nullptr, true))
		{
			// TODO: faire l'historique
		}
		if (ImGui::MenuItem("Save", "Ctrl+S", true, !m_img.empty()))
		{
			ImGuiFileDialog::Instance()->OpenDialog(
				"ImgSavePicker",
				"Choose an image",
				"Image files (.bmp *.dib *.jpeg *.jpg *.jpe *.jp2 *.png *.webp *.pbm *.pgm *.ppm *.pxm *.pnm *.sr *.ras *.tiff *.tif){.bmp,.dib,.jpeg,.jpg,.jpe,.jp2,.png,.webp,.pbm,.pgm,.ppm,.pxm,.pnm,.sr,.ras,.tiff,.tif}",
				"/");
		}

		float height = (float)m_video.get(cv::CAP_PROP_FRAME_HEIGHT);
		float width = (float)m_video.get(cv::CAP_PROP_FRAME_WIDTH);
		// Computing viewport in order to center image and keep aspect ratio
		float img_aspect_ratio = width / height;
		float viewport_aspect_ratio = io.DisplaySize.x / io.DisplaySize.y;
		float scale = img_aspect_ratio > viewport_aspect_ratio ? io.DisplaySize.x / width : io.DisplaySize.y / height;
		ImGui::Image((void *)(intptr_t)m_texture_id, ImVec2(width, height));

		ImGui::End();
	}

	// defining img picker
	{
		if (ImGuiFileDialog::Instance()->Display("ImgPicker"))
		{
			// action if OK
			if (ImGuiFileDialog::Instance()->IsOk())
			{
				std::string path = ImGuiFileDialog::Instance()->GetFilePathName();
				cv::Mat img = cv::imread(path);
				if (img.empty())
				{
					show_popup("Error", "Could not load image");
				}
				else
				{
					// update_img(img);
				}
				ImGuiFileDialog::Instance()->Close();
			}
			ImGuiFileDialog::Instance()->Close();
		}
	}

	// defining img saver
	{
		if (ImGuiFileDialog::Instance()->Display("ImgSavePicker"))
		{
			// action if OK
			if (ImGuiFileDialog::Instance()->IsOk())
			{
				std::string path = ImGuiFileDialog::Instance()->GetFilePathName();
				cv::imwrite(path, m_img);
				ImGuiFileDialog::Instance()->Close();
			}
			ImGuiFileDialog::Instance()->Close();
		}
	}

	if (!m_img.empty())
	{
		// Defining module windows
		for (auto &module : m_modules)
		{
			module->update();
		}
	}

	update_img();
}

void Application::show_popup(std::string title, std::string message)
{
	static bool show_popup = true;
	ImGui::OpenPopup(title.c_str());
	if (show_popup)
	{
		ImGui::BeginPopupModal(title.c_str(), &show_popup, ImGuiWindowFlags_AlwaysAutoResize);
		ImGui::Text(message.c_str());
		ImGui::Separator();

		if (ImGui::Button("OK", ImVec2(120, 0)))
		{
			show_popup = false;
		}
		ImGui::EndPopup();
	}
}

void Application::update_img()
{
	static cv::Mat img;
	m_video >> img;

	cv::cvtColor(img, img, cv::COLOR_BGR2RGB);

	glDeleteTextures(1, &m_texture_id);

	glEnable(GL_TEXTURE_2D);
	// glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glGenTextures(1, &m_texture_id);
	glBindTexture(GL_TEXTURE_2D, m_texture_id);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Set texture clamping method
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	glTexImage2D(GL_TEXTURE_2D,	   // Type of texture
				 0,				   // Pyramid level (for mip-mapping) - 0 is the top level
				 GL_RGB,		   // Internal colour format to convert to
				 img.cols,		   // Image width  i.e. 640 for Kinect in standard mode
				 img.rows,		   // Image height i.e. 480 for Kinect in standard mode
				 0,				   // Border width in pixels (can either be 1 or 0)
				 GL_RGB,		   // Input image format (i.e. GL_RGB, GL_RGBA, GL_BGR etc.)
				 GL_UNSIGNED_BYTE, // Image data type
				 img.ptr());	   // The actual image data itself
}

void Application::render()
{
	ImGui::Render();
	int display_w, display_h;
	glfwGetFramebufferSize(m_window, &display_w, &display_h);
	glViewport(0, 0, display_w, display_h);
	glClearColor(m_clear_color.x * m_clear_color.w, m_clear_color.y * m_clear_color.w, m_clear_color.z * m_clear_color.w, m_clear_color.w);
	glClearDepth(0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	glfwSwapBuffers(m_window);
}