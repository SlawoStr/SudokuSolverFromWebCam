# SudokuSolverFromWebCam

A program that allows you to detect with a webcam a sudoku game and solve it. Each frame taken from a webcam is subjected to serval operations.   
Each frame is: 
- Changed to grayscale
- Smoothed using mediana filter
- Thresholded using adaptive thresholding
- Eroded to remove noise
- Dilated to improve line visibility

These operations are followed by the detection of lines separating squares with numbers using the Hough transformation. Based on these lines, contours are detected, which are then filtered based on their area and position on the frame to get rid of misdetection. If, after these operations, the remaining number of contours is not 81 (the number of squares in sudoku) then the current frame is discarded. Then each of the detected contours is passed to the digit recognizer (trained on MNIST dataset). Only predictions with high confidence are accepted. Prediction are made in python using keras CNN model. The final step is to pass the results to the sudoku solver algorithm. The result of his work is then shown on camera in real time. 

# Example

Detected sudoku example:   
![Oryginal](https://github.com/SlawoStr/SudokuSolverFromWebCam/blob/master/Results/Gray.png)

Image after thresholding and morphology operations:    
![Oryginal](https://github.com/SlawoStr/SudokuSolverFromWebCam/blob/master/Results/Open.png)

Image after contour detection and prediction:    
![Oryginal](https://github.com/SlawoStr/SudokuSolverFromWebCam/blob/master/Results/PredictionResultFinal.png)

Final results after sudoku solver algorithm:    
![Oryginal](https://github.com/SlawoStr/SudokuSolverFromWebCam/blob/master/Results/SolverResultFinal.png)


