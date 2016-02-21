Process to run the Tiny SQL interpreter.

1. Download the C++code folder and unzip.

2. On the command line navigate to the path where the C++ code is unzipped.

3. Execute the following commands.
a. make clean
b. make all
c. ./Main_DB

4. Following options are given to the user
**************************************
[1] Read TinySQL Queries from File
[2] Read TinySQL Query from Command Prompt
[3] Exit
>> Please Enter an Option (number): 

Press 1 to enter a text file. 2 if you want to enter a SQL query on command and 3 
if you decide to quit.

5. If I give option and press enter following is seen
>> Please Enter an Option (number): 1
>> Please Enter the TinySQL Query File Name: 

For the purpose of testing I have used 2 scripts
SQL_textscript and SQL_textscript1. User can create his or her scripts and give 
the Query File name. Make sure you give whatever is the extension of the file
while you input the file at this step.

6.Once I give the Query File input name, following options are given
>> Please Enter the TinySQL Query File Name: SQL_textscript1
[1] Print Output to Screen
[2] Print Output to File (query_output)
>> Please Enter an Option (number): 

If you opt for option 1, the results are stored in a fixed file called query_output
If you choose option2, results are seen on the screen
At any point of time you can either output results on the screen or the output file

7. Finally Once the execution of the query file completes the total time taken to execute the query file is displayed
along with the following options. Continue if required or choose option 3 to exit

Time Taken to compute the complete input in seconds: 415.858
**************************************
[1] Read TinySQL Queries from File
[2] Read TinySQL Query from Command Prompt
[3] Exit
>> Please Enter an Option (number): 

This completes the process

