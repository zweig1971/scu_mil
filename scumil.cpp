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
  scu_info.scu_connected = false;
}

scu_mil::~scu_mil(void)
{
}

// liest das mil-statusregister
// ---------------------
DWORD scu_mil::mil_status_read(WORD &mil_status)
{
	eb_data_t milstatus;

	if((eb_device_read(scu_info.device, scu_info.mil_base+mil_wr_rd_status, EB_BIG_ENDIAN|EB_DATA32, &milstatus, 0, eb_block)) != EB_OK)
	return timer_error;

	mil_status = WORD(milstatus);

	return status_ok;
}

// zieht den timer auf der scu/mil
// auf in us & kommt erst nach ablauf 
// zurueck in us
// ---------------------
DWORD scu_mil::mil_timer_wait(WORD time)
{
	
	int MilData = 1;
	eb_data_t readtimer = 0;
	int status = 0;

	// timer ruecksetzen
	if ((status = eb_device_write(scu_info.device, scu_info.mil_base+rd_clr_wait_timer, EB_BIG_ENDIAN|EB_DATA32, MilData, 0, eb_block)) != EB_OK)
	return timer_error;


	// timer abfragen bis wert erreicht
	do
	{
		eb_device_read(scu_info.device, scu_info.mil_base+rd_clr_wait_timer, EB_BIG_ENDIAN|EB_DATA32, &readtimer, 0, eb_block);	
	}
	while(int(readtimer) < time);

	return status_ok;

}


// Prüft ob MilBus sendebereit ist
// ---------------------
bool scu_mil::mil_write_wait(void)
{
	// Pruefen mil-bus bereit
	if(mil_test_status(mil_trm_rdy))
	{
		return(true);
	}

	// timer aufziehen
	mil_timer_wait(WaitMilBusWriteTimeOut);

	// erneut pruefen ob mil-bus bereit
        if(mil_test_status(mil_trm_rdy))
        {
                return(true);
        }

	// timeout
	return (false);
}

// Prüft ob daten am MilBus bereitstehen
// ---------------------
bool scu_mil::mil_read_wait(void)
{
        // Pruefen mil-bus bereit
        if(mil_test_status(mil_rcv_rdy))
        {
                return(true);
        }

        // timer aufziehen
        mil_timer_wait(WaitMilBusReadTimeOut);

        // erneut pruefen ob mil-bus bereit
        if(mil_test_status(mil_rcv_rdy))
        {
                return(true);
        }

        // timeout
        return (false);
}

// Prueft status nach einem bestimten bit ab
// ---------------------
bool scu_mil::mil_test_status(WORD statusbit)
{
        WORD  mil_stat = status_ok;
        DWORD  status = status_ok;

        status = mil_status_read(mil_stat);

        if (status != status_ok){
                return false;
        }

        if((mil_stat & statusbit)== statusbit)
                return true;
        else
                return false;
}

// oeffnet ein socket & die device
// ---------------------
DWORD scu_mil::open_scu()
{
	eb_status_t status;

	// open socket
	if ((status = eb_socket_open(EB_ABI_CODE,0, EB_ADDR32|EB_DATA32, &scu_info.socket)) != EB_OK)
	return socketopen_error;

	// open device
	if ((status = eb_device_open(scu_info.socket,scu_info.scu_adress, EB_ADDR32|EB_DATA32, 3, &scu_info.device)) != EB_OK)
	return deviceopen_error;

	return status_ok;
}

// schliesst das socket & die device
// ---------------------
DWORD scu_mil::close_scu()
{
	eb_status_t status;

  	// close device
	if((status = eb_device_close(scu_info.device)) != EB_OK)
	return deviceclose_error;
  
 	// close socket
  	if((status = eb_socket_close(scu_info.socket)) != EB_OK)
	return socketclose_error;

	return status_ok;
}

