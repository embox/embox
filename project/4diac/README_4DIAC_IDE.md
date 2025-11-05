create a new 4diac IDE project

Type library
    Import functional blocks from file system (from embox/project/4diac/FBs)

System configuration
    Add FORTE_PC (set ip address of embox)
    Add ethernet (from segments)
    Link FORTE_PC with ethernet

app
    Add events: E_CYRCLE , E_SWITCH, E_SR 
    Link E_SR:Q to E_SWITCH:G
    Add LED_OFF & LED_ON (connect from E_SWITCH to E_SR)

Deploy or System Configuration -> FORTE_PC -> Create FORTE boot file
