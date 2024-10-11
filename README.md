Here's a revised README without the "homework assignment" tone:

---

# Linux Pipes and Multistage Process Control

This repository contains C++ programs demonstrating the use of Linux pipes, process creation, and inter-process communication. The programs mimic the functionality of shell pipelines, connecting multiple processes using pipes and performing a variety of operations.

## Files in This Repository

### 1. **`pipes.cc`**
   - **Description**: Implements a simple two-stage pipeline where the output of one process is passed to another through a pipe.
   - **Functionality**:
     - The first child process executes the `ps -aux` command, redirecting its output to the write end of a pipe.
     - The second child process reads from the pipe and executes the `sort -r -n -k 5` command, sorting the process list by memory usage.
     - The parent process manages both child processes and waits for them to complete.

### 2. **`ops.cpp`**
   - **Description**: A program that performs basic arithmetic and comparison operations on input data passed through standard input.
   - **Functionality**:
     - Accepts an operator (e.g., `+`, `x`, `le`) and a value as command-line arguments.
     - Reads floating-point numbers from standard input, applies the given operation, and outputs the result.
     - Can be chained in pipelines to perform operations on streams of data.

### 3. **`runpipe.cpp`**
   - **Description**: A program that replicates shell-like behavior by parsing and executing a command line with multiple stages connected by pipes.
   - **Functionality**:
     - Reads a user-inputted pipeline command (e.g., `cat file.txt | ./ops + 10 | ./ops x 2`).
     - Sets up pipes between the different stages, forks child processes, and executes each command in the pipeline.
     - Manages the execution and waits for all processes to complete before exiting.

## Usage

1. **Compile**:
   ```bash
   g++ -o pipes pipes.cc
   g++ -o ops ops.cpp
   g++ -o runpipe runpipe.cpp
   ```

2. **Running `pipes.cc`**:
   ```bash
   ./pipes
   ```

3. **Running `ops.cpp`** with piped commands:
   ```bash
   echo 20 | ./ops + 10
   ```

4. **Running `runpipe.cpp`** for multistage commands:
   ```bash
   ./runpipe
   Enter pipe command: cat numbers.txt | ./ops + 10 | ./ops x 2
   ```

---

