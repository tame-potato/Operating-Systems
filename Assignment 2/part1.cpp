/*******************************************************************************/
/*                          Programmer: Pablo Ruiz                             */
/*                                                                             */
/*                          Course: Operating Systems                          */
/*                                                                             */
/*                          Project: Assignment 2                              */
/*                                                                             */
/*******************************************************************************/

#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <condition_variable>
#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <time.h>

using namespace std ;

// Declare global variables
// this mutex protects both the buffer and the number of active Buyers
mutex bufferMutex ;
condition_variable cvBuffer ;
sem_t providerSemaphore ;
queue<int> productBuffer ;
int activeBuyers ;


// Check conditional variable
bool bufferNotEmpty() {return !productBuffer.empty() ;} 

// Buyer Function
void *buyer(void *rank) {

    // Generate unique lock corresponding to the bufferMutex for each buyer
    unique_lock<mutex> lock(bufferMutex) ;
    
    // Wait to be notified and for the buffer not to be empty and lock the mutex
    cvBuffer.wait(lock, bufferNotEmpty) ;

    // Print buyer status and pop a product from the buffer
    cout << "Buyer " << *(int*)rank << " is buying product " << productBuffer.front() << " and exiting\n" ;
    productBuffer.pop() ;

    // Decrement the number of active buyers
    activeBuyers-- ;

    // Mutex is unlocked when it leaves the scope

}

// Provider function
void *provider(void *rank) {

    // Wait until the provider semaphore is larger than 0
    sem_wait(&providerSemaphore) ;

    // Seed rand function with the current time multiplied times the provider rank to make sure they both start at around the same time but have different seeds
    srand(time(NULL)* *(int*)rank) ;

    while (true) {

        // Lock the mutex until the end of each loop
        lock_guard<mutex> lock(bufferMutex) ;

        // If no buyers are left exit loop and return
        if (activeBuyers == 0){break ;}

        // Push number into buffer
        productBuffer.push(rand()) ;

        // Print the number pushed onto the buffer by the provider
        cout << "Provider " << *(int*)rank << " is adding product " << productBuffer.back() << "\n" ;

        // Notify one of the buyers that is waiting
        cvBuffer.notify_one();

    }

    // Increment semaphore before returning to allow any threads that are waiting to accesss it
    sem_post(&providerSemaphore) ;

}

int main(int argc, char *argv[]) {

    int numProviders = 2,
        numBuyers = atoi(argv[1]),
        numThreads = numProviders + numBuyers,
        providerRank[numProviders],
        buyerRank[numBuyers],
        i ;

    // Initialize thread ID array to store PIDs of every thread
    pthread_t threadID[numThreads] ;

    // Initialize the semaphore to 0 to prevent the providers from starting until signaled
    sem_init(&providerSemaphore, 0, 0) ;

    // Loop once for every provider rquired
    for (i = 0; i < numProviders; i++) {

        // Store the providers rank so a pointer to it can be passed to the thread
        providerRank[i] = i + 1 ;

        // Create a provider thread and pass it its rank
        pthread_create(&threadID[i], NULL, provider, &providerRank[i]) ;
    
    }

    // Loop once for every buyer rquired
    for (i = 0; i < numBuyers; i++) {

        // Store the buyers rank to a pointer so it can be passed to the thread
        buyerRank[i] = i + 1 ;

        // Create buyer thread and pass it its rank
        pthread_create(&threadID[i + numProviders], NULL, buyer, &buyerRank[i]) ;

        // Lock mutex until the end of each loop to increment the number of active buyers
        lock_guard<mutex> lock(bufferMutex) ;

        // Increment the activeBuyer count
        activeBuyers++ ;

    }

    // Increment (signal) the semaphore once for every provider thread
    for (i = 0; i < numProviders; i++)
        sem_post(&providerSemaphore) ;

    // Wait for all threads to exit
    for (i = 0; i < numThreads; i++)
        pthread_join(threadID[i], NULL) ;

    cout << "All threads have finished running and main is exiting cleanly. Good bye!\n" ;

    return 0 ;

}

