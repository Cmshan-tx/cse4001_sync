#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <cstdlib>
#include "semaphore_class.h"

using namespace std;

//------------------------------------------------------------
// Problem 1: No-starve Readers/Writers
//------------------------------------------------------------

int data1 = 0;
int readCount1 = 0;
Semaphore turnstile1(1);
Semaphore roomEmpty1(1);
Semaphore mutex1(1);

void* reader1(void* arg) {
    long id = (long)arg;
    for (int i = 0; i < 5; i++) {
        turnstile1.wait();
        turnstile1.signal();

        mutex1.wait();
        readCount1++;
        if (readCount1 == 1) roomEmpty1.wait();
        mutex1.signal();

        cout << "[P1] Reader " << id << " reading: " << data1 << endl;
        usleep(100000);

        mutex1.wait();
        readCount1--;
        if (readCount1 == 0) roomEmpty1.signal();
        mutex1.signal();
        usleep(100000);
    }
    return nullptr;
}

void* writer1(void* arg) {
    long id = (long)arg;
    for (int i = 0; i < 5; i++) {
        turnstile1.wait();
        roomEmpty1.wait();

        data1++;
        cout << "[P1] Writer " << id << " writing: " << data1 << endl;
        usleep(150000);

        roomEmpty1.signal();
        turnstile1.signal();
        usleep(120000);
    }
    return nullptr;
}

void run_problem1() {
    cout << "Running Problem 1: No-starve Readers/Writers" << endl;

    pthread_t readers[5], writers[5];
    for (long i = 0; i < 5; i++) pthread_create(&readers[i], nullptr, reader1, (void*)i);
    for (long i = 0; i < 5; i++) pthread_create(&writers[i], nullptr, writer1, (void*)i);

    for (int i = 0; i < 5; i++) pthread_join(readers[i], nullptr);
    for (int i = 0; i < 5; i++) pthread_join(writers[i], nullptr);

    cout << "Problem 1 complete." << endl << endl;
}

//------------------------------------------------------------
//------------------------------------------------------------
// Problem 2: Writer-Priority Readers/Writers
//------------------------------------------------------------

int readCount2 = 0;
int writeCount2 = 0;
int data2 = 0;

Semaphore rmutex2(1);      // protects readCount2
Semaphore wmutex2(1);      // protects writeCount2
Semaphore readTry2(1);     // blocks readers when writers waiting
Semaphore resource2(1);    // shared resource

void* reader2(void* arg) {
    long id = (long)arg;

    for (int i = 0; i < 5; i++) {
        readTry2.wait();
        rmutex2.wait();
        readCount2++;
        if (readCount2 == 1) resource2.wait();
        rmutex2.signal();
        readTry2.signal();

        cout << "[P2] Reader " << id << " reading: " << data2 << endl;
        usleep(120000);

        rmutex2.wait();
        readCount2--;
        if (readCount2 == 0) resource2.signal();
        rmutex2.signal();

        usleep(100000);
    }
    return nullptr;
}

void* writer2(void* arg) {
    long id = (long)arg;

    for (int i = 0; i < 5; i++) {
        wmutex2.wait();
        writeCount2++;
        if (writeCount2 == 1) readTry2.wait();
        wmutex2.signal();

        resource2.wait();

        data2++;
        cout << "[P2] Writer " << id << " writing: " << data2 << endl;
        usleep(150000);

        resource2.signal();

        wmutex2.wait();
        writeCount2--;
        if (writeCount2 == 0) readTry2.signal();
        wmutex2.signal();

        usleep(110000);
    }
    return nullptr;
}
void run_problem2() {
    cout << "Running Problem 2: Writer-Priority Readers/Writers" << endl;

    pthread_t readers[5], writers[5];

    for (long i = 0; i < 5; i++) pthread_create(&readers[i], nullptr, reader2, (void*)i);
    for (long i = 0; i < 5; i++) pthread_create(&writers[i], nullptr, writer2, (void*)i);

    for (int i = 0; i < 5; i++) pthread_join(readers[i], nullptr);
    for (int i = 0; i < 5; i++) pthread_join(writers[i], nullptr);

    cout << "Problem 2 complete." << endl << endl;
}



