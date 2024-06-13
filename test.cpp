/*=============================================================================|
|  PROJECT SNAP7                                                         1.4.0 |
|==============================================================================|
|  Copyright (C) 2013, 2014 Davide Nardella                                    |
|  All rights reserved.                                                        |
|==============================================================================|
|  SNAP7 is free software: you can redistribute it and/or modify               |
|  it under the terms of the Lesser GNU General Public License as published by |
|  the Free Software Foundation, either version 3 of the License, or           |
|  (at your option) any later version.                                         |
|                                                                              |
|  It means that you can distribute your commercial software linked with       |
|  SNAP7 without the requirement to distribute the source code of your         |
|  application and without the requirement that your application be itself     |
|  distributed under LGPL.                                                     |
|                                                                              |
|  SNAP7 is distributed in the hope that it will be useful,                    |
|  but WITHOUT ANY WARRANTY; without even the implied warranty of              |
|  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the               |
|  Lesser GNU General Public License for more details.                         |
|                                                                              |
|  You should have received a copy of the GNU General Public License and a     |
|  copy of Lesser GNU General Public License along with Snap7.                 |
|  If not, see  http://www.gnu.org/licenses/                                   |
|==============================================================================|
|                                                                              |
|  Client Example                                                              |
|                                                                              |
|=============================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <cstring>
#include "snap7.h"
#include "Common/S7Utils.hxx"


#ifdef OS_WINDOWS
# define WIN32_LEAN_AND_MEAN
# include <windows.h>
#endif

    TS7Client *Client;

    byte Buffer[65536]; // 64 K buffer
    int SampleDBNum = 1000;

    char *Address;     // PLC IP Address
    int Rack=0,Slot=2; // Default Rack and Slot

    int ok = 0; // Number of test pass
    int ko = 0; // Number of test failure

    bool JobDone=false;
    int JobResult=0;

//------------------------------------------------------------------------------
//  Async completion callback 
//------------------------------------------------------------------------------
// This is a simply text demo, we use callback only to set an internal flag...
void S7API CliCompletion(void *usrPtr, int opCode, int opResult)
{
    JobResult=opResult;
    JobDone = true;
}

//------------------------------------------------------------------------------
//  Usage Syntax
//------------------------------------------------------------------------------
void Usage()
{
    printf("Usage\n");
    printf("  client <IP> [Rack=0 Slot=2]\n");
    printf("Example\n");
    printf("  client 192.168.1.101 0 2\n");
    printf("or\n");
    printf("  client 192.168.1.101\n");
    getchar();
}

//------------------------------------------------------------------------------
// Check error
//------------------------------------------------------------------------------
bool Check(int Result, const char * function)
{
    printf("\n");
    printf("+-----------------------------------------------------\n");
    printf("| %s\n",function);
    printf("+-----------------------------------------------------\n");
    if (Result==0) {
        printf("| Result         : OK\n");
        printf("| Execution time : %d ms\n",Client->ExecTime());
        printf("+-----------------------------------------------------\n");
        ok++;
    }
    else {
        printf("| ERROR !!! \n");
        if (Result<0)
            printf("| Library Error (-1)\n");
        else
            printf("| %s\n",CliErrorText(Result).c_str());
        printf("+-----------------------------------------------------\n");
        ko++;
    }
    return Result==0;
}

TS7DataItem RAMS7200Read(std::string RAMS7200Address, void* val)
{
    TS7DataItem item = Common::S7Utils::TS7DataItemFromAddress(RAMS7200Address, true);
    int memSize = ( Common::S7Utils::DataSizeByte(item.WordLen )*item.Amount);

    printf("-------------read RAMS7200Address=>(Area, Start, WordLen, Amount): memSize : %s =>(%d, %d, %d, %d) : %dB", RAMS7200Address.c_str(), item.Area, item.Start, item.WordLen, item.Amount, memSize);
    
    if(Client->ReadMultiVars(&item, 1) == 0){
        if(item.WordLen == S7WLWord){
            uint16_t wordVal = Common::Utils::CopyNSwapBytes<uint16_t>(item.pdata);
            std::memcpy(val, &wordVal, memSize);
        }
        else{
            std::memcpy(val, item.pdata, memSize);
        }
        printf(Common::S7Utils::DisplayTS7DataItem(&item, Common::S7Utils::Operation::READ).c_str());
    }
    else{
        printf("--> read NOK!\n");
    }
    return item;
}

TS7DataItem RAMS7200Write(std::string RAMS7200Address, void* val)
{
    TS7DataItem item =  Common::S7Utils::TS7DataItemFromAddress(RAMS7200Address, true);
    int memSize = ( Common::S7Utils::DataSizeByte(item.WordLen )*item.Amount);
    if(item.WordLen == S7WLWord){
        uint16_t wordVal = Common::Utils::CopyNSwapBytes<uint16_t>(val);
        std::memcpy(item.pdata , &wordVal , memSize);
    }
    else{
        std::memcpy(item.pdata , val , memSize);
    }
    printf("-------------write RAMS7200Address=>(Area, Start, WordLen, Amount): memSize : %s =>(%d, %d, %d, %d) : %dB", RAMS7200Address.c_str(), item.Area, item.Start, item.WordLen, item.Amount, memSize);
    if(Client->WriteMultiVars(&item, 1) == 0){
        printf("--> write OK!\n");
    }
    else{
        printf("--> write NOK!\n");
    }
    return item;
}




//------------------------------------------------------------------------------
// Unit Connection
//------------------------------------------------------------------------------
bool CliConnect()
{
    Client->SetConnectionParams(Address, 0x1101, 0x1100);
    printf("Trying to connect to %s", Address);
    int res = Client->Connect();
    if (Check(res,"UNIT Connection")) {
          printf("  Connected to   : %s (Rack=%d, Slot=%d)\n",Address,Rack,Slot);
          printf("  PDU Requested  : %d bytes\n",Client->PDURequested());
          printf("  PDU Negotiated : %d bytes\n",Client->PDULength());
    };
    return res==0;
}
//------------------------------------------------------------------------------
// Unit Disconnection
//------------------------------------------------------------------------------
void CliDisconnect()
{
     Client->Disconnect();
}
//------------------------------------------------------------------------------
// Perform readonly tests, no cpu status modification
//------------------------------------------------------------------------------
void PerformTests()
{
    std::string addresses[] = {
        "VW1984", //VW1984=13220
        "VB2978.20", //VB2978.20='29 1-035',
        "VW2000",
        "VW2002"
        "VB1604", //VB1604=8
        "V1604.2", //V1604.2=0
        "V1604.3", //V1604.3=1
        "V2640.0",
        "V2640.02",
        "V2640.04",
        "V2640.06",
        "V2641.0",
        "V2641.02",
        "V2641.04",
        "V2641.06",
        //"M10.0", //M10.0=1
        //"M10.1", //M10.1=0 Read-only?
        //"E0.0", //E0.0=1 Read-only?
        //"E1.1", //E0.0=0 Read-only?
        //"A0.2", //A0.2=1 Read-only?
        //"A0.3", //A0.3=0 Read-only?
        //"I0.2", //I0.2=1 Read-only?
        //"I0.3", //I0.3=0 Read-only?
        //"Q0.2", //Q0.2=1 Read-only?
        //"Q0.3" //Q0.3=0 Read-only?

    };

    for(std::string address : addresses){
        TS7DataItem item =  Common::S7Utils::TS7DataItemFromAddress(address, true);
        switch (item.WordLen){
            case S7WLByte:
                if(item.Amount >1){
                    printf("Item.amount is : %d\n", item.Amount);
                    char valInitR[256], valInitW[256], valChangedW[256], valChangedR[256];
                    RAMS7200Read(address, &valInitR);
                    std::memcpy(valChangedW , &valInitR , 256);
                    valChangedW[0] = valChangedW[0]+1;
                    RAMS7200Write(address, &valChangedW);
                    RAMS7200Read(address, &valChangedR);
                    Check(strcmp(valChangedW,valChangedR), ("IO :" + address).c_str());
                    std::memcpy(valInitW , &valInitR , 256);
                    RAMS7200Write(address, &valInitW);
                    RAMS7200Read(address, &valInitR);
                    Check(strcmp(valInitW,valInitR), ("IO re-init :" + address).c_str());
                    printf("valInitR: %s\n",valInitR);
                    printf("valInitW: %s\n",valInitW);
                    printf("valChangedW: %s\n",valChangedW);
                    printf("valChangedR: %s\n",valChangedR);
                }
                else{
                    uint8_t  valInitR, valInitW, valChangedW, valChangedR;
                    RAMS7200Read(address, &valInitR);
                    valChangedW = valInitR == 1 ? 0 : 1;
                    RAMS7200Write(address, &valChangedW);
                    RAMS7200Read(address, &valChangedR);
                    Check((valChangedW==valChangedR) ? 0 : -1, ("IO :" + address).c_str());
                    valInitW = valInitR;
                    RAMS7200Write(address, &valInitW);
                    RAMS7200Read(address, &valInitR);
                    Check((valInitW==valInitR) ? 0 : -1, ("IO re-init :" + address).c_str());
                    printf("valInitR: %d\n",valInitR);
                    printf("valInitW: %d\n",valInitW);
                    printf("valChangedW: %d\n",valChangedW);
                    printf("valChangedR: %d\n",valChangedR);  
                }
                break;
            case S7WLBit:{
                    uint8_t  valInitR, valInitW, valChangedW, valChangedR;
                    RAMS7200Read(address, &valInitR);
                    valChangedW = valInitR == 0x0001 ? 0x0000 : 0x0001;
                    RAMS7200Write(address, &valChangedW);
                    RAMS7200Read(address, &valChangedR);
                    Check((valChangedW==valChangedR) ? 0 : -1, ("IO :" + address).c_str());
                    valInitW = valInitR;
                    RAMS7200Write(address, &valInitW);
                    RAMS7200Read(address, &valInitR);
                    Check((valInitW==valInitR) ? 0 : -1, ("IO re-init :" + address).c_str());
                    printf("valInitR: %d\n",valInitR);
                      //hexdump(&valInitR , sizeof(valInitR));
                    printf("valInitW: %d\n",valInitW);
                      //hexdump(&valInitW , sizeof(valInitW));
                    printf("valChangedW: %d\n",valChangedW);
                      //hexdump(&valChangedW , sizeof(valChangedW));
                    printf("valChangedR: %d\n",valChangedR);
                      //hexdump(&valChangedR , sizeof(valChangedR));
                }
                break;
            case S7WLWord:{
                    uint16_t  valInitR, valInitW, valChangedW, valChangedR;
                    RAMS7200Read(address, &valInitR);
                    valChangedW = valInitR == 1 ? 0 : 1;
                    RAMS7200Write(address, &valChangedW);
                    RAMS7200Read(address, &valChangedR);
                    Check((valChangedW==valChangedR) ? 0 : -1, ("IO :" + address).c_str());
                    valInitW = valInitR;
                    RAMS7200Write(address, &valInitW);
                    RAMS7200Read(address, &valInitR);
                    Check((valInitW==valInitR) ? 0 : -1, ("IO re-init :" + address).c_str());
                    printf("valInitR: %d\n",valInitR);
                    printf("valInitW: %d\n",valInitW);
                    printf("valChangedW: %d\n",valChangedW);
                    printf("valChangedR: %d\n",valChangedR);
                }
                break;
            case S7WLReal:{
                    float  valInitR, valInitW, valChangedW, valChangedR;
                    RAMS7200Read(address, &valInitR);
                    valChangedW = valInitR == 1.0 ? 0.0 : 1.0;
                    RAMS7200Write(address, &valChangedW);
                    RAMS7200Read(address, &valChangedR);
                    Check((valChangedW==valChangedR) ? 0 : -1, ("IO :" + address).c_str());
                    valInitW = valInitR == 1 ? 0 : 1;
                    RAMS7200Write(address, &valInitW);
                    RAMS7200Read(address, &valInitR);
                    Check((valInitW==valInitR) ? 0 : -1, ("IO re-init :" + address).c_str());
                    printf("valInitR: %.3f\n",valInitR);
                    printf("valInitW: %.3f\\n",valInitW);
                    printf("valChangedW: %.3f\\n",valChangedW);
                    printf("valChangedR: %.3f\\n",valChangedR);
                }
                break;
            default:
                break;
        }
    }
}

//------------------------------------------------------------------------------
// Tests Summary
//------------------------------------------------------------------------------
void Summary()
{
    printf("\n");
    printf("+-----------------------------------------------------\n");
    printf("| Test Summary \n");
    printf("+-----------------------------------------------------\n");
    printf("| Performed : %d\n",(ok+ko));
    printf("| Passed    : %d\n",ok);
    printf("| Failed    : %d\n",ko);
}
//------------------------------------------------------------------------------
// Main              
//------------------------------------------------------------------------------
int main(int argc, char* argv[])
{
// Get Progran args (we need the client address and optionally Rack and Slot)  
    if (argc!=2 && argc!=4)
    {
        Usage();
        return 1;
    }
    Address=argv[1];
    if (argc==4)
    {
        Rack=atoi(argv[2]);
        Slot=atoi(argv[3]);
    }
    
// Client Creation
    Client= new TS7Client();
    Client->SetAsCallback(CliCompletion,NULL);

// Connection
    if (CliConnect())
    {
        PerformTests();
        CliDisconnect();
    };

// Deletion
    delete Client;
    Summary();

    return 0;
}
