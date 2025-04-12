# OS_Project2
# Readers-Writers Synchronization Simulator

This project simulates real-time process execution using threads in C, demonstrating the classic **Readers-Writers synchronization problem**. It uses **POSIX threads (pthreads)** and **POSIX semaphores** to manage concurrent access to a shared resource.

## 📌 Problem Description

In the Readers-Writers problem:
- **Multiple readers** can access the shared resource at the same time.
- **Writers** require **exclusive access**—no other readers or writers can access the resource while a writer is writing.

This simulation reads input from a file and starts threads accordingly.

Each line in `processes.txt` represents a process and has the format:
- `type`: `R` for reader, `W` for writer  
- `id`: Process ID (an integer)  
- `duration`: Time in seconds that the thread simulates reading/writing

Note: Due to multithreading, the output order may vary with each run.
