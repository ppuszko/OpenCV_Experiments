#pragma once

#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <shared_mutex>
#include <thread>
#include <atomic>

auto CapDeleter = [](cv::VideoCapture* ptr){
	if (ptr->isOpened())
	{
		ptr->release();
		std::cout << "Cap released";
	}
	delete ptr;
};
using CapPtr = std::unique_ptr<cv::VideoCapture, decltype(CapDeleter)>;

class CameraCapture
{
public:

	CameraCapture(int id) : id_(id), keepRunning_(true), stopped_(false), cap_(new cv::VideoCapture(id_), CapDeleter)
	{
		
		start();
	}

	CameraCapture(const CameraCapture& other) = delete;
	CameraCapture(CameraCapture&& other) = delete;
	CameraCapture& operator=(const CameraCapture& other) = delete;
	CameraCapture& operator=(CameraCapture&& other) = delete;

	~CameraCapture()
	{
		stop();
	}

	void stop()
	{
		if (stopped_) return;

		stopped_ = true;
		keepRunning_ = false;

		if (captureThread_.joinable())captureThread_.join();

		
	}

	cv::Mat getFrame()
	{
		std::shared_lock<std::shared_mutex> lock(frameMutex_);
		
		return frame_.clone();
	}

private:

	void mainLoop()
	{
		//cap_->open(id_);
		if (!cap_->isOpened())
		{
			std::cerr << "Failed to open camera capture" << std::endl;
			return;
		}
		while (keepRunning_)
		{
			cv::Mat temp;
			cap_->read(temp);
			if (temp.empty()) continue;

			std::unique_lock<std::shared_mutex> lock(frameMutex_);
			cv::flip(temp, temp, 1);
			frame_ = std::move(temp);
		}
	}

	void start()
	{
		captureThread_ = std::thread(&CameraCapture::mainLoop, this);
	}

	CapPtr cap_;
	cv::Mat frame_;
	unsigned int id_;

	std::shared_mutex frameMutex_;
	std::thread captureThread_;
	std::atomic<bool> keepRunning_;
	std::atomic<bool> stopped_;
};