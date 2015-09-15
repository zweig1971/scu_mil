// Darmstadt, Germany 
//
// scumil.cpp
//
// Autor           : Zweig,Marcus
// Version         : 0.1
// letzte Änderung : 31.08.15
//------------------------------

#include "scumil.h"
#include "scumil_def.h"

scu_mil::scu_mil(void)
{
  scu_info.socket = 0;
  scu_info.device = 0;
  scu_info.mil_base = 0;
}

scu_mil::~scu_mil(void)
{
}

// liest das mil-statusregister
// ---------------------
int mil_status_read()
{
}

// zieht den timer auf der scu/mil
// auf in us & kommt erst nach ablauf 
// zurueck
// ---------------------
void mil_timer_wait()
{
}

// Prüft ob MilBus wieder bereit ist, dabei
// wird ein Timer aufgezogen
// ---------------------
bool mil_write_wait(void)
{
}

// Prüft ob MilBus wieder bereit ist, dabei
// wird ein Timer aufgezogen
// ---------------------
bool mil_write_read(void)
{
}

// oeffnet ein socket & die device
// ---------------------
int scu_mil::open_scu()
{
	eb_status_t status;

	// open socket
	if ((status = eb_socket_open(EB_ABI_CODE,0, EB_ADDR32|EB_DATA32, &scu_info.socket)) != EB_OK)
	return SocketOpen_Error;

	// open device
	if ((status = eb_device_open(scu_info.socket,scu_info.scu_adress, EB_ADDR32|EB_DATA32, 3, &scu_info.device)) != EB_OK)
	return DeviceOpen_Error;

	return StatusOK;
}

// schliesst das socket & die device
// ---------------------
int scu_mil::close_scu()
{
	eb_status_t status;

  	// close device
	if((status = eb_device_close(scu_info.device)) != EB_OK)
	return DeviceClose_Error;
  
 	// close socket
  	if((status = eb_socket_close(scu_info.socket)) != EB_OK)
	return SocketClose_Error;

	return StatusOK;
}

// sucht die baseadresse vom milbus
// auf der device & gibt sie zurück
// ---------------------
int scu_mil::find_mil()
{
	eb_status_t status;
	struct sdb_device sdbDevice;

	int nDevices = 1;
	sdbDevice.sdb_component.addr_first = 0;
	
	if ((status = eb_sdb_find_by_identity(scu_info.device, vendor, product, &sdbDevice, &nDevices)) != EB_OK)
	return BaseAdress_Error;

	scu_info.mil_base = sdbDevice.sdb_component.addr_first;

        return StatusOK;
}

// schreibt einen funktionscode auf
// den milbus
// ---------------------
int scu_mil::fct_send()
{
        return 0;
}

// schreibt ein datum auf den milbus
// ---------------------
int scu_mil::milbus_write()
{
        return 0;
}

// liest ein datum vom milbus
// ---------------------
int scu_mil::milbus_read()
{
        return 0;
}

// liest das eventfifo der scu aus
// ---------------------        
int scu_mil::event_fifo_read()
{
        return 0;
}

int scu_mil::event_timer_read()
{
        return 0;
}

int scu_mil::event_filter_read()
{
        return 0;
}

int scu_mil::event_filter_set()
{
        return 0;
}
        
int scu_mil::irq_mask_read()
{
        return 0;
}

int scu_mil::irq_mask_write()
{
        return 0;
}

int scu_mil::irq_enable()
{
        return 0;
}

int scu_mil::irq_disable()
{
        return 0;
}

string scu_mil::scu_milerror(int status)
{
        string ErrorMessage;

        switch (status)
        {
        case StatusOK:
                ErrorMessage = "Status OK";
                break;
        case SocketOpen_Error:
                ErrorMessage = "Open socket failure";
                break;
        case DeviceOpen_Error:
                ErrorMessage = "Open device failure";
                break;


	}
	return ErrorMessage;
}


int scu_mil::scu_milbusopen(const char adress[], int &errorstatus)
{

  int status = 0;

  strcpy (scu_info.scu_adress, adress);

  status=open_scu();

  if (status != StatusOK) {
    errorstatus = (errorstatus | status);
    return status;
  };

  status=find_mil();
  if (status != StatusOK) {
    errorstatus = (errorstatus | status);
    return status;
  };

  return StatusOK;
}


int scu_mil::scu_milbusclose(int &errorstatus)
{
  int status = 0;

  status=close_scu();

  if (status != StatusOK){
    errorstatus = (errorstatus | status);
    return status;
 }
  return StatusOK;
}


