/*******************************************************************************/
/*                          Programmer: Pablo Ruiz                             */
/*                                                                             */
/*                          Course: Operating Systems                          */
/*                                                                             */
/*                          Project: Assignment 1 Part 2                       */
/*                                                                             */
/*******************************************************************************/

#include <iostream>
#include <string>
#include <vector>
#include <unistd.h>
#include <sys/wait.h>

using namespace std ;

int main( int argc, char *argv[] ){

    int i, child ;
    int *childArr ;
    // Read the first argument, interpret as an int and multiply it times two
    int numProc = atoi(argv[1]) * 2 ; 

    // Print the number of processes that will be created (2*n)
    cout << numProc << " processes will be created" << endl ;

    // Allocate space for the array of integers that will contain the process ID of all the children
    childArr = (int*) malloc(numProc * sizeof(int)) ;

    // Loop as 2*n times and create a child each iteration
    for (i = 0 ; i < numProc ; i++){

        child = fork() ;

        // If its the children process it will enter this loop, print, and then exit so that it doesnt spawn additional children in the next loop
        if ( child == 0 ) { 

            cout << "I am " << getpid() << " and my parent is " << getppid() << endl ;

            exit(0) ; 
            
        }
        // The parent appends the ID of the new child to the array
        else{ childArr[i] = child ; }

    }

    // The parent loops 2*n times, waiting for each of the child process IDs in the array.
    for (i = 0 ; i < numProc ; i++){

        waitpid(childArr[i], NULL, 0) ;

    }

    return 0 ;
}