#include "systemcalls.h"
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <string.h>
#include "errno.h"

#define _XOPEN_SOURCE

/**
 * @param cmd the command to execute with system()
 * @return true if the command in @param cmd was executed
 *   successfully using the system() call, false if an error occurred,
 *   either in invocation of the system() call, or if a non-zero return
 *   value was returned by the command issued in @param cmd.
*/
bool do_system(const char *cmd)
{
/*
 * TODO  add your code here
 *  Call the system() function with the command set in the cmd
 *   and return a boolean true if the system() call completed with success
 *   or false() if it returned a failure
*/
    return ((0 == system(cmd))? true : false);
}

/**
* @param count -The numbers of variables passed to the function. The variables are command to execute.
*   followed by arguments to pass to the command
*   Since exec() does not perform path expansion, the command to execute needs
*   to be an absolute path.
* @param ... - A list of 1 or more arguments after the @param count argument.
*   The first is always the full path to the command to execute with execv()
*   The remaining arguments are a list of arguments to pass to the command in execv()
* @return true if the command @param ... with arguments @param arguments were executed successfully
*   using the execv() call, false if an error occurred, either in invocation of the
*   fork, waitpid, or execv() command, or if a non-zero return value was returned
*   by the command issued in @param arguments with the specified arguments.
*/

bool do_exec(int count, ...)
{
    va_list args;
    va_start(args, count);
    char * command[count+1];
    int ret_value = false;
    int i;
    int child_pid = -1;
    int status = 0;

    for(i=0; i<count; i++)
    {
        command[i] = va_arg(args, char *);
    }
    command[i] = NULL;

/*
 * TODO:
 *   Execute a system command by calling fork, execv(),
 *   and wait instead of system (see LSP page 161).
 *   Use the command[0] as the full path to the command to execute
 *   (first argument to execv), and use the remaining arguments
 *   as second argument to the execv() command.
 *
*/

/* Create a new process. 
 * fork() returns 0 in case of success to the child 
 *      and the child's PID to the parent. */
    child_pid = fork();
    if (-1 == child_pid) {        // Could not create a child process.
        perror("fork: ");   // The reason.
        return false;        
    } else if (0 == child_pid) {    // Are we in the child process?
        char *const argv[4] = {
            command[0], command[1],
            command[2], NULL
        };
        ret_value = execv(argv[0], argv);    // Load the binary of the child process.
        perror("execv");

        exit(ret_value);   // Return failure if execv returned as it should not return on success!
    } else {    // The parent process code
    // Wait for the child process execution before terminating the parent  
        ret_value = waitpid(child_pid, &status, 0);
        if (-1 == ret_value) {   // waitpid() failed? 
            perror("waitpid: ");
            ret_value = false;
        } else if (0 != WIFEXITED(status)) {  // Succeeded.            
            if (0 == WEXITSTATUS(status)) {   // Process exited normally & the command ran by execv succeeded!
                ret_value = true;
            } else {            // Command ran by execv failed!
                ret_value = false;  
            }
        }  
    }
    va_end(args);

    return ret_value;
}

/**
* @param outputfile - The full path to the file to write with command output.
*   This file will be closed at completion of the function call.
* All other parameters, see do_exec above
*/
bool do_exec_redirect(const char *outputfile, int count, ...)
{
    va_list args;
    va_start(args, count);
    char * command[count+1];
    int i;
    unsigned short child_pid = 0;
    int status = -1, ret_val = -1;

    for(i=0; i<count; i++)
    {
        command[i] = va_arg(args, char *);
    }
    command[i] = NULL;

/*
 * TODO
 *   Call execv, but first using https://stackoverflow.com/a/13784315/1446624 as a refernce,
 *   redirect standard out to a file specified by outputfile.
 *   The rest of the behaviour is same as do_exec()
 *
*/
    child_pid = fork();
    if (-1 == child_pid) {
        perror("fork");
        return false;
    } else if (0 == child_pid) { // child's code
        int fd = -1;
        char *argv[4] = {
            command[0], command[1],
            command[2], command[3]
        };
        fd = open(outputfile, O_RDWR  | 
                              O_CREAT | 
                              O_TRUNC, S_IRWXU);
        if (-1 == fd) {  // Failed to open the file?
            perror("open");
            return false;
        }     
    // Redirect the output of the standard out to outputfile.                      
        fd = dup2 (fd, STDOUT_FILENO); // Redirects STDOUT to outputfile
        if (-1 == fd) {    // Failed to redirect the file?
            perror("dup2"); 
            return false;
        }
        ret_val = execv(argv[0], argv);
        perror("execv");

        exit(ret_val);
    } else {  // Parent's code
        ret_val = waitpid(child_pid, &status, 0);
        if (-1 == ret_val) {
            perror("waitpid");
            ret_val = false; // child process failed.
        } else if (0 != WIFEXITED(status)) { // normal termination?
            if (0 == WEXITSTATUS(status)) {
                ret_val = true;     // Success.
            } else {    
                ret_val = false;    // Failure.
            }
        }
    }
    va_end(args);

    return ret_val;
}
