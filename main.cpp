#include "scumil.h"



int main(void){

	int status = 0;
	int errorstatus = 0;
	string error_str;
	scu_mil myscu;

	status = myscu.scu_milbusopen("tcp/scuxl0089.acc", errorstatus);
	error_str = myscu.scu_milerror(status);  
	cout <<"open scu :"<<error_str<<endl;
	

        status = myscu.scu_milbusclose(errorstatus);
        error_str = myscu.scu_milerror(status);
        cout <<"close scu :"<<error_str<<endl;

	

	return 0;
}
