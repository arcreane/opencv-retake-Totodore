#pragma once

#include "GLFW/glfw3.h"
#include "imgui.h"

#include <string>
#include <memory>
#include <vector>
#include <opencv2/opencv.hpp>
#include <thread>
#include <future>

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

	cv::Mat detect_contours(const cv::Mat &img);

private:
	GLFWwindow *m_window;
	ImVec4 m_clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	// This image should always be in BGR format
	cv::Mat m_img;
	cv::VideoCapture m_video;

	unsigned int m_texture_id = 2;

	bool frozen = false;
	bool camera_off = false;


	// Options
	float m_canny_t1 = 45.0f;
	float m_canny_t2 = 200.0f;
	float m_epsilon = 0.1f;
	bool m_convex_check = true;

	// Computation task
	std::future<cv::Mat> m_future;

};