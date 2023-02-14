Readme for Lab 3
Class: Operating Systems
Semester: Fall 2021
Author: Pablo Ruiz

Purpose:

    To implement a memory management system and explore different memory allocation strategies such as first fit, best fit, and worst fit.


Names of Source Code Files:

    assignment3.cpp
	
Name of Executable Files:

    assignment3
	
How to Run:

    When calling the executable follow it with three options:

        Number of threads to be created:
            Any integer (N)

        Option 1:
            first_fit
            worst_fit
            best_fit

        Option 2:
            frag
            no_frag

    Example:

        ./assignment3 10 first_fit no_frag

        (* The example above will create 10 threads which are allocated memory using the first_fit strategy without using fragmentation.)