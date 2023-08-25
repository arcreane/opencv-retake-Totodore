#pragma once

#include <vector>
#include <memory>
#include <opencv2/opencv.hpp>
#include <functional>

class Module
{
public:
	explicit Module(cv::Mat *current_img, std::function<void(cv::Mat &)> update_img) : m_current_img(current_img), m_update_img(update_img) {}
	virtual ~Module() = default;
	virtual void update() = 0;

protected:
	cv::Mat *m_current_img;
	std::function<void(cv::Mat &)> m_update_img;
};