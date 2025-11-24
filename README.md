## **Notes**

- Output interleaving is expected due to multi-threaded printing.
- All semaphore logic follows the patterns from *The Little Book of Semaphores*.
- Tested on macOS using POSIX pthreads and semaphores.

Problem 1 output 
This solution prevents starvation by using a **turnstile semaphore** that enforces fairness and a **lightswitch-style reader counter** controlling access to the shared resource.
<img width="871" height="756" alt="Screenshot 2025-11-24 at 3 54 45 PM" src="https://github.com/user-attachments/assets/2eda1267-aed7-4788-b447-e59a7bac088d" />

Problem 2 output  
This solution gives **priority to writers** by having readers wait on a `readTry` semaphore whenever a writer is waiting. Writers enter exclusively and readers only enter when no writers are queued.
<img width="779" height="771" alt="Screenshot 2025-11-24 at 3 54 25 PM" src="https://github.com/user-attachments/assets/aa8f4cdd-3154-418a-b213-269f3b31d4f9" />

Problem 3 output 
Deadlock is prevented through **asymmetry**:  
- Even philosophers pick up LEFT → RIGHT  
- Odd philosophers pick up RIGHT → LEFT  

This breaks circular wait and ensures progress.

<img width="854" height="1077" alt="Screenshot 2025-11-24 at 3 54 10 PM" src="https://github.com/user-attachments/assets/63d9d02c-4f2f-4fb3-b3d0-5f98c91e2813" />

Problem 4 output 
The footman semaphore limits table access to **4 philosophers at a time**, preventing deadlock by removing the possibility that all 5 hold one chopstick simultaneously.

<img width="805" height="1072" alt="Screenshot 2025-11-24 at 3 53 50 PM" src="https://github.com/user-attachments/assets/5c6d9c77-7ddf-4b23-af1a-36e3954a6108" />
Compile: make
run with any:
./cse4001_sync 1
./cse4001_sync 2
./cse4001_sync 3
./cse4001_sync 4
