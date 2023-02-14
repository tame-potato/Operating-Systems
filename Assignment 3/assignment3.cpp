/*******************************************************************************/
/*                          Programmer: Pablo Ruiz                             */
/*                                                                             */
/*                          Course: Operating Systems                          */
/*                                                                             */
/*                          Project: Assignment 3                              */
/*                                                                             */
/*******************************************************************************/

#include <iostream>
#include <mutex>
#include <string>
#include <vector>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

#define MAX_SIZE 150 

using namespace std ;

class gap{
    public:

    char *addr ;
    int size ;
    bool state ; // True = Ocuppied, False = Free

    gap(char *start, int length, bool occupied){

        addr = start ;
        size = length ;
        state = occupied ;

    }

    void set(char *start, int length, bool occupied){
        addr = start ;
        size = length ;
        state = occupied ;
    }
};

// Global variables
void *mem ;
int activeUsers = 0 ;
string option1 ; // first_fit, best_fit, or worst_fit
string option2 ; // frag or no_frag
vector<gap> memory ;
mutex manager,
      user;

// Checks to see if the adjacent gaps have continuous addresses and are free, if the are they are merged back together into one gap
void consolidate(vector<gap>::iterator itr){ 

    vector<gap>::iterator next,
                          prev ;
    
    if (itr != memory.end()-1){

        next = itr + 1 ;

        if (next->state == false && next->addr == itr->addr + itr->size){
            itr->size += next->size ;
            memory.erase(next) ;
        }
    }

    if (itr != memory.begin()){

        prev = itr - 1 ;

        if (prev->state == false && prev->addr == itr->addr - prev->size){
            prev->size += itr->size ;
            memory.erase(itr) ;
        }
    }

}

// Defragments all of the empty gaps to the back and all of the full memory to the front
void defrag(){
    int index ;
    vector<gap>::iterator j ;

    // separate free gaps from full gaps
    for (vector<gap>::iterator i = memory.begin(); i < memory.end(); i++){

        if (i->state == false && i != memory.end()-1) {

            if ((i+1)->state == true){

                index = i - memory.begin() ;

                swap(memory[index], memory[index+1]) ;
            }

        }

    }

    // merge empty gaps
    for (vector<gap>::iterator i = memory.begin(); i < memory.end(); i++){

        if (i->state == false){

            j = i + 1 ;

            while (j < memory.end()){

                i->size += j->size ;
                memory.erase(j) ;
                j = i + 1 ;

            }

            break ;

        }

    }

}

void *split(vector<gap>::iterator itr, int numBytes){

    void *allocated = itr->addr ;
    int diff = itr->size - numBytes ;

    if (numBytes < itr->size){

        gap leftover (itr->addr + numBytes, diff, false) ;
        gap newItr (itr->addr, numBytes, true) ;

        memory.erase(itr) ;
        memory.insert(memory.begin(), leftover) ;
        memory.insert(memory.begin(), newItr) ;

    }
    else if (numBytes == itr->size){

        itr->state = true ;

    }

    return allocated ;

}

// First fit memory selection
void *first_fit(int numBytes){

    void *allocated = NULL ;

    // Check for a gap that fits the required memory
    for (vector<gap>::iterator i = memory.begin(); i < memory.end(); i++){

        if (i->size >= numBytes && i->state == false){
            allocated = split(i, numBytes) ;
            break ;
        }

    }

    return allocated ;

}

// Best fit memory selection
void *best_fit(int numBytes){

    void *allocated = NULL ;
    int bestDelta = MAX_SIZE,
        currentDelta = 0 ;
    vector<gap>::iterator bestIt = memory.end() ;

    for (vector<gap>::iterator i = memory.begin(); i < memory.end(); i++) {

        currentDelta = i->size - numBytes ;

        if (bestDelta > currentDelta && currentDelta >= 0 && i->state == false) {

            bestDelta = currentDelta ;
            bestIt = i ; 

        }

    }

    if (bestIt == memory.end()) 
        return NULL ;
    else
        allocated = split(bestIt, numBytes) ;

    return allocated ;

}

