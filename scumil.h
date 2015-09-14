// GSI Gesellschaft für 
// Schwerionenforschung mbH, 
// Darmstadt, Germany 
//
// scumil.h
//
// Autor           : Zweig,Marcus
// Version         : 0.1
// letzte Änderung : 31.08.15
//------------------------------


#include <iostream>
#include <stdio.h>
#include <etherbone.h>
#include "scumil_def.h"
#include <string.h>
#include <cstring>

using namespace std;

class scu_mil
{
private :

	struct data_scu
	{
		char scu_adress[30];
		eb_socket_t socket;
		eb_device_t device;
		eb_address_t mil_base;
	};


	data_scu scu_info;

	int open_scu();
	int close_scu();
	int find_mil();

	int fct_send();
	int milbus_write();
	int milbus_read();
	
	int event_fifo_read();
	int event_timer_read();
	int event_filter_read();
	int event_filter_set();
	
	int irq_mask_read();
	int irq_mask_write();

	int irq_enable();
	int irq_disable();


public:

	scu_mil(void);
	~scu_mil(void);

	string scu_milerror(int status);

	int scu_milbusopen(const char adress[], int &errorstatus);
	int scu_milbusclose(int &errorstatus);
};
