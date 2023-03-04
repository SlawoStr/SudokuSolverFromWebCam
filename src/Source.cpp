#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <vector>
#include <optional>
#include <Python.h>
#include "PythonConnector.h"

const int N = 9;

// Checks whether it will be
// legal to assign num to the
// given row, col
bool isSafe(int grid[N][N], int row,
	int col, int num)
{

	// Check if we find the same num
	// in the similar row , we
	// return false
	for (int x = 0; x <= 8; x++)
		if (grid[row][x] == num)
			return false;

	// Check if we find the same num in
	// the similar column , we
	// return false
	for (int x = 0; x <= 8; x++)
		if (grid[x][col] == num)
			return false;

	// Check if we find the same num in
	// the particular 3*3 matrix,
	// we return false
	int startRow = row - row % 3,
		startCol = col - col % 3;

	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			if (grid[i + startRow][j +
				startCol] == num)
				return false;

	return true;
}

bool solveSudoku(int grid[N][N], int row, int col)
{
	// Check if we have reached the 8th
	// row and 9th column (0
	// indexed matrix) , we are
	// returning true to avoid
	// further backtracking
	if (row == N - 1 && col == N)
		return true;

	// Check if column value  becomes 9 ,
	// we move to next row and
	//  column start from 0
	if (col == N) {
		row++;
		col = 0;
	}

	// Check if the current position of
	// the grid already contains
	// value >0, we iterate for next column
	if (grid[row][col] > 0)
		return solveSudoku(grid, row, col + 1);

	for (int num = 1; num <= N; num++)
	{

		// Check if it is safe to place
		// the num (1-9)  in the
		// given row ,col  ->we
		// move to next column
		if (isSafe(grid, row, col, num))
		{

			/* Assigning the num in
			   the current (row,col)
			   position of the grid
			   and assuming our assigned
			   num in the position
			   is correct     */
			grid[row][col] = num;

			//  Checking for next possibility with next
			//  column
			if (solveSudoku(grid, row, col + 1))
				return true;
		}

		// Removing the assigned num ,
		// since our assumption
		// was wrong , and we go for
		// next assumption with
		// diff num value
		grid[row][col] = 0;
	}
	return false;
}

/// <summary>
/// Sort squares
/// </summary>
/// <param name="lhs"></param>
/// <param name="rhs"></param>
/// <returns></returns>
bool sortSquares(const cv::Rect& lhs, const cv::Rect& rhs)
{
	if (abs(lhs.y - rhs.y) < 20)
	{
		return lhs.x < rhs.x;
	}
	return lhs.y < rhs.y;
}

/// <summary>
/// Detect lines on img
/// </summary>
/// <param name="src">Source image</param>
/// <returns>Image with detected lines</returns>
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

/// <summary>
/// Detect sudoku number squares ( and return them if their number is equal to number of sudoku squares(81))
/// </summary>
/// <param name="src">Source image</param>
/// <returns>Return rectangle shapes </returns>
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

int main(int argc, char* argv[]) {

	// Video from camera
	cv::VideoCapture cap(0);
	// Parameters
	int medianaBlur{ 5 };
	int erosionSize{ 1 };
	cv::Mat matFrame, matGray, matMediana, matThreshold, matOpen, matLines;
	cv::Mat element{ cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(2 * erosionSize + 1, 2 * erosionSize + 1), cv::Point(erosionSize, erosionSize)) };
	PythonConnector connector;
	bool isFinished{ false };
	int sudokuTabel[9][9]{};

	while (true)
	{
		// Handle keyboard events
		auto keyPressed = cv::waitKey(15);
		if (keyPressed == 'q')
		{
			break;
		}
		else if (keyPressed == 'r')
		{
			isFinished = false;
		}
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
			if (!isFinished)
			{
				cv::Size boxSize{ 28,28 };
				for (int i = 0; i < 9; i++)
				{
					for (int j = 0; j < 9; j++)
					{
						// Prepare detected box for ml model (28x28)
						cv::Mat roi{ matOpen((*boundingBoxes)[i * 9 + j]) };
						cv::resize(roi, roi, boxSize);
						cv::imwrite("Digit.png", roi);
						// Fill sudoku table from predictions
						sudokuTabel[i][j] = connector.predictDigit();
					}
				}
				// Run solver
				solveSudoku(sudokuTabel, 0, 0);
				isFinished = true;
			}
			else
			{
				// Draw detected squares and predicted values
				for (int i = 0; i < 9; i++)
				{
					for (int j = 0; j < 9; j++)
					{
						cv::Rect rect = (*boundingBoxes)[i * 9 + j];
						cv::rectangle(matFrame, rect, cv::Scalar(255, 0, 255));
						std::string result = std::to_string(sudokuTabel[i][j]);
						cv::putText(matFrame, result, cv::Point(rect.x + rect.width / 3, rect.y + rect.height), cv::FONT_HERSHEY_COMPLEX, 1.5, cv::Scalar(36, 255, 12));
					}
				}
			}
		}
		// Detection steps visualisation
		imshow("Oryginal", matFrame);
		imshow("Gray", matGray);
		imshow("Mediana", matMediana);
		imshow("Threshold", matThreshold);
		imshow("Open", matOpen);
		imshow("Result", matLines);
	}
    return 0;
}