#include <vector>
#include <ranges>
#include <opencv2/opencv.hpp>
#include <filesystem>
#include <array>
#include <cmath>
#include <stdexcept>
#include <numbers>
#include <algorithm>
#include <print>
#include <thread>
#include <random>
#include <numeric>

#include "ScreenMatting.hpp"
#include "CameraCapture.hpp"

template <typename Iterator, typename T>
T myAccumulate(Iterator begin, Iterator end, T init)
{
	const auto length = std::distance(begin, end);
	auto availableThreads = std::thread::hardware_concurrency() - 1;
	size_t blockSize = length / availableThreads;

	std::vector<T> results(availableThreads);
	std::vector<std::thread> threads(availableThreads);

	auto first = begin;

	for (int i{0}; i < availableThreads; ++i)
	{
		
		auto last = (i == availableThreads - 1) ? end : std::next(first, blockSize);
		threads[i] = std::thread([first, last, &results, i](){
			results[i] = std::accumulate(first, last, T{});
		});
		first = last;
	}

	for (auto& t : threads)
	{
		t.join();
	}

	for (const auto& r : results)
	{
		init += r;
	}

	return init;

}

void colorSelector(int event, int x, int y, int flags, void* param)
{
	ScreenMatting* sm{static_cast<ScreenMatting*>(param)};
	if (event == cv::EVENT_LBUTTONDOWN)
	{
		sm->setPoints(cv::Point{x, y});
	}
}

int main()
{
	cv::utils::logging::setLogLevel(cv::utils::logging::LOG_LEVEL_SILENT);

	CameraCapture cam(1);

	while (true)
	{
		auto frame = cam.getFrame();

		if(!frame.empty())
			cv::imshow("frame", frame);

		auto key = cv::waitKey(1);
		if (key == 'q')
			break;
	}
}
