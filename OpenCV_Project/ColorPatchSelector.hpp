#pragma once

#include <opencv2/opencv.hpp>
#include <vector>
#include <ranges>

class ColorPatchSelector
{
public:
	void loadFrame(const cv::Mat& frame)
	{
		cv::cvtColor(frame, lab_, cv::COLOR_BGR2Lab);
	}
	void fillVector(cv::Point coords)
	{
		colorsRange_.push_back(lab_.at<cv::Vec3b>(coords.y, coords.x));
	}
	cv::Mat getMask()
	{
		cv::Mat mask;
		cv::inRange(lab_, lower_, upper_, mask);

		return mask;
	}



private:
	
	void findExtremas()
	{
		std::ranges::for_each(colorsRange_, [this](const auto& color){
			lower_[0] = std::min(static_cast<int>(lower_[0]), static_cast<int>(color[0]));
			lower_[1] = std::min(static_cast<int>(lower_[1]), static_cast<int>(color[1]));
			lower_[2] = std::min(static_cast<int>(lower_[2]), static_cast<int>(color[2]));

			upper_[0] = std::max(static_cast<int>(upper_[0]), static_cast<int>(color[0]));
			upper_[1] = std::max(static_cast<int>(upper_[1]), static_cast<int>(color[1]));
			upper_[2] = std::max(static_cast<int>(upper_[2]), static_cast<int>(color[2]));
		});
	}

	std::vector<cv::Vec3b> colorsRange_;
	cv::Mat lab_;
	cv::Vec3b lower_{255, 255, 255};
	cv::Vec3b upper_{0, 0, 0};

};