// sucht die baseadresse vom milbus
// auf der device & gibt sie zurück
// ---------------------
DWORD scu_mil::find_mil()
{
	eb_status_t status;
	struct sdb_device sdbDevice;

	int nDevices = 1;
	sdbDevice.sdb_component.addr_first = 0;
	
	if ((status = eb_sdb_find_by_identity(scu_info.device, vendor, product, &sdbDevice, &nDevices)) != EB_OK)
	return baseadress_error;

	scu_info.mil_base = sdbDevice.sdb_component.addr_first;

        return status_ok;
}

// schreibt einen funktionscode auf
// den milbus
// ---------------------
DWORD scu_mil::fct_send()
{
	
        return 0;
}

// schreibt ein datum auf den milbus
// ---------------------
DWORD scu_mil::milbus_write()
{
        return 0;
}

// liest ein datum vom milbus
// ---------------------
DWORD scu_mil::milbus_read()
{
        return 0;
}

// liest das eventfifo der scu aus
// ---------------------        
DWORD scu_mil::event_fifo_read()
{
        return 0;
}

DWORD scu_mil::event_timer_read()
{
        return 0;
}

DWORD scu_mil::event_filter_read()
{
        return 0;
}

DWORD scu_mil::event_filter_set()
{
        return 0;
}
        
DWORD scu_mil::irq_mask_read()
{
        return 0;
}

DWORD scu_mil::irq_mask_write()
{
        return 0;
}

DWORD scu_mil::irq_enable()
{
        return 0;
}

DWORD scu_mil::irq_disable()
{
        return 0;
}

// wertet die fehler nummer aus
string scu_mil::scu_milerror(DWORD status)
{
        string ErrorMessage;

        switch (status)
        {
        case status_ok:
                ErrorMessage = "Status OK";
                break;
        case socketopen_error:
                ErrorMessage = "Open socket to scu failure";
                break;
        case deviceopen_error:
                ErrorMessage = "Open scu-device failure";
                break;
        case deviceclose_error:
                ErrorMessage = "Close scu-device failure";
                break;
        case socketclose_error:
                ErrorMessage = "Close socket to scu failure";
                break;
        case baseadress_error:
                ErrorMessage = "Cant find mil-baseadress on scu-device";
                break;
        case device_allrdyopen:
                ErrorMessage = "Scu-device allready open";
                break;
        case timer_error:
                ErrorMessage = "Can not set the timer on scu-device";
                break;
        case timeout_write:
                ErrorMessage = "Timeout: mil-bus too busy to transmit";
                break;
        case timeout_read:
                ErrorMessage = "Timeout: no data from mil-bus received";
                break;
	}
	return ErrorMessage;
}

// oeffnet scu/milbus
DWORD scu_mil::scu_milbusopen(const char adress[], DWORD &errorstatus)
{
  DWORD status = status_ok;

  if(scu_info.scu_connected)
  return device_allrdyopen;	

  strcpy (scu_info.scu_adress, adress);

  status=open_scu();

  if (status != status_ok) {
    errorstatus = (errorstatus | status);
    return status;
  };

  status=find_mil();
  if (status != status_ok) {
    errorstatus = (errorstatus | status);
    return status;
  };

  scu_info.scu_connected = true;
  return status_ok;
}

// schliesse scu/milbus
DWORD scu_mil::scu_milbusclose(DWORD &errorstatus)
{
  DWORD status = status_ok;

  status=close_scu();

  if (status != status_ok){
    errorstatus = (errorstatus | status);
    return status;
 }

  scu_info.scu_connected = false;
  return status_ok;
}

// testet den mil status ab
bool scu_mil::scu_milstatustest(WORD statusbit, DWORD &errorstatus)
{
	return (true);
}


// zieht einen timer auf & kommt nach ablauf zurueck
DWORD scu_mil::scu_timer_wait(DWORD time, DWORD &errorstatus)
{
	DWORD status = status_ok;
	
	status = mil_timer_wait(time);

        if (status != status_ok){
                errorstatus = (errorstatus | status);
                return false;
        }
	return status_ok;
}



