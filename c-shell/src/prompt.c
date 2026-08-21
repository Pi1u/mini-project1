#include <stdio.h>
#include <unistd.h>
#include <pwd.h>
#include <string.h>
#include <stdlib.h>
#include "prompt.h"

void displayPrompt(void) 
{
    char hostname[256];
    char cwd[1024];
    struct passwd *pw;
    
    //Get the User ID and Password struct
    pw = getpwuid(getuid());
    if (pw == NULL) 
    {
        perror("Failed to get user info");
        return;
    }

    //Get the Hostname
    if (gethostname(hostname, sizeof(hostname)) != 0) 
    {
        perror("Failed to get hostname");
        return;
    }

    //Get the Current Working Directory
    if (getcwd(cwd, sizeof(cwd)) == NULL) 
    {
        perror("Failed to get cwd");
        return;
    }

    char *home_dir = pw->pw_dir;
    size_t home_len = strlen(home_dir);

    if (strncmp(cwd, home_dir, home_len) == 0 && 
       (cwd[home_len] == '\0' || cwd[home_len] == '/')) 
    {
        printf("<%s@%s:~%s> ", pw->pw_name, hostname, cwd + home_len);
    } 
    else printf("<%s@%s:%s> ", pw->pw_name, hostname, cwd);
}