This ARL meteorological profile conversion software (ARL_MET-profile_Converter version 1.0.0) is designed to aid in the analysis of meteorological model output and various types of measured soundings such as radiosonde observations.   Input and output samples may be found in the sub-directory ‘Input_Output_Samples’.

The software produces profiles of meteorological variables for levels and layers as defined by user input height or pressure levels.  It can process input profiles from sensing systems such as radiosonde, lidar, or wind profiling radar, and from profiles derived from numerical weather model output such as from the Weather Research and Forecasting model. Some changes to the input routine may be required for different input types and formats, and in a few cases modest changes in one or more other routines may be needed. 
 
This software is is covered by the ARL CC0 1.0 Universal (CC0_1.0) Public Domain Dedication and the Contributor License Agreement (CLA) available at the ARL GitHub site (https://github.com/USArmyResearchLab/ARL-Open-Source-Guidance-and-Instructions). 

The software has two primary types.  One produces height based level and layer output and the other pressure based level and layer output.  For Weather Research and Forecasting (WRF) model derived atmospheric profiles each type uses a common input routine.  For radiosonde observation (RAOB) soundings from the University of Wyoming weather web site each type of software has a common input routine.  For RAOBs from the NOAA archive site each type has a common input routine.  In addition to the input routine, use of RAOB data from the NOAA archive required a modification to one other routine.  The total number of executables is 6 as of June 2017. 

The software is written in standard ASCII “C” and should compile on a wide variety of systems.   It may be compiled via a simple command using a file that contains the necessary compile code; it uses the standard GNU "C" compiler supplied with many operating systems.  For example, ccmsg_wyo compiles the code for height based level and layer output from a RAOB obtained from the University of Wyoming website producing the executable convertwyo.  The code for this example follows:

gcc -g -lm -o convertwyo convertdatawyo.c msgvalues.c initdata.c \
level.c layer.c clean_data.c tvfromtemp.c prscomp.c readwyo.c \
writeusrmsg.c writeusrlvls.c checkdata.c

The other executables have similar compile codes.  The other executables are:

convertwrf - produces height based level and layer output from soundings derived from WRF output,
convertnoaa - produces height based level and layer output from NOAA archive RAOBs,
convertprswyo - produces pressure based level and layer output from the University of Wyoming RAOBs,
convertprswrf - produces pressure based level and layer output from soundings derived from WRF output,
convertprsnoaa - produces pressure based level and layer output from NOAA archive RAOBs,

The 6 programs require a "lvls" file where lvls is the last part of the name of the file containing the user supplied height or pressure levels (usrhgt_lvls or usrprs_lvls).  They also need a parameter file (i.e., input_parameters) with the paths of the input and output directories.  They may be the same or different, but both need to be listed.   Here we present the procedures to use the programs once the lvls and input_parameters files are in place.  Those files should be in the same directory as the executable.    Sample versions of the _lvls and input_paramters files are included in the same directory as the source code files.

1.  Height based method:
       For RAOBs from the U of Wyoming type and return the command line ./convertwyo 'input file name' (e.g., ./convertwyo RAOB_ABC_20140520).
       For RAOBs from the NOAA archive type and return the command line ./convertnoaa 'input file name' (e.g., ./convertnoaa RAOB_XYZ_20140520).         
       For WRF generated soundings type and return the command line ./convertwrf 'input file name' (e.g., ./convertwrf WRF_ABC_20140520).
 
2.  Pressure based method:
       For RAOBs from the U of Wyoming type and return the command line ./convertprswyo 'input file name' (e.g., ./convertprswyo RAOB_ABC_20140520).
       For RAOBs from the NOAA archive type and return the command line ./convertprswyo 'input file name' (e.g., ./convertprsnoaa RAOB_XYZ_20140520).
       For WRF generated soundings type and return the command line ./convertprswrf 'input file name' (e.g., ./convertprswrf WRF_ABC_20140520).

The programs have run on both Linux and Windows operating systems.  For use with the Windows "Command Prompt" and some Linux systems the "./" before the executable is not used.  For example, the syntax for an instance of the convertwrf program could be convertwrf WRF_ABC_2016050512 vs. ./convertwrf WRF_ABC_2016050512.

The function checkdata (file name = checkdata.c) allows the user to check the input or output data at an intermediate stage.  At present it's commented out where it appears.  To activate "uncomment" it, or if not at a specific location in the code add as appropriate.  To stop the program after the use of checkdata add the statement "exit(0);" immediately afterwards.   The output appears in the file “datacheck” and has some, but not all, variables normally in the output data structure.    A sample may be found in the directory with the source code. 

Based on user experiences at ARL, a technically trained person with no previous knowledge of any of the programs should be able to run it within 5 to 10 min.
