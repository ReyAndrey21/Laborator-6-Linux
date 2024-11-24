#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <vector>
#include <cmath>
#include <fcntl.h>

using namespace std;


bool isPrime(int num) {
    if (num <= 1) return false;
    for (int i = 2; i <= sqrt(num); i++) {
        if (num % i == 0) return false;
    }
    return true;
}


void findPrimesInRange(int start, int end, int writePipe) {
    vector<int> primes;
    for (int i = start; i <= end; i++) {
        if (isPrime(i)) {
            primes.push_back(i);
        }
    }

    
    write(writePipe, primes.data(), primes.size() * sizeof(int));
    close(writePipe);
}

int main() {
    const int numProcesses = 10;
    const int rangeSize = 1000;
    const int maxNumber = 10000;
    vector<int> pipes(numProcesses * 2); 

    
    for (int i = 0; i < numProcesses; i++) {
        int pipefd[2];
        if (pipe(pipefd) == -1) {
            perror("pipe");
            return 1;
        }
        pipes[i * 2] = pipefd[0]; 
        pipes[i * 2 + 1] = pipefd[1]; 
    }

    
    for (int i = 0; i < numProcesses; i++) {
        pid_t pid = fork();

        if (pid == 0) {  
            
            int start = i * rangeSize + 1;
            int end = (i + 1) * rangeSize;
            if (end > maxNumber) end = maxNumber;

            
            findPrimesInRange(start, end, pipes[i * 2 + 1]);
            exit(0);  
        }
    }

    
    for (int i = 0; i < numProcesses; i++) {
        wait(NULL); 

        
        vector<int> primes;
        int prime;
        ssize_t bytesRead;
        while ((bytesRead = read(pipes[i * 2], &prime, sizeof(int))) > 0) {
            primes.push_back(prime);
        }

        
        cout << "Procesele pentru intervalul " << i * rangeSize + 1 << " - " << (i + 1) * rangeSize << " au gasit urmatoarele numere prime:\n";
        for (int prime : primes) {
            cout << prime << " ";
        }
        cout << endl;
        close(pipes[i * 2]);
    }

    return 0;
}
