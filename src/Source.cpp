#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <vector>
#include <optional>
#include "PythonConnector.h"

bool sortSquares(const cv::Rect& lhs, const cv::Rect& rhs)
{
	if (abs(lhs.y - rhs.y) < 20)
	{
		return lhs.x < rhs.x;
	}
	return lhs.y < rhs.y;
}

cv::Mat detectLines(cv::Mat& src)
{
	std::vector<cv::Vec4i> linesP;
	cv::Mat result{ cv::Mat::zeros(src.size(), CV_8U) };
	cv::HoughLinesP(src, linesP, 1, CV_PI / 180, 50, 50, 10);
	//Draw lines on empty mat
	for (size_t i = 0; i < linesP.size(); i++)
	{
		cv::Vec4i line{ linesP[i] };
		cv::line(result, cv::Point(line[0], line[1]), cv::Point(line[2], line[3]), cv::Scalar(255, 255, 255), 3, cv::LINE_AA);
	}
	cv::bitwise_not(result, result);
	return result;
}

std::optional<std::vector<cv::Rect>> detectSudokuContours(cv::Mat& src)
{
	std::vector<std::vector<cv::Point>> contours;
	std::vector<cv::Vec4i> hierarchy;
	const int minArea{ 1000 };
	const int maxArea{ 3500 };

	cv::findContours(src, contours, hierarchy, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);
	// Filter results based on contour area (Get rid of false detection of small or very big contours)
	for (auto it = contours.begin(); it != contours.end();)
	{
		double area = cv::contourArea(*it);
		if (area > minArea && area < maxArea)
		{
			++it;
		}
		else
		{
			it = contours.erase(it);
		}
	}
	if (contours.size() == 81)
	{
		std::vector<cv::Rect> boundingBoxes;
		for (int i = 0; i < 81; ++i)
		{
			boundingBoxes.push_back(cv::boundingRect(contours[i]));
		}
		std::sort(boundingBoxes.begin(), boundingBoxes.end(), sortSquares);
		return boundingBoxes;
	}
	return std::nullopt;
}

int main() {
	// Video from camera
	cv::VideoCapture cap(0);
	// Parameters
	int medianaBlur{ 5 };
	int erosionSize{ 1 };
	cv::Mat matFrame, matGray, matMediana, matThreshold, matOpen, matLines;
	cv::Mat element{ cv::getStructuringElement(cv::MORPH_RECT, cv::Size(2 * erosionSize + 1, 2 * erosionSize + 1), cv::Point(erosionSize, erosionSize)) };

	while (cv::waitKey(15) != 'q')
	{
		// Error couldnt take a frame from camera
		cap >> matFrame;
		if (matFrame.empty())
		{
			std::cout << "Error occured during taking frame check if camera is working and is connected" << std::endl;
			break;
		}
		// Image preprocessing
		cvtColor(matFrame, matGray, cv::COLOR_BGR2GRAY);
		cv::medianBlur(matGray, matMediana, medianaBlur);
		cv::adaptiveThreshold(matMediana, matThreshold, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY_INV, 11, 2);
		cv::erode(matThreshold, matOpen, element);
		cv::dilate(matOpen, matOpen, element);

		// Detect number squars location on image
		matLines = detectLines(matOpen);
		auto boundingBoxes = detectSudokuContours(matLines);
		if (boundingBoxes)
		{
			cv::Size boxSize{ 28,28 };
			for (int i = 0; i < 9; i++)
			{
				for (int j = 0; j < 9; j++)
				{
					// Prepare detected box for ml model (28x28)
					cv::Mat roi{ matGray((*boundingBoxes)[i * 9 + j]) };
					cv::resize(roi, roi, boxSize);
					cv::imwrite("Digit.png", roi);
				}
			}
		}
		// Print result of detection
		imshow("Oryginal", matFrame);
		imshow("Gray", matGray);
		imshow("Mediana", matMediana);
		imshow("Threshold", matThreshold);
		imshow("Open", matOpen);
		imshow("Result", matLines);
	}
    return 0;
}