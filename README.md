# SudokuSolverFromWebCam

A program that allows you to detect with a webcam a sudoku game and solve it. Each frame taken from a webcam is subjected to serval operations.   
Each frame is: 
- Changed to grayscale
- Smoothed using mediana filter
- Thresholded using adaptive thresholding
- Eroded to remove noise
- Dilated to improve lines visibility
