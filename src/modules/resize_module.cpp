#include "resize_module.hpp"

#include "imgui.h"

#include <chrono>
#include <iostream>

void ResizeModule::update()
{
	static int w = m_current_img->cols;
	static int h = m_current_img->rows;
	static bool keep_aspect_ratio = true;

	static int previous_w = w;
	static int previous_h = h;

	if (keep_aspect_ratio && w - previous_w != 0)
		h = ((m_current_img->rows * w) / m_current_img->cols);
	else if (keep_aspect_ratio && h - previous_h != 0)
		w = ((m_current_img->cols * h) / m_current_img->rows);

	previous_w = w;
	previous_h = h;

	// Create a window
	ImGui::Begin("Camera window");

	ImGui::BeginGroup();
	ImGui::PushItemWidth(120);
	ImGui::InputInt("##", &w, 16, 4096);
	ImGui::PopItemWidth();
	ImGui::SameLine();
	ImGui::SliderInt("Width", &w, 16, 4096);
	ImGui::EndGroup();

	ImGui::BeginGroup();
	ImGui::PushItemWidth(120);
	ImGui::InputInt("###", &h, 16, 4096);
	ImGui::PopItemWidth();
	ImGui::SameLine();
	ImGui::SliderInt("Height", &h, 16, 4096);
	ImGui::EndGroup();

	ImGui::Checkbox("Keep aspect ratio", &keep_aspect_ratio);
	ImGui::Text("Current aspect ratio: %.2f", (float)w / (float)h);
	ImGui::Text("FPS: %f", ImGui::GetIO().Framerate);

	ImGui::Spacing();

	if (ImGui::Button("Resize !"))
	{
		// Heavy computation on another thread (e.g. performing openCV operations)
		m_future = std::async(std::launch::async, [this]
							  {
			cv::Mat dest;
			cv::Mat src = m_current_img->clone();
			cv::resize(src, dest, cv::Size(w, h),0,0, cv::INTER_NEAREST);
			return dest; });
	}

	/// When the heavy computation is done, update the image
	if (m_future.valid())
	{
		auto status = m_future.wait_for(std::chrono::milliseconds(0));
		if (status == std::future_status::ready)
		{
			cv::Mat result = m_future.get();
			m_update_img(result);
		}
	}

	// End the window
	ImGui::End();
}