Semaphore chopsticks3[5] = {Semaphore(1),Semaphore(1),Semaphore(1),Semaphore(1),Semaphore(1)};


void* philosopher3(void* arg) {
long id = (long)arg;
int left = id;
int right = (id + 1) % 5;


for (int i = 0; i < 5; i++) {
cout << "[P3] Philosopher " << id << " thinking" << endl;
usleep(120000);


// Asymmetry: odd philosophers pick up right first
if (id % 2 == 0) {
chopsticks3[left].wait();
cout << "[P3] Philosopher " << id << " picked LEFT " << left << endl;
chopsticks3[right].wait();
cout << "[P3] Philosopher " << id << " picked RIGHT " << right << endl;
} else {
chopsticks3[right].wait();
cout << "[P3] Philosopher " << id << " picked RIGHT " << right << endl;
chopsticks3[left].wait();
cout << "[P3] Philosopher " << id << " picked LEFT " << left << endl;
}


cout << "[P3] Philosopher " << id << " eating" << endl;
usleep(150000);


chopsticks3[left].signal();
chopsticks3[right].signal();
cout << "[P3] Philosopher " << id << " put down chopsticks" << endl;


usleep(120000);
}
return nullptr;
}
void run_problem3() {
    cout << "Running Problem 3: Dining Philosophers (Asymmetry)" << endl;

    pthread_t phils[5];
    for (long i = 0; i < 5; i++)
        pthread_create(&phils[i], nullptr, philosopher3, (void*)i);

    for (int i = 0; i < 5; i++)
        pthread_join(phils[i], nullptr);

    cout << "Problem 3 complete." << endl << endl;
}

//------------------------------------------------------------
// Problem 4: Dining Philosophers (Footman solution)
//------------------------------------------------------------

// 5 chopsticks, each a binary semaphore
Semaphore chopsticks4[5] = {
    Semaphore(1), Semaphore(1), Semaphore(1), Semaphore(1), Semaphore(1)
};

// Footman semaphore: at most 4 philosophers may try to eat at once
Semaphore footman4(4);

void* philosopher4(void* arg) {
    long id = (long)arg;
    int left  = id;
    int right = (id + 1) % 5;

    for (int i = 0; i < 5; i++) {
        cout << "[P4] Philosopher " << id << " thinking" << endl;
        usleep(120000);

        // Ask the footman for permission to try to eat
        footman4.wait();

        // Pick up chopsticks
        chopsticks4[left].wait();
        cout << "[P4] Philosopher " << id << " picked LEFT " << left << endl;
        chopsticks4[right].wait();
        cout << "[P4] Philosopher " << id << " picked RIGHT " << right << endl;

        cout << "[P4] Philosopher " << id << " eating" << endl;
        usleep(150000);

        // Put down chopsticks
        chopsticks4[left].signal();
        chopsticks4[right].signal();
        cout << "[P4] Philosopher " << id << " put down chopsticks" << endl;

        // Tell footman we're done
        footman4.signal();

        usleep(120000);
    }

    return nullptr;
}

void run_problem4() {
    cout << "Running Problem 4: Dining Philosophers (Footman)" << endl;

    pthread_t phils[5];

    // Create 5 philosopher threads
    for (long i = 0; i < 5; i++) {
        pthread_create(&phils[i], nullptr, philosopher4, (void*)i);
    }

    // Wait for them to finish
    for (int i = 0; i < 5; i++) {
        pthread_join(phils[i], nullptr);
    }

    cout << "Problem 4 complete." << endl << endl;
}


//------------------------------------------------------------
// Main dispatcher
//------------------------------------------------------------

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cout << "Usage: ./cse4001_sync <problem#>" << endl;
        return 1;
    }

    int p = atoi(argv[1]);
    switch (p) {
        case 1: run_problem1(); break;
        case 2: run_problem2(); break;
        case 3: run_problem3(); break;
        case 4: run_problem4(); break;
        default:
            cout << "Invalid problem number." << endl;
    }

    return 0;
}

