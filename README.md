opencv_task2
============

Material (and your code) for Task 2. Fork this repository to write your solution to the task.

NOTES
=====

For part 1, I used opencv's FlannBasedMatcher to find matches between each pair of pictures
and then draw the matches that were found. There were quite a few matches that are obviously
(to human eyes) wrong but make at least some sense given the similarities between some parts
of the box and the nondescript nature of the backgroun, but there are also a number of matches 
that look pretty close.

For part 2, I used the opencv findChessboardCorners function to locate the checkerboard patterns.
This is NOT the most robust way of doing it, since this function can easily fail to find a chessboard
if it isn't pretty good (the upper left checker board has some white spots that cause the function
to fail, so since I was running low on time I manually modified the image to show that the algorithm
works in principle. This could perhaps also be solved by performing erosions/dilations to get rid of holes)
Since the algorithm also only finds one checkerboard, and as I had less time than I would have liked,
I simply applied the algorithm to four subsections of the image (it was divided in half each way).
