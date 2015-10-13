#include "scumil.h"
#include <stdlib.h>
#include <termios.h>

int getch() {
    static int ch = -1, fd = 0;
    struct termios neu, alt;
    fd = fileno(stdin);
    tcgetattr(fd, &alt);
    neu = alt;
    neu.c_lflag &= ~(ICANON|ECHO);
    tcsetattr(fd, TCSANOW, &neu);
    ch = getchar();
    tcsetattr(fd, TCSANOW, &alt);
    return ch;
}

int kbhit(void) {
        struct termios term, oterm;
        int fd = 0;
        int c = 0;
        tcgetattr(fd, &oterm);
        memcpy(&term, &oterm, sizeof(term));
        term.c_lflag = term.c_lflag & (!ICANON);
        term.c_cc[VMIN] = 0;
        term.c_cc[VTIME] = 1;
        tcsetattr(fd, TCSANOW, &term);
        c = getchar();
        tcsetattr(fd, TCSANOW, &oterm);
        if (c != -1)
        ungetc(c, stdin);
        return ((c != -1) ? 1 : 0);
}


void scan_milbus(scu_mil myscu)
{
        int countmax = 256;
        int index    = 1;
        WORD returnifkad= 0;
        DWORD errorstatus = status_ok;

	cout<<"IFKs online:"<<endl;
	cout<<"------------"<<endl; 

	do
	{
		if(myscu.scu_milbus_ifk_on (index, returnifkad, errorstatus))
                {
			cout<<hex<<index<<" --- "<<returnifkad<<endl;
		}
                index++;

        }while (index < countmax);
}


void scan_milbus_loop(scu_mil myscu)
{
	getch();

	do
	{
		cout << "\033[2J\033[1;1H";
		scan_milbus(myscu);

	}while(kbhit()<1);
	
	cout << "\033[2J\033[1;1H";
}


BYTE choose_ifk(scu_mil myscu)
{
	int choose;
	scan_milbus(myscu);
	cout << "Choose IFK 0x";
	cin>>hex>>choose;
	return BYTE(choose);	
}


void write_cmd(scu_mil myscu)
{
	DWORD errorstatus = status_ok;
	DWORD status = status_ok;
	int cmd;
	int ifkadr;

	cout<<endl<<"Send a CMD to a IFK"<<endl;
	cout<<"-------------------"<<endl;

	cout<<"IFK Nr ? 0x";
	cin>>hex>>ifkadr;
	cout<< "CMD ? 0x";
	cin>>hex>>cmd;	

	status = myscu.scu_milbus_write_cmd(BYTE(cmd), BYTE(ifkadr),errorstatus);
	cout<<"Send CMD to IFK :";
	cout<<myscu.scu_milerror(status)<<endl;
}


void write_data(scu_mil myscu)
{
        DWORD errorstatus = status_ok;
        DWORD status = status_ok;
        int data;

        cout<<endl<<"Write DATA"<<endl;
        cout<<"-------------------"<<endl;

        cout<< "DATA ? 0x";
        cin>>hex>>data;

	status =  myscu.scu_milbus_write_data(WORD(data), errorstatus);	
        cout<<"Write Data to MilBus :";
        cout<<myscu.scu_milerror(status)<<endl;
}


void read_data(scu_mil myscu)
{
        DWORD errorstatus = status_ok;
        DWORD status = status_ok;
        WORD data = 0;

        cout<<endl<<"Read DATA"<<endl;
        cout<<"-------------------"<<endl;

	status=myscu.scu_milbus_read_data(data, errorstatus);
        cout<<"Read Data from MilBus :";
        cout<<myscu.scu_milerror(status)<<endl;
	printf("Data :0x%x \n", data);
}


void write_ifk(scu_mil myscu)
{
        DWORD errorstatus = status_ok;
        DWORD status = status_ok;
        int data = 0;
	int cmd  = 0;
	BYTE ifkadr;

        cout<<endl<<"Write DATA to IFK"<<endl;
        cout<<"-------------------"<<endl;

	ifkadr=choose_ifk(myscu);

        cout<< "DATA ? 0x";
        cin>>hex>>data;

        cout<< "CMD ? 0x";
        cin>>hex>>cmd;

	status = myscu.scu_milbus_ifk_wr (BYTE(ifkadr), BYTE (cmd), WORD (data), errorstatus);
        cout<<"Write Data to IFK :";
        cout<<myscu.scu_milerror(status)<<endl;
}


void read_ifk(scu_mil myscu)
{
        DWORD errorstatus = status_ok;
        DWORD status = status_ok;
        WORD data = 0;
        int cmd  = 0;
        BYTE ifkadr;

        cout<<endl<<"Read DATA from IFK"<<endl;
        cout<<"-------------------"<<endl;

        ifkadr=choose_ifk(myscu);

        cout<< "CMD ? 0x";
        cin>>hex>>cmd;

	status = myscu.scu_milbus_ifk_rd (BYTE(ifkadr), BYTE (cmd), data, errorstatus);

        cout<<"Read Data from IFK :";
        cout<<myscu.scu_milerror(status)<<endl;

	cout<<"Data read 0x"<<hex<<data<<endl;
}





void echo_test(scu_mil myscu, BYTE ifk)
{

        DWORD errorstatus = status_ok;
	bool run = true;
	int random;
	int count=0;
	WORD data_send = 0;
	WORD data_read = 0;
	DWORD status = status_ok;
	BYTE ifk_code_wr = 0x13;
	BYTE ifk_code_rd = 0x89;

//	printf("0x%x \n", ifk);

	cout<<endl<<"Echo test is running"<<endl;
	cout<<"--------------------"<<endl;

        getch();

	do
	{
		count ++;
		random = (rand()+rand());
		data_send = random & 0xFF;
		
		status = myscu.scu_milbus_ifk_wr (ifk, ifk_code_wr, data_send, errorstatus);
		if(status != status_ok)
		{
			cout<<"ERROR ifk write :"<<myscu.scu_milerror(status)<<endl;
		}

		status = myscu.scu_milbus_ifk_rd (ifk, ifk_code_rd, data_read, errorstatus);
                if(status != status_ok)
                {
                        cout<<"ERROR ifk read :"<<myscu.scu_milerror(status)<<endl;
                }

		if(data_send != data_read)
		{
			cout<<"ERROR : send data "<<hex<<data_send<<" < > read data "<<hex<<data_read<<endl;
		}
		
		if ((count % 1000) == 0)
		{
			cout<<"count : "<<dec<<count<<endl;
			if (kbhit()>=1){
				getch();
				run = false;
			}
		}
		

	}while (run);//(kbhit()<1);
}


int print_menue()
{
	int choice = 0;

	cout<<endl<<"Milbus Test"<<endl;
	cout<<"==========="<<endl<<endl;
	cout<<"[1] -- IFKs online"<<endl;
	cout<<"[2] -- write data"<<endl;
        cout<<"[3] -- write cmd"<<endl;
	cout<<"[4] -- read data"<<endl;
	cout<<"[5] -- write IFK"<<endl;
	cout<<"[6] -- read IFK"<<endl;
	cout<<"[7] -- echo test"<<endl;
	cout<<"[99] -- exit"<<endl<<endl;
	cout<<"your choice :";
	cin >> choice;
	
	return choice;
}	




int main(int argc, char *argv[]){

	DWORD status = 0;
	DWORD errorstatus = 0;
	string error_str;
	scu_mil myscu;

/*	int test = 0;
	BYTE blub = 0x38;

	cin>>hex>>test;

	printf("test 0x%x \n", (unsigned char)test);
	cout<<"HEX :"<<hex<<test<<endl;
	cout<<"DEC :"<<dec<<test<<endl;
	cout<<"BYTE HEX :"<<hex<<char(test)<<endl;
        cout<<"BYTE DEC :"<<dec<<BYTE(test)<<endl;
	
	//blub = test;

	cout<<"HEX blub: "<<hex<<blub<<endl;*/

	if(argc == 1){
		cout<<"No target device defined !"<<endl;
		cout<<"example :  scumil tcp/scuxl0089.acc"<<endl;
		cout<<"end"<<endl;
		return 0;
	}	

	
	int choice = 0;
	bool run = true;

	// generator starten
	srand((unsigned)time(NULL));

//	status = myscu.scu_milbusopen("tcp/scuxl0089.acc", errorstatus);
	status = myscu.scu_milbusopen(argv[1], errorstatus);
	error_str = myscu.scu_milerror(status);  
	cout <<"open scu : "<<error_str<<endl;
	if (status != status_ok){
		return 0;
	}
	
// -------------------------

	do{
		choice = print_menue();

		switch (choice)
		{
			case 1:
                		scan_milbus_loop(myscu);
				break;
                        case 2:
				write_data(myscu);
                                break;
                        case 3:
				write_cmd(myscu);
                                break;
                        case 4:
				read_data(myscu);
                                break;
                        case 5:
				write_ifk(myscu);
                                break;
                        case 6:
                                read_ifk(myscu);
				break;
			case 7:
				cout<<endl<<"IFKs online:"<<endl;
				cout<<"----------------"<<endl;
				echo_test(myscu,choose_ifk(myscu));
				break;
			default : run = false;
				break;
		}
	}while (run);
	


	
// -------------------------
        status = myscu.scu_milbusclose(errorstatus);
        error_str = myscu.scu_milerror(status);
        cout <<"close scu : "<<error_str<<endl;
	
	return 0;
}
