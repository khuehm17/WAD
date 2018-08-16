-- WHEELCHAIR ACCIDENT DETECTOR Instruction --
 * Define Wheelchair ID in AccidentAlert File.


-- WHEELCHAIR Workflow --
      Start
  	|-- Initialization
  	|-- Check Sensor 
  	|-- Loop:
              |-- Read Sensor Data
	      |-- Detect Accident Event
	      	|-- Send Warning Signal
		|-- Buzzer ON
	      |-- Check Safe Button Status
		|-- Send Safe Signal
			

** Note:
|-- Interrupt can be used on any digital pin