// Worst fit memory selection
void *worst_fit(int numBytes){

    void *allocated = NULL ;
    int worstDelta = -MAX_SIZE,
        currentDelta = 0 ;
    vector<gap>::iterator bestIt = memory.end() ;

    for (vector<gap>::iterator i = memory.begin(); i < memory.end(); i++) {

        currentDelta = i->size - numBytes ;

        if (worstDelta < currentDelta && currentDelta >= 0 && i->state == false) {

            worstDelta = currentDelta ;
            bestIt = i ; 

        }

    }

    if (bestIt == memory.end()) 
        return NULL ;
    else
        allocated = split(bestIt, numBytes) ;

    return allocated ;

}

// Selects a type of memory allocation. If an incorrect option is passed it uses best fit
void *memory_malloc(int numBytes){

    void *addr = NULL ;

    if (numBytes > MAX_SIZE){
        cout << "Memory request larger than MAX_SIZE, rejoining the queue and retrying with lower value." << endl ;
        return addr ;
    }

    if (option1 == "first_fit") {
        addr = first_fit(numBytes) ;
    }
    else if (option1 == "worst_fit")
        addr = worst_fit(numBytes) ;

    else 
        addr = best_fit(numBytes) ;

    return addr ;

}

// Frees the gaps corresponding to the passed address/es and tries to join them with adjacent gaps if there addresses are continuous and they are free, can also perform defragmentation
void memory_free(void *addr){

    for (vector<gap>::iterator i = memory.begin(); i < memory.end(); i++) {

        if (addr == i->addr && i->state == true) {

            i->state = false ;

            consolidate(i) ;

            if (option2 == "frag")
                defrag() ;

        }

    }

}

// Threads requesting memory
void *thread(void *rank){

    void *addr = NULL ;
    int requestSize,
        sleepTime ;

    while (addr == NULL) {

        requestSize = rand() % (int)(1.3 * MAX_SIZE) + 1 ;
        sleepTime = rand() % 10 ;

        // Sleep to allow other threads to release their memory
        sleep(1) ;

        // Wait until the user mutex is unlocked
        user.lock() ;

        cout << endl << "I am thread " << *(int *)rank << ": requesting " << requestSize <<" bytes and going to sleep for " << sleepTime << " seconds." << endl ;

        addr = memory_malloc(requestSize) ;    

        // Unlock the mutex so that other threads can access the memory
        user.unlock() ;

    }

    // Sleep for a maximum of 10 sec
    sleep(sleepTime) ;

    user.lock() ;

    cout << endl << "I am thread " << *(int *)rank << " waking up" << endl ;

    memory_free(addr) ;

    cout << "I am thread " << *(int *)rank << ": " << requestSize << " bytes released succesfully. Exiting now." << endl ;

    user.unlock() ;

    activeUsers-- ;

}

// Memory management system which allocates memory from system and coordinates the requesting threads
void *mms(void *data){

    // Lock the users from interacting with memory until it has been allocated
    user.lock() ;

    // Allocate memory to be managed from the system
    gap mem ((char*)malloc(MAX_SIZE), (int)MAX_SIZE, false) ;

    // Add the empty memory space to the list
    memory.push_back(mem) ;

    // Wait until the manager mutex is unlocked
    manager.lock() ;

    // Unlock the users 
    user.unlock() ;

    while (activeUsers > 0) {}

    manager.unlock() ;

}

int main( int argc, char *argv[] ){

    int numThreads = atoi(argv[1]),
        threadRank[numThreads],
        i ;

    option1 = argv[2],
    option2 = argv[3] ;

    // Seed random function
    srand(time(0)) ;

    // Initialize thread ID array to store PIDs of every thread
    pthread_t threadID[numThreads+1] ;

    // Lock manager mutex so that we can control when it starts
    manager.lock() ;

    // Initialize Memory Management Thread
    pthread_create(&threadID[0], NULL, mms, NULL) ;

    // Loop once for every provider rquired
    for (i = 0; i < numThreads; i++) {

        // Store the providers rank so a pointer to it can be passed to the thread
        threadRank[i] = i + 1 ;

        // Create a provider thread and pass it its rank
        pthread_create(&threadID[i+1], NULL, thread, &threadRank[i]) ;

        // Increase the number of active users
        activeUsers++ ;
    
    }

    // Unlock the manager allowing it to begin
    manager.unlock() ;

    // Wait for all threads to exit
    for (i = 0; i < numThreads+1; i++)
        pthread_join(threadID[i], NULL) ;


    return 0 ;
}