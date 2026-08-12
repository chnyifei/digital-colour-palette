# Digital Color Palette

note: add demo gif after completion

> **Why I Built This:** When working with polymer clay, color matching concept art usually requires tedious trial-and-error that wastes both time and material, so I built this desktop lookup tool to simulate real-world pigment blending on an SPI display. It calculates exactly how much of each base I need, so I can get my target colour in one go.

## **Executive Summary**

note: add 2-3 sentences describing; list all hardware; 

## **Key Challenges and Insights**

note: add 3-5 bullet points on the most challenging parts
- combining colours with a less precise data type causes quantization loss; solution: convert to 888
- additive colours (digital) and subtractive colours (physical pigments) don't behave the same way; iternation 1: tried fitting to x type curve, used in games, iteration 2: tried fitting to log curve (based on assumption); iteration 3: manually selected for data points and gave each pigment a general strength (REFER TO THESE ITERNATIONS HERE; SHOW THEM IN DETAILS LATER)

## **Deep Dive and System Architecture**

### FSM Diagram

note: add short description and diagram

### Part-by-Part Building Process

> Note: this project is being transferred Github near its completion, so only the last version of the independent programs and the final combined project (containing slightly altered versions of those programs) are included in this repository. However, snippets of older code are included in the detailed breakdown below in order to illustrate the process.

<physical_calc> </physical_calc>
<joystick_input> </joystick_input>
<display_colour> </display_colour>
<UI_design> </UI_design>
<main_loop> </main_loop>

<physical_assembly> </physical_assembly>
<CAD_casing> </CAD_casing>

### Closing Thoughts and Note to Clay Artists