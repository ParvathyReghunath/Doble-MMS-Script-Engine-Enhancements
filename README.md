# Doble-MMS-Script-Engine-Enhancements

Doble Engineering Company wants to enhance their MMS Script Engine and integrate the changes to their Protection Suite. 

# Project Overview

MMS Script Engine enhancement project is a plugin for already existing Protection Suite marketed by  Doble Engineering. To develop the additional enhancements of the project and integrate it to Protection Suite, Doble is engaging with Kalkitech. 

SalesForce ID: 31571
Project code: SVSV23240044
Defecting Tracking System: Doble JIRA
# Scope of work
The project is to do the necessary design, development, testing and integration of enhancement required in MMS Script Engine for the upcoming release version 3.1.6.

The list of items identified for the release from Doble’s JIRA DMS are –

No.	JIRA Ticket	Description

1.	GSE-1561	Remove hard-coded delays built into the algorithm. It takes almost 30 seconds to fail, when the tool attempts to connect to an IED that is not available in the network. Connection attempts to be made parallel to address this issue as the current round robin approach creates a lot of delay when there are configured IEDs unavailable in the network.
2.	GSE-1564	Progress report to be provided from MMS Script Engine to keep the user informed of the system status after being invoked by Protection Suite.
3.	GSE-1647	Fix the case where MMS Script Engine strips data name prefix in the output file. This problem, which was originally reported in version 5 of the MMS engine (see GSE-1636),was fixed in version 6, but has re-appeared in version 7. See the input and output files attached to GSE-1647 where the MON/DRPRDRE1.CO.Mod element's result is reported as DRPRDRE1$CO$Mod.
4.	GSE-1646	The MMS engine needs the ability to set the test field of the Oper data object. It seems feasible that the engine could make this happen automatically be asserting that field whenever it is setting the Sim data object and the target's associated Mod Beh is either test or test-blocked, but the more serviceable solution going forward is probably to add an input parameter to the executable(s) that allows the calling code to force this behavior when needed.
5.	GSE-1565	The program should return numeric error codes to the caller (0 on success). Right now, it just prints a message to its output file or to the console.
6.	GSE-1563	When running against a simulate bit, MMS Script Engine echoes its command line arguments in its output. Running against a test mode control point, it does not. Since Protection Suite code parses the output of the program, it would be convenient to have a single consistent output format regardless of control point type. Add a command line option with which the caller may turn argument echoing on/off.
7.	GSE-1562	The OutputFile parameter seems to require that the file name end in a .TXT extension. The program fails silently with no indication of the reason when tried with files of other extension (like .TMP). The program shall allow any arbitrary file extension, or it should indicate an error on failure.
8.	GSE-1471	Add Test parameter to the control command functionality to Python MMS Engine (This is more for use with Protection Suite).
The product proposal and offer are available with the Salesforce (SF ID: 31571). 
This project code is SVSV23240044.
