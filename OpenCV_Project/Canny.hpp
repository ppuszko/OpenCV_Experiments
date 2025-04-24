#pragma once

#include <iostream>
#include <memory>
#include <print>
#include <stdexcept>


#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

struct CannyData
{
	cv::Mat src;
	cv::Mat srcGray;
	cv::Mat grayAfterBlur;
	cv::Mat dst;
	cv::Mat detectedEdges;

	std::string windowName{"Canny"};
	std::string	trackThresh{"Hysteresis threshold"};
	std::string	trackBlurKernel{"Blur kernel"};
	std::string trackSobelKernel{"Sobel kernel"};

	int lowThreshold{0};
	int maxLowThreshold{100};
	int ratio{2};
	int maxRatio{5};
	int blurKernelSize{3};
	int maxBlurKernelSize{12};
	int sobelKernelSize{3};
	int maxSobelKernelSize{7};
};

void CannyCallback(int, void* data)
{
	CannyData* cd = static_cast<CannyData*>(data);

	if (cd->blurKernelSize % 2 == 0)
		cd->blurKernelSize++;

	if (cd->sobelKernelSize % 2 == 0)
		cd->sobelKernelSize++;

	cv::blur(cd->srcGray, cd->grayAfterBlur, cv::Size(cd->blurKernelSize, cd->blurKernelSize));
	cv::Canny(cd->grayAfterBlur, cd->detectedEdges, cd->lowThreshold, cd->lowThreshold * cd->ratio, cd->sobelKernelSize);

	cd->dst = cv::Scalar::all(0);
	cd->src.copyTo(cd->dst, cd->detectedEdges);

	cv::Mat hConcat1;
	cv::hconcat(cd->detectedEdges, cd->grayAfterBlur, hConcat1);
	cv::cvtColor(hConcat1, hConcat1, cv::COLOR_GRAY2BGR);

	cv::Mat hConcat2;
	cv::hconcat(cd->dst, cd->src, hConcat2);

	cv::Mat final;
	cv::vconcat(hConcat1, hConcat2, final);

	cv::imshow(cd->windowName, final);
}

class Canny
{
public:
	Canny(const std::string& path)
	{
		data = std::make_unique<CannyData>();
		data->src = cv::imread(path, cv::IMREAD_COLOR);
		try
		{
			if (data->src.empty())
			{
				throw std::runtime_error(std::format("Can't load an image from {}\n", path));
			}
		}
		catch (std::exception& e)
		{
			std::cerr << e.what();
		}
		cv::cvtColor(data->src, data->srcGray, cv::COLOR_BGR2GRAY);
		data->dst = cv::Mat(data->src.size(), data->src.type());
	}



	void CreateTrackbars()
	{
		cv::createTrackbar(data->trackThresh, data->windowName, &data->lowThreshold, data->maxLowThreshold, CannyCallback, data.get());

		cv::createTrackbar(data->trackBlurKernel, data->windowName, &data->blurKernelSize, data->maxBlurKernelSize, CannyCallback, data.get());
		cv::setTrackbarMin(data->trackBlurKernel, data->windowName, 3);
		

		cv::createTrackbar(data->trackSobelKernel, data->windowName, &data->sobelKernelSize, data->maxSobelKernelSize, CannyCallback, data.get());
		cv::setTrackbarMin(data->trackSobelKernel, data->windowName, 3);
		
	}

	void Run()
	{
		cv::namedWindow(data->windowName, cv::WINDOW_NORMAL );
		CreateTrackbars();
		CannyCallback(0, data.get());
		while (true)
		{
			auto key = cv::waitKey(20);

			if (key == 'q')
				break;
		}
		cv::destroyWindow(data->windowName);
	}

private:
	std::unique_ptr<CannyData> data = nullptr;
};