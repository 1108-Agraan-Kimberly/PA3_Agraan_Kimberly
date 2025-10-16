sched: sched.c print_progress.c
	gcc -o sched sched.c print_progress.c -pthread
clean:
	rm -f *o sched

	
