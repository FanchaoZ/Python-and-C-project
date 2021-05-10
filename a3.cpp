/*
#include <iostream>


int main (int argc, char **argv) {
    std::cout << "Replace with your solution for the main "
              << "driver program of Assignment 3\n";
    return 0;
}
*/

#include <stdio.h>
#include <sys/wait.h>
#include <iostream>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
using namespace std;

int main(int argc, char* argv[])
{
    int rtn;
    pid_t child[4];
    int ItoO1[2];
    pipe(ItoO1);

    int ItoO2[2];
    pipe(ItoO2);
        


    pid_t child_pid;
    child_pid = fork();
    if (child_pid == 0) {
        // redirect stdout to the pipe
        dup2(ItoO1[1], STDOUT_FILENO);
        close(ItoO1[0]);
        close(ItoO1[1]);
        execv("rgen", argv);
        
    }
    else if (child_pid < 0) {
        cerr << "Error: could not fork\n";
        return 1;
    }
    child[0] = child_pid;

    child_pid = fork();
    if (child_pid == 0) {
        // redirect stdin from the pipe
        dup2(ItoO1[0], STDIN_FILENO);
        close(ItoO1[1]);
        close(ItoO1[0]);
        dup2(ItoO2[1], STDOUT_FILENO);
        close(ItoO2[0]);
        close(ItoO2[1]);


        execl("a1ece650.py", "a1ece650.py", NULL);
    }
    else if (child_pid < 0) {
        cerr << "Error: could not fork\n";
        return 1;
    }
    child[1] = child_pid;

    child_pid = fork();
    if (child_pid == 0) {
        // redirect stdin from the pipe
        dup2(ItoO2[0], STDIN_FILENO);
        close(ItoO2[1]);
        close(ItoO2[0]);

        execl("a2ece650", "a2ece650", NULL);
    }
    else if (child_pid < 0) {
        cerr << "Error: could not fork\n";
        return 1;
    }
    child[2] = child_pid;
 
    child_pid = fork();
    if (child_pid == 0) {
        // redirect stdout to the pipe
        dup2(ItoO2[1], STDOUT_FILENO);
        close(ItoO2[0]);
        close(ItoO2[1]);
        
        while (!cin.eof()) {
            string line;
            getline(cin, line);
            if (line.size () > 0)
                cout << line << endl;
        }
        
        return 0;
    } 
    else if (child_pid < 0) {
        std::cerr << "Error: could not fork a3 loop" << std::endl;
        return 1;  
    }
    child[3] = child_pid;
    
    wait(&rtn);
    
    // send kill signal to all children
    for (int i = 0; i < 4; i ++) {
        int status;
        kill (child[i], SIGTERM);
        waitpid(child[i], &status, 0);
    }
    
    return 0; 
}
