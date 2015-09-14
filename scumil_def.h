//-----------------------------
// GSI Gesellschaft für 
// Schwerionenforschung mbH, 
// Darmstadt, Germany 
//
// PCIMilTreiber.h
//
// Autor           : Zweig,Marcus
// Version         : 1.09
// letzte Änderung : 13.02.04
//------------------------------

// mil information zur bestimmung der baseadress auf der scu
// --------------
#define vendor  0x0000000000000651
#define product 0x35aa6b96

// Fehlermeldungen 
//----------------
#define StatusOK		0x0
#define SocketOpen_Error	0x1
#define DeviceOpen_Error	0x2
#define DeviceClose_Error       0x3
#define SocketClose_Error       0x4
#define BaseAdress_Error        0x5 
