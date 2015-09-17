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
#define DeviceClose_Error       0x4
#define SocketClose_Error       0x8
#define BaseAdress_Error        0x10
#define Timer_Error		0x20

// Bitmasken Status 
//----------------
#define interlock		0x1
#define data_rdy		0x2		
#define data_req		0x4
#define ev_fifo_ne		0x8
#define ev_fifo_full		0x10
#define mil_rcv_rdy		0x20
#define mil_cmd_rcv		0x40
#define mil_trm_rdy		0x80
#define mil_rcv_err		0x100
#define ev_reset_on		0x200
#define puls1_frame		0x400
#define puls2_frame		0x800
#define debounce_on		0x1000
#define ev_filt_on		0x2000
#define ev_filt_12_8b		0x4000
#define sel_fpga_n6408		0x8000

// Register Interface Mil-Macro 
//----------------
#define mil_rd_wr_data		0x0
#define mil_wr_cmd		0x4
#define mil_wr_rd_status	0x8
#define rd_clr_no_vw_cnt	0xc
#define rd_wr_not_eq_cnt	0x10
#define rd_wr_ev_fifo		0x14
#define rd_clr_ev_timer		0x18
#define rd_clr_dly_timer	0x1c
#define rd_clr_wait_timer	0x20
#define mil_wr_rd_lemo_conf	0x24
#define mil_wr_rd_lemo_dat	0x28
#define mil_rd_lemo_inp		0x2c

