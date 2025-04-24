#pragma once

#include <filesystem>

#include <opencv2/opencv.hpp>

class ImageInpainting
{
public:
	explicit ImageInpainting(const std::filesystem::path& path) :
		image_(cv::imread(path.string()))
	{
		if (image_.empty())
			throw std::runtime_error(std::format("Can't load image from specified path {}", path.string()));

		imageCopy_ = image_.clone();
		mask_ = cv::Mat::zeros(image_.size(), CV_8U);
	}

	const cv::Mat& GetImage() const
	{
		return image_;
	}
	
	cv::Mat& GetImage() 
	{
		return image_;
	}

	cv::Mat& GetMask()
	{
		return mask_;
	}

	void Reset()
	{
		image_ = imageCopy_.clone();
		mask_ = cv::Scalar::all(0);
	}

	cv::Point GetPoint() const
	{
		return previousPoint_;
	}
	
	void SetPoint(const cv::Point& point)
	{
		previousPoint_ = point;
	}

	cv::Mat UseInpaint(double radius, int flag)
	{
		cv::Mat result;

		cv::inpaint(image_, mask_, result, radius, flag);

		return result;
	}

	const std::string originalWindowName = "original";
	const std::string maskWindowName = "mask";

private:
	cv::Mat image_;
	cv::Mat imageCopy_;
	cv::Mat mask_;
	cv::Point previousPoint_{-1, -1};
};


void mouseCallback(int event, int x, int y, int flags, void* data)
{
	ImageInpainting* imageData = static_cast<ImageInpainting*>(data);

	if(event == cv::EVENT_LBUTTONUP || !(flags & cv::EVENT_FLAG_LBUTTON))
	{
		imageData->SetPoint({-1, -1});
	}
	else if (event == cv::EVENT_LBUTTONDOWN)
	{
		imageData->SetPoint({x, y});
	}
	else if (event == cv::EVENT_MOUSEMOVE && (flags & cv::EVENT_FLAG_LBUTTON))
	{
		cv::Point pt{x, y};
		if (imageData->GetPoint().x < 0)
			imageData->SetPoint(pt);
		
		cv::line(imageData->GetMask(),
			imageData->GetPoint(),
			pt,
			cv::Scalar::all(255),
			5, 
			cv::LINE_AA);

		cv::line(imageData->GetImage(),
			imageData->GetPoint(),
			pt,
			cv::Scalar::all(255),
			5,
			cv::LINE_AA);

		imageData->SetPoint(pt);

		cv::imshow(imageData->originalWindowName, imageData->GetImage());
		cv::imshow(imageData->maskWindowName, imageData->GetMask());
	}
}