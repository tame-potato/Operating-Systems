Readme for Lab 1 
Class: Operating Systems
Semester: Fall 2021
Author: Pablo Ruiz

Purpose:

	The purpose of this lab is to demonstrate the creation of new "children" processes from a "parent" process by using the function fork().
	Part one of the lab calls the fork() function multiple times consecutively thus creating (2^n - 1) new processes where n is the number of fork() calls.
	Part two of the lab takes user input through the first command line argument and creates (2*n) new processes where n is the user input.

Names of Source Code Files:

	Part 1: prog.cpp
		b.cpp

	Part 2: prog_part2.cpp

Name of Executable Files:

	Part 1: part1
		b

	Part 2: part2

How to Run:

	Part 1:
		In the command line, while being in the directory containing the executable file "part1", type "./part1" and hit enter. The terminal should 
		return 15 lines of text announcing in which order the processes ran. The first 8 lines should correspond to the processes while the last 7 should 
		correspond to program B.

	Part 2:
		
		In the command line, while being in the directory containing the executable file "part2", type "./part2 " followed by an integer and hit enter. 
		The terminal should return (2*n + 1) lines of text, where n is the integer passed as an argument. The first line will anounce the number of processes 
		that will be created. The following lines will list the process ID of all the processes created. 