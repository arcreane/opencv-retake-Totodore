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
	void perspective_transform();

	void img_gl_call(const cv::Mat &img);

private:
	GLFWwindow *m_window;
	ImVec4 m_clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	// This image should always be in BGR format
	cv::Mat m_img;
	cv::VideoCapture m_video;
	std::vector<cv::Point> m_contours;
	unsigned int m_texture_id = 2;

	bool frozen = false;
	bool camera_off = false;
	bool previewing = false;


	// Options
	float m_canny_t1 = 45.0f;
	float m_canny_t2 = 200.0f;
	float m_epsilon = 0.1f;
	int m_area_min = 500;
	bool m_closed_polygon = true;

	// Computation task
	std::future<cv::Mat> m_future;

};