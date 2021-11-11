## ECE376 - Homework #9  
### Problem Statements  
#### Timer1 Capture
Problem 1-5) Use Timer1 Capture to measure time to 1 clock (100ns). Some options are:  
  - Reflex Timer: Measure your reflex time with a resolution of 100ns  
    - Start the game by pressing RB1.  
    - 3-7 seconds later, the lights on PORTA turn on (time recorded with Capture1)  
    - Press RB0 as soon as the lights turn on (time recorded with Capture 2)  
    - The time difference is your reflex time  
  - Capacitance Sensor: Measure C using the circuit below. Use Timer1 Captuer to measure the period (T) to 100ns. From that, compute C.  
  - Temperature Sensor: Use the circuit below to measure temperature. Use Timer1 Capture to measure the period (T) for the circuit, precise to 100ns. From that, compute R2 and the temperature in degrees C. Let  
    - R1 = 1k Ohm resistor  
    - R2 = a Thermistor (1k or 10k at 25C)  
    - C = 10uF  
1. Requirements: Define the inputs, outputs, and how they relate.
Part of the requirement must be to measure time to 100ns (i.e. use Timer1 Capture interrupts)
2. C-Code and flow chart.
3. Test: Collect data in lab to verify that your interrupts are working properly.
4. Validation: Collect data to validate your design works.
5. Demo  

##### My Demo link: https://youtu.be/Y_mhbS2GqI0  

#### Timer1 Compare
Problem 6-10) Use Timer1 Compare to output precise frequencies. Some suggestions are:  
  - Precise 8 key piano: Play notes A3..A4 on RC0 when you press buttons RB0..RB7  
  - Electronic Trombone: Play note A3 (A2D=0) to A4 (A2D = 1023) on RC0 when you press RB0  
  - Input a number from 100 to 9999 with a keypad. Output that frequency on RC0  
Other...  
6. Requirements: Define the inputs, outputs, and how they relate. Part of the requirement must be to measure time to 100ns (i.e. use Timer1 Capture interrupts)
7. C-Code and flow chart.
8. Test: Collect data in lab to verify that your interrupts are working properly.
9. Validation: Collect data to validate your design works.
10. Demo  

##### My Demo link: https://youtu.be/dsG4ypv3q6Q