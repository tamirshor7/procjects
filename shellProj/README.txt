Linux Small Shell (Smash) Project
Implementation of Unix shell - This projects implements various Unix bash commands:
	chprompt 	- Change prompt text in shell
	showpid  	- Present pid of current shell process
	pwd      	- Print full path to current working directory
	cd       	- Change directory
	jobs     	- Print the job list, containing all unfinished jobs (running in the background) and all stopped (received SIGSTP) jobs.
			   	  Jobs are printed in the following format: [<job-id>] <command> : <process id> <seconds elapsed> (stopped).
	kill     	- Sends a signal which its number specified by <signum> to jobs which its sequence ID in jobs list is <job-id> (same as job-id in jobs command) 
                  and prints a message reporting that the specified signal was sent to the specified job.
	fg       	- Brings a stopped process or a process that runs in the background to the foreground.
	bg       	- Resumes a stopped process in background. Process is given by job-id. If no argument is passed, the last stopped job would be selected.
	quit     	- Exits the smash and deallocated resources. If the kill argument was specified, smash would kill 
			      all of its unfinished and stopped jobs before exiting.
	cat      	- Output all files given to standard output.
	redirection - <command1> > <file1> redirects the output of command1 to file1, truncating any existing content in file1.
				  <command1> >> <file1> would behave similiarly, but would append to the contents of file1.
	pipe        - <command1> | <command2> - using the pipe character “|” will produce a pipe that redirects command1 stdout 
	              to its write channel and command2 stdin to its read channel.
				  <command1> |& <command2> would behave similiarly, but would recirect command1's stderr to the write channel, instead of its stdout.
	timeout     - timeout <duration> <command> sets an alarm for ‘duration’ seconds, and runs the given ‘command’ as though it was
				  given to the smash directly, and when the time is up it shall send a SIGKILL to the given command’s process (unless it’s the smash itself).
	
	All other shell commands are supported by redirection to the Linux bash.
	
	Signal Handling: The project implements handling of SIGINT (Ctrl+C), SIGSTP(Ctrl+Z) and SIGALARM. Other signals' handling is redirected to default
					 (not implemented by the project).



	           