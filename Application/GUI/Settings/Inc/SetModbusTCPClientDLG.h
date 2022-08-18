/*
 * SetModbusTCPClientDLG.h
 *
 *  Created on: 29.05.2018
 *      Author: TomaszSok
 */

#ifndef GUI_SETTINGS_SETMODBUSTCPCLIENTDLG_H_
#define GUI_SETTINGS_SETMODBUSTCPCLIENTDLG_H_

#include "DIALOG.h"
#include "tcpipclient_netconn.h"

extern MODBUS_TCP_REGISTER tempModbusTCPregisters[MAX_MODBUS_TCP_REGISTERS];
extern MODBUS_TCP_SERVER tempMBserver[MAX_MODBUS_TCP_CONNECTIONS];

WM_HWIN CreateSetModbusTCPServers(WM_HWIN hParent);
WM_HWIN CreateSetModbusTCPRegisters(WM_HWIN hParent);

#endif /* GUI_SETTINGS_SETMODBUSTCPCLIENTDLG_H_ */
