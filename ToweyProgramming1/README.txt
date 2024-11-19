Name: Jaden Towey

Student ID: 2420751

Chapman email: towey@chapman.edu

Course: CPSC380

Section: 02

Assignment: Programming Assignment 1

All Files: 
fops_test.c
fops.h
libfops.c 
fops.o
fops.a 
output.text

Sources:
    - https://www.geeksforgeeks.org/input-output-system-calls-c-create-open-close-read-write/ 
    - chatGPT

To Run:
gcc -c libfops.c -o fops.o
ar rcs libfops.a fops.o
gcc fops_test.c -L. -lfops -o fops_test

./fops_test create sample.txt
./fops_test write sample.txt "Hello, this is a test."
./fops_test read sample.txt
./fops_test delete sample.txt

To get output.txt:
{
    ./fops_test create sample.txt
    ./fops_test write sample.txt "Hello, this is a test."
    ./fops_test read sample.txt
    ./fops_test delete sample.txt
} > output.txt

Output Text:
Read: Hello, this is a test.
