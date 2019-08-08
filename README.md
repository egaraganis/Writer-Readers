## Writer-Readers
The writer readers problem that is solved is the following.
![Problem](https://github.com/VangelisGara/Writer-Readers/blob/master/images/Selection_006.png)
The feeder process will fill an internal array with size M ( M > 3000 ) with random integers.
This array should be represented with the exact same format by n processes. The shared memory segment allows to story only one integer at the time, along with the timestamp of the time that the integer was inserted.

Each Ci process will analyze the timestamp and calculate the delay it took  for the process to read the integer from the shared memory. Every process will store the running average that are summing up from the M reads delays.

When finished, the last process filling its array, will print to a file the sequence of the array and will print to the screen its PID along with the running average.

## To compile and execute

    make clean & make
    ./ipc -n [number of processes] -m [size of array] -d [optional, for debug mode]

