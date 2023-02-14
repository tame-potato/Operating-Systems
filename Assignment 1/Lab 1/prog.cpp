/*******************************************************************************/
/*                          Programmer: Pablo Ruiz                             */
/*                                                                             */
/*                          Course: Operating Systems                          */
/*                                                                             */
/*                          Project: Assignment 1 Part 1 A                     */
/*                                                                             */
/*******************************************************************************/

#include <iostream>
#include <string>
#include <vector>
#include <unistd.h>
#include <sys/wait.h>

using namespace std ;

int main( int argc, char *argv[] ){

    int c1, c2, c3 ;
    string msg = "Child running: " ;
    char* env[] = {NULL} ;

    // Create new processes with each fork call, the number of total processes created (including parent) will be 2^n, 
    // where n is the number of consecutive fork() calls
    c1 = fork() ;
    c2 = fork() ;
    c3 = fork() ;

    if ( c1 == 0 && c2 != 0 && c3 != 0 ) { //first child of the parent

        // Print this child's lineage
        cout << "Child 1 of parent running" << endl ;

        // Run the executable b 
        execve( "./b", argv , env) ;

        // wait for its children to end before exiting
        waitpid(c2, NULL, 0) ;
        waitpid(c3, NULL, 0) ;

        exit(0) ;

    }
    else if ( c2 == 0 && c1 != 0 && c3 != 0 ) {  //second child of parent

        cout << "Child 2 of parent running" << endl ;

        execve( "./b", argv , env) ;

        // wait for its child to end before exiting
        waitpid(c3, NULL, 0) ;

        exit(0) ;

    }
    else if ( c3 == 0 && c1 != 0 && c2 != 0 ) { //third child of parent

        cout << "Child 3 of parent running" << endl ;

        execve( "./b", argv , env) ;

        exit(0) ;

    }
    else if ( c1 == 0 && c2 == 0 && c3 != 0 ) { //first child of child 1

        cout << "Child 1 of Child 1 running" << endl ;

        execve( "./b", argv , env) ;

        // wait for its child to end before exiting
        waitpid(c3, NULL, 0) ;

        exit(0) ;

    }
    else if ( c1 == 0 && c2 != 0 && c3 == 0 ) { //second child of child 1

        cout << "Child 2 of Child 1 running" << endl ;

        execve( "./b", argv , env) ;

        exit(0) ;

    }
    else if ( c3 == 0 && c1 != 0 && c2 == 0 ) { //first child of child 2

        cout << "Child 1 of child 2 running" << endl ;

        execve( "./b", argv , env) ;

        exit(0) ;

    }
    else if ( c3 == 0 && c1 == 0 && c2 == 0 ) { //first child of child 1 of child 1

        cout << "Child 1 of child 1 of child 1 is running" << endl ;

        execve( "./b", argv , env) ;

        exit(0) ;

    }
    
    else if ( c1 != 0 && c2 != 0 && c3 != 0 ) { // Parent is running

        cout << "Parent Running" << endl ;

        // wait for each of the parent's child processes to end before exiting
        waitpid(c1, NULL, 0) ;
        waitpid(c2, NULL, 0) ;
        waitpid(c3, NULL, 0) ;
    }

    return 0 ;
}