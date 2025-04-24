#pragma once

#include <string>
#include <ranges>

#include <opencv2/opencv.hpp>

class AutoFocus
{
public:

	AutoFocus(const std::string& p)
	{
		cap = cv::VideoCapture(p);
		try
		{
			if (!cap.isOpened())
				throw std::runtime_error(std::format("Can't open video from given path: {}\n", p));
		}
		catch (std::exception& e)
		{
			std::cerr << e.what();
		}

		fps = cap.get(cv::CAP_PROP_FPS);
		delay = static_cast<int>(1000 / (fps*12));
	}

	~AutoFocus() { cap.release(); }

	cv::Mat CalculateSharpness(const cv::Mat& currentFrame)
	{
		//blur frame and convert to grayscale
		cv::Mat gray;
		cv::GaussianBlur(currentFrame, gray, cv::Size(5, 5), 0, 0, cv::BORDER_DEFAULT);
		cv::cvtColor(gray, gray, cv::COLOR_BGR2GRAY);

		//perform canny edge detection
		cv::Mat dest;
		cv::Canny(gray, dest, 100, 300, 3);

		//iterate over processed frame to measure it's edge density
		cv::Mat reshaped = dest.reshape(1,1);
		int currMax = 0;
		std::for_each(reshaped.begin<uchar>(), reshaped.end<uchar>(), [&](uchar& pixel){
			currMax += static_cast<int>(pixel);
		});

		//grab and save frame if it's the best found so far
		if (currMax >= maxSharpness)
		{
			maxSharpness = currMax;
			sharpestFrame = currFrame;
			cap.grab();
			cap.retrieve(selectedFrame);
		}

		return dest;
	}

	//manage input
	void WaitInput(char key)
	{
		if (key == 'q')
			running = false;
		else if (key == 'p')
			cv::waitKey(0);
		else if (key == 'c')
			cv::waitKey(delay);
		else if (key == 'o')
			displayOriginal = !displayOriginal;
	}

	//process frame and display desired version - original or processed
	void DisplayCurrentFrame(cv::Mat& frame)
	{	
		cv::Mat processed = CalculateSharpness(frame);
		currFrame++;

		if (displayOriginal)
			cv::imshow(windowName, frame);
		else
			cv::imshow(windowName, processed);
	}

	//display the best found frame 
	void DisplayChosenFrame()
	{
		std::cout << "\nmax sharpness: " << maxSharpness << " at frame: " << sharpestFrame;
		cv::imshow(windowName, selectedFrame);
		cv::waitKey(0);
		cv::destroyWindow(windowName);
	}

	void Run()
	{
		cv::namedWindow(windowName, cv::WINDOW_NORMAL);

		while (cap.isOpened() && running)
		{
			//capture frame
			cv::Mat frame;
			cap.read(frame);
			if (frame.empty())
				break;

			//display frame and process frame 
			DisplayCurrentFrame(frame);
			
			//manage input
			WaitInput(cv::waitKey(delay));
		}
		DisplayChosenFrame();
	}

private:

	cv::VideoCapture cap;
	std::string windowName = "Auto focus";
	cv::Mat selectedFrame;
	double fps;
	int delay;
	int maxSharpness = 0;
	int currFrame = 0;
	int sharpestFrame = 0;
	bool running = true;
	bool displayOriginal = true;
	
};