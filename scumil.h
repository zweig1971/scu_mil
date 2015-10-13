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
		bool scu_connected;
	};


	data_scu scu_info;

	DWORD mil_status_read(WORD &mil_status);
	DWORD mil_timer_wait(WORD time);
	bool mil_write_wait(void);
	bool mil_read_wait(void);
	bool mil_test_status(WORD statusbit, DWORD &errorstatus);

	DWORD open_scu();
	DWORD close_scu();
	DWORD find_mil();

	DWORD milbus_write(WORD mildata);
	DWORD milbus_write_cmd(int cmd);
	DWORD milbus_read(WORD &mildata);
	
	DWORD event_fifo_read();
	DWORD event_timer_read();
	DWORD event_filter_read();
	DWORD event_filter_set();
	
	DWORD irq_mask_read();
	DWORD irq_mask_write();

	DWORD irq_enable();
	DWORD irq_disable();


public:

	scu_mil(void);
	~scu_mil(void);

	string scu_milerror(DWORD status);

	DWORD scu_milbusopen(const char adress[], DWORD &errorstatus);
	DWORD scu_milbusclose(DWORD &errorstatus);
	bool scu_milstatustest(WORD statusbit, DWORD &errorstatus);
	DWORD scu_timer_wait(DWORD time, DWORD &errorstatus);
	DWORD scu_milbus_write_cmd(BYTE funktionscode, BYTE ifkadresse, DWORD &errorstatus);
	DWORD scu_milbus_write_data(WORD data, DWORD &errorstatus);
	DWORD scu_milbus_read_data(WORD &data, DWORD &errorstatus);
	DWORD scu_milbus_ifk_rd (BYTE ifkadress, BYTE ifkfunktioncode, WORD &data, DWORD &errorstatus);
	DWORD scu_milbus_ifk_wr (BYTE ifkadress, BYTE ifkfunktioncode, WORD data, DWORD &errorstatus);
	bool scu_milbus_ifk_on (BYTE ifkadress, WORD &returnifkad, DWORD &errorstatus);
};
