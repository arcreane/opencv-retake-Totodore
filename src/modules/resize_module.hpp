#pragma once
#include "module.hpp"

#include <future>
#include <thread>

class ResizeModule : public Module
{

	// Inherit the constructors of Module (they are not inherited automatically)
	using Module::Module;

public:
	void update() override;
private:
	// Future to compute heavy stuff in another thread
	std::future<cv::Mat> m_future;
};
