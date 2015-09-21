#include "scumil.h"



int main(void){

	DWORD status = 0;
	DWORD errorstatus = 0;
	string error_str;
	scu_mil myscu;

	status = myscu.scu_milbusopen("tcp/scuxl0089.acc", errorstatus);
	error_str = myscu.scu_milerror(status);  
	cout <<"open scu : "<<error_str<<endl;



	cout<<"ziehe timer auf"<<endl;
	status = myscu.scu_timer_wait(100000, errorstatus);
	error_str = myscu.scu_milerror(status);
	cout <<"...abgelaufen: "<<error_str<<endl;

	

        status = myscu.scu_milbusclose(errorstatus);
        error_str = myscu.scu_milerror(status);
        cout <<"close scu : "<<error_str<<endl;

	

	return 0;
}
