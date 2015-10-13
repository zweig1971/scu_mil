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
#include <bitset>

scu_mil::scu_mil(void)
{
  scu_info.socket	= 0;
  scu_info.device	= 0;
  scu_info.mil_base	= 0;
  scu_info.scu_connected= false;
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
// ---------------------
DWORD scu_mil::mil_timer_wait(WORD time)
{
	
	int MilData		= 1;
	eb_data_t readtimer	= 0;
	int status		= 0;

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

	DWORD status;

	// Pruefen mil-bus bereit
	if(mil_test_status(mil_trm_rdy, status))
	{
		return(true);
	}

	// timer aufziehen
	mil_timer_wait(WaitMilBusWriteTimeOut);

	// erneut pruefen ob mil-bus bereit
        if(mil_test_status(mil_trm_rdy, status))
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

	DWORD status;

        // Pruefen mil-bus bereit
        if(mil_test_status(mil_rcv_rdy, status))
        {
                return(true);
        }

        // timer aufziehen
        mil_timer_wait(WaitMilBusReadTimeOut);

        // erneut pruefen ob mil-bus bereit
        if(mil_test_status(mil_rcv_rdy, status))
        {
                return(true);
        }

        // timeout
        return (false);
}

// Prueft status nach einem bit ab
// ---------------------
bool scu_mil::mil_test_status(WORD statusbit, DWORD &errorstatus)
{
        WORD  mil_stat = status_ok;
        DWORD status = status_ok;

	// lese status
        status = mil_status_read(mil_stat);
	
/*	cout<<"Mil status :";
	std::bitset<16> y(mil_stat);
	std::bitset<16> v(statusbit);
	std::cout << y <<" & "<<v<<endl;

	printf("HEX : 0x%x & 0x%x \n", mil_stat,statusbit);
*/
        if (status != status_ok){
		errorstatus = (errorstatus | status);
                return false;
        }

        if((mil_stat & statusbit)== statusbit){

//		cout<<"TRUE"<<endl;	
                return true;
	}
        else{
//		cout<<"FALSE"<<endl;
	        return false;
	}
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

	int nDevices				= 1;
	sdbDevice.sdb_component.addr_first	= 0;
	
	if ((status = eb_sdb_find_by_identity(scu_info.device, vendor, product, &sdbDevice, &nDevices)) != EB_OK)
	return baseadress_error;

	scu_info.mil_base = sdbDevice.sdb_component.addr_first;

        return status_ok;
}

// schreibt ein datum auf den milbus
// ---------------------
DWORD scu_mil::milbus_write(WORD mildata)
{
        eb_status_t status;

	// schreibe daten
  	if ((status = eb_device_write(scu_info.device, scu_info.mil_base+mil_rd_wr_data, EB_BIG_ENDIAN|EB_DATA32, int(mildata), 0, eb_block)) != EB_OK)
	return mil_write_error;

        return status_ok;
}

// schreibt einen cmd (functioscode) auf den milbus
// ---------------------
DWORD scu_mil::milbus_write_cmd(int cmd)
{
	eb_status_t status;

	// schreibe cmd
	if ((status = eb_device_write(scu_info.device, scu_info.mil_base+mil_wr_cmd, EB_BIG_ENDIAN|EB_DATA32, cmd, 0, eb_block)) != EB_OK)
	return mil_write_cmd_error;

	return status_ok;
}

// liest ein datum vom milbus
// ---------------------
DWORD scu_mil::milbus_read(WORD &mildata)
{
	eb_data_t mil_rd_data = 0;
	eb_status_t status;

	//lese vom milbus
	if ((status = eb_device_read(scu_info.device, scu_info.mil_base+mil_rd_wr_data, EB_BIG_ENDIAN|EB_DATA32, &mil_rd_data, 0, eb_block)) != EB_OK)
	return mil_read_error;


	mildata = WORD(mil_rd_data);
        return status_ok;
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

// wertet die fehlernummer aus
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
        case device_not_open:
                ErrorMessage = "Scu-device not open";
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
        case mil_write_cmd_error:
                ErrorMessage = "Write cmd to mil-bus failure";
                break;
        case mil_read_error:
                ErrorMessage = "Read data from mil-bus failure";
                break;
        case mil_write_error:
                ErrorMessage = "Write data to mil-bus failure";
                break;
        case mil_ifknr_error:
                ErrorMessage = "Send/read ifk-nr not equal";
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

        // pruefen ob device offen
        if(!(scu_info.scu_connected)){
                errorstatus = (errorstatus | device_not_open);
                return device_not_open;
        }

	// device close
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

	DWORD test_status = status_ok;

        // pruefen ob device offen
        if(!(scu_info.scu_connected)){
                errorstatus = (errorstatus | device_not_open);
                return device_not_open;
        }

	// bit pruefen
	if(!(mil_test_status(statusbit, test_status))){
		errorstatus = (errorstatus | test_status);
		return false;
	}

	return true;
}

// zieht einen timer auf & kommt nach ablauf zurueck
DWORD scu_mil::scu_timer_wait(DWORD time, DWORD &errorstatus)
{
	DWORD status = status_ok;

        // pruefen ob device offen
        if(!(scu_info.scu_connected)){
                errorstatus = (errorstatus | device_not_open);
                return device_not_open;
        }
	
	// timer aufziehen
	status = mil_timer_wait(time);

        if (status != status_ok){
                errorstatus = (errorstatus | status);
                return false;
        }
	return status_ok;
}

// schreibt einen funktionscode auf
// den milbus
// ---------------------
DWORD scu_mil::scu_milbus_write_cmd(BYTE funktionscode, BYTE ifkadresse, DWORD &errorstatus)
{
        int sendfunctioncode	= 0;
	DWORD status		= status_ok;

	// pruefen ob device offen
	if(!(scu_info.scu_connected)){
		errorstatus = (errorstatus | device_not_open);
		return device_not_open;
	}

        // milbuspaket zusammenbasteln
        sendfunctioncode = funktionscode;
        sendfunctioncode = sendfunctioncode << 8;
        sendfunctioncode = sendfunctioncode + ifkadresse;

        // milbus prüfen ob sendebereit
        if (!(mil_write_wait())){
		errorstatus = (errorstatus | timeout_write);
		return timeout_write;
	}

	// schreibe cmd auf milbus
	status = milbus_write_cmd(sendfunctioncode);

        if (status != status_ok){
                errorstatus = (errorstatus | status);
                return status;
        }
        return status_ok;
}

// schreibt ein datum auf
// den milbus
// ---------------------
DWORD scu_mil::scu_milbus_write_data(WORD data, DWORD &errorstatus)
{

	DWORD status = status_ok;

        // pruefen ob device offen
        if(!(scu_info.scu_connected)){
                errorstatus = (errorstatus | device_not_open);
                return device_not_open;
        }

        // milbus prüfen ob sendebereit
        if (!(mil_write_wait()))
        return timeout_write;

        // schreibe daten auf milbus
        status = milbus_write(data);
		
        if (status != status_ok){
                errorstatus = (errorstatus | status);
                return status;
        }
        return status_ok;
}

// liest ein datum 
// vom milbus
// ---------------------
DWORD scu_mil::scu_milbus_read_data(WORD &data, DWORD &errorstatus)
{

        DWORD status = status_ok;

        // pruefen ob device offen
        if(!(scu_info.scu_connected)){
                errorstatus = (errorstatus | device_not_open);
                return device_not_open;
        }

        // milbus auf daten pruefen
        if (!(mil_read_wait()))
        return timeout_read;

        // lese daten vom  milbus
        status = milbus_read(data);

        if (status != status_ok){
                errorstatus = (errorstatus | status);
                return status;
        }

	return status_ok;
}

//schreibt ein datum an eine ifk
DWORD scu_mil::scu_milbus_ifk_wr (BYTE ifkadress, BYTE ifkfunktioncode, WORD data, DWORD &errorstatus)
{

	DWORD status	= status_ok;
	DWORD errstatus = status_ok;

        // pruefen ob device offen
        if(!(scu_info.scu_connected)){
                errorstatus = (errorstatus | device_not_open);
                return device_not_open;
        }

	// schreibe datum auf milbus
	status = scu_milbus_write_data( data, errstatus);
        if (status != status_ok){
                errorstatus = (errorstatus | status);
                return status;
        }

	// schreibe cmd auf milbus
	status = scu_milbus_write_cmd(ifkfunktioncode, ifkadress, errstatus);
        if (status != status_ok){
                errorstatus = (errorstatus | status);
                return status;
        }

/*	// lese datum vom milbus
	status = scu_milbus_read_data(data, errstatus);
        if (status != status_ok){
                errorstatus = (errorstatus | status);
                return status;
        }*/

	return status_ok;
}

//liest ein datum von einer ifk
DWORD scu_mil::scu_milbus_ifk_rd (BYTE ifkadress, BYTE ifkfunktioncode, WORD &data, DWORD &errorstatus)
{

        DWORD status	= status_ok;
        DWORD errstatus = status_ok;

        // pruefen ob device offen
        if(!(scu_info.scu_connected)){
                errorstatus = (errorstatus | device_not_open);
                return device_not_open;
        }

        // schreibe cmd auf milbus
        status = scu_milbus_write_cmd(ifkfunktioncode, ifkadress, errstatus);
        if (status != status_ok){
                errorstatus = (errorstatus | status);
                return status;
        }

        // lese datum vom milbus
        status = scu_milbus_read_data(data, errstatus);
        if (status != status_ok){
                errorstatus = (errorstatus | status);
                return status;
        }

	return status_ok;
}

//prueft ob die ifk online ist
bool scu_mil::scu_milbus_ifk_on (BYTE ifkadress, WORD &returnifkad, DWORD &errorstatus)
{

        DWORD status		= status_ok;
        DWORD errstatus		= status_ok;

        WORD ReadAdress		= 0;

        // pruefen ob device offen
        if(!(scu_info.scu_connected)){
                errorstatus = (errorstatus | device_not_open);
                return false;
        }

        // schreibe cmd auf milbus
        status = scu_milbus_write_cmd(Fct_Rd_Stat0, ifkadress, errstatus);
        if (status != status_ok){
                errorstatus = (errorstatus | status);
                return false;
        }

        // lese datum vom milbus
        status = scu_milbus_read_data(ReadAdress, errstatus);
        if (status != status_ok){
                errorstatus = (errorstatus | status);
                return false;
        }

        // Bei C0-Status Highbyte wegmaskieren
        ReadAdress = ReadAdress & Fct_Rd_Stat0_AdrMask;
	returnifkad = ReadAdress;

	// beide nummern vergleichen -> muessen gleich sein
	if (ReadAdress != ifkadress)
	{
                errorstatus = (errorstatus | mil_ifknr_error);
                return false;
	}

        return true;
}










