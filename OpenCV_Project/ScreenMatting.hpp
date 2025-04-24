#pragma once

#include <filesystem>

#include "ColorPatchSelector.hpp"


class ScreenMatting
{
public:
	ScreenMatting(const std::filesystem::path& path)
	{
		background_ = cv::imread(path.string());
		if (background_.empty())
		{
			throw std::runtime_error("Background didn't load properly");
		}
	}

	void setPoints(cv::Point point)
	{
		colorPicker_.fillVector(point);
	}

	void setMask()
	{
		mask_ = colorPicker_.getMask();
	}

	cv::Mat& getSetFrame()
	{
		return image_;
	}

	void process()
	{
		checkSize();
		colorPicker_.loadFrame(image_);
		setMask();

		cv::bitwise_not(mask_, mask_);
		cv::cvtColor(mask_, mask_, cv::COLOR_GRAY2BGR);
		cv::Mat temp = background_.clone();

		cv::bitwise_and(temp, ~mask_, temp);

		cv::bitwise_and(image_, mask_, image_);

		cv::bitwise_or(image_, temp, image_);
	}

	const std::string windowName_ = "window";

private:

	void checkSize()
	{
		if (!isBackgroundFitting_)
		{
			cv::resize(background_, background_, image_.size());
			isBackgroundFitting_ = true;
		}
	}

	ColorPatchSelector colorPicker_;
	cv::Mat image_;
	cv::Mat background_;
	cv::Mat processed_;
	cv::Mat mask_;
	bool isBackgroundFitting_ = false;


};

