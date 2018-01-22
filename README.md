# producer-consumer
## Description
This is an implementation of a version of the producer consumer synchronization problem, for more information on the phenomenon see: https://en.wikipedia.org/wiki/Producer–consumer_problem

The program takes an input file containing the student IDs, names and CGPAs of students, as well as a number associated with each record specifying the ID of the producer thread that should be reading that record from the file. Then these records are sent to a single consumer file, which sorts these arriving records with regards to CGPA and writes them to an output file.

## Run
Simply clone the repository and run ```make``` to compile all the necessary files. Then you can run pcsync with the following parameters: 

```pcsync <buffersize> <producer_count> <input_file> <output_file>```

where:

 <b>buffersize</b> determines the maximum size of the bounded buffers between each producer and the consumer. The maximum value for this buffer is 1000.

 <b>producer_count</b> specifies the amount of producers that will read the input file. The default value for this parameter is determined by performing an initial pass on the input file and reading the producer IDs associated with the records. 

 <b>input_file</b> should be in the following format: 
 
 ```<producerid> <sid> <firstname> <lastname> <cgpa>```

for each row record contained within.
