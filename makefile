CC=gcc

IPC_Program: IPC_Program.o IPC_Functions.o
	$(CC) -o ipc IPC_Program.o IPC_Functions.o
.PHONY: clean
clean:
	@rm -f ipc *.o core
	@rm REPORT.txt