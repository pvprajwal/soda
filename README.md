# soda
SODA (Simulation Of Diffusion &amp; Automata) explores using GPGPU to simulate cellular automata and reaction-diffusion systems. 

# Features:

-> Grid size (N) and total iterations (iter) can be changed in code.  

-> Uses Ping-Pong buffering to minimize PCIe data transfer.

-> 10Hz throttled display for human readability using sleep time for cpu thread.

-> The sleep delay can be adjusted in milliseconds and can be removed as well.

# Limitations:

Input: Currently hardcoded for a "Blinker" pattern (starting at row 5).

Display: Primitive console output using + for alive and . for dead cells.