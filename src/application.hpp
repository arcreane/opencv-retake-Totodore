#pragma once

#include "GLFW/glfw3.h"
#include "imgui.h"

#include <string>
#include <memory>
#include <vector>
#include <opencv2/opencv.hpp>
#include "modules/module.hpp"

// Modules
#include "modules/resize_module.hpp"

class Application
{
public:
	Application();
	~Application();
	void run();

private:
	void update();
	void render();

	void show_popup(std::string title, std::string message);

	void update_img();

private:
	GLFWwindow *m_window;
	ImVec4 m_clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	// This image should always be in BGR format
	cv::Mat m_img;
	cv::VideoCapture m_video;

	unsigned int m_texture_id = 2;

	std::vector<cv::Mat> m_img_history{};

	// clang-format off

    // List of modules with a callback to update the image
    std::array<std::unique_ptr<Module>, 1> m_modules = {
            std::make_unique<ResizeModule>(&m_img, [this] (cv::Mat &img) { this->update_img(); }),
    };
	// clang-format on
};