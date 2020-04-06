This is an auxiliary tool to generate code for implementation of new STEP entities into OCCT.

Example how to run ExpToCas tool:
ExpToCasExe.exe schemaAP000.exp to_add.lst occt_step_entities.lst N

where
* schemaAP000.exp - STEP schema, some lines in can be not parsed, just delete them.
ap242.exp - cleaned schema for AP242 edition 2.
* to_add.lst  - list of necessary entities, each line should be like this "ShapeAspect StepRepr".
* occt_step_entities.lst - list of already implemented entities, please update the file after each generation.
* N - a first index for entity registration (files StepAP214_Protocol.cxx, RWStepAP214_GeneralModule.cxx,RWStepAP214_ReadWriteModule.cxx)

The result of generation consists of several folders:
* two folders for each package for which new entities was generated.
* a folder Registration with *.txt files with code to copy into common files (see description of last parameter).