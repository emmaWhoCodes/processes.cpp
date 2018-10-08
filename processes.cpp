#include <iostream>
#include <unistd.h>
#include <sys/wait.h>


using namespace std;


// Program used to mimic terminal command:  'ps -A | grep | wc -l'
int main(int argc, char * argv[]) {
    pid_t pid; //process id

    int filedescriptor[2]; // file descriptors created
    int status;
    int read = 0; // reading data from a pipe index
    int write = 1; //writing data to a pipe index


    if (pipe(filedescriptor) < 0) { //Pipe created
        perror("pipe error");
    }

    if ((pid = fork()) < 0) {
        perror("fork error");
        return EXIT_FAILURE;

    } else if (pid == 0) { //child

        if ((pid = fork()) < 0) {
            perror("fork error");
            return EXIT_FAILURE;

        } else if (pid == 0) { //grand child

            if ((pid = fork()) < 0 ) {
                perror("fork error");
                return EXIT_FAILURE;

            } else if (pid == 0) { //great grand child
                execlp("ps", "ps", "-A", NULL);

            } else { //grandchild else

                execlp("grep", "grep", argv[1], NULL);

            }

        } else { //child else

            execlp("wc", "wc", "-l", NULL); // pipe back to parent
        }
    } else { //parent else
        wait(&status);

    }
    return 0;
}


