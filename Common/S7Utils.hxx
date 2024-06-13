/** © Copyright 2023 CERN
 *
 * This software is distributed under the terms of the
 * GNU Lesser General Public Licence version 3 (LGPL Version 3),
 * copied verbatim in the file “LICENSE”
 *
 * In applying this licence, CERN does not waive the privileges
 * and immunities granted to it by virtue of its status as an
 * Intergovernmental Organization or submit itself to any jurisdiction.
 *
 * Author: Alexandru Savulescu (HSE)
 *
 **/
#pragma once

#include <string>
#include <cstring>
#include "snap7.h"
#include <sstream>
#include <iomanip>
#include "Common/Utils.hxx"

namespace Common{
    class S7Utils{
        public:
            enum class Operation: int {READ = 0, WRITE = 1};
            static int AddressGetArea(const std::string& Address)
            {
                if(Address.length() < 2){
                    return -1; //invalid
                }
                switch(std::toupper(Address[0]))
                {
                    case 'V': //Data Blocks
                        return S7AreaDB;
                    case 'I':
                    case 'E': //Inputs
                        return S7AreaPE;
                    case 'Q':
                    case 'A': //Outputs
                        return S7AreaPA;
                    case 'M':
                    case 'F': //Flag memory
                        return S7AreaMK;
                    case 'T': //Timers
                        return S7AreaTM;
                    case 'C':
                    case 'Z': //Counters
                        return S7AreaCT;
                    default:
                        return -1; //invalid
                }

                return 0; //dummy
            }

            static int AddressGetWordLen(const std::string& Address)
            {
                if(Address.length() < 2){
                    return -1; //invalid
                }
                switch(std::toupper(Address[1]))
                {
                    case 'B':
                        return S7WLByte;
                    case 'W':
                        return S7WLWord;
                    case 'D':
                        return S7WLReal;     //e.g. VD124 GLB.CAL.GANA1
                    default:                
                        return S7WLBit;      //e.g.: V255.3
                }
                return 0; //dummy
            }


            static int AddressGetStart(const std::string& Address)
            {
                if(Address.length() < 2){
                    return -1; //invalid
                }
                switch(std::toupper(Address[1]))
                {
                    case 'B':
                    case 'W':
                    case 'D':  
                        //Addesses like XX9999
                        if(Address.find_first_of('.')  == std::string::npos){
                            return std::stoi(Address.substr(2)); //e.g.:VB2978
                        }
                        else{
                            return std::stoi(Address.substr(2, Address.find_first_of('.')-1)); //e.g.:VB2978.20
                        }
                        break;
                    default:
                        //Addesses like X9999
                        if(Address.find_first_of('.')  == std::string::npos){
                            return -1; //invalid
                        }
                        else{
                            return std::stoi(Address.substr(1, Address.find_first_of('.')-1)); //e.g.: V255.3
                        }
                        break;
                }
                
                return 0; //dummy
            }



            static int AddressGetAmount(const std::string& Address)
            {
                if(Address.length() < 2){
                    return -1; //invalid
                }
                if(std::toupper(Address[1]) == 'B' && Address.find_first_of('.') != std::string::npos){
                    return std::stoi(Address.substr(Address.find('.')+1));
                }
                return 1; //default
            }

            static int AddressGetBit(const std::string& Address)
            {
                if(Address.length() < 2){
                    return -1; //invalid
                }
                if(AddressGetWordLen(Address) == S7WLBit && Address.find_first_of('.') != std::string::npos){
                    return std::stoi(Address.substr(Address.find('.')+1));
                }

                return 0; //default
            }

            static int DataSizeByte(int WordLength)
            {
                switch (WordLength){
                    case S7WLBit     : return 1;  // S7 sends 1 byte per bit
                    case S7WLByte    : return 1;
                    case S7WLWord    : return 2;
                    case S7WLDWord   : return 4;
                    case S7WLReal    : return 4;
                    case S7WLCounter : return 2;
                    case S7WLTimer   : return 2;
                    default          : return 0;
                }
            }

            static bool AddressIsValid(const std::string& Address){
                return AddressGetArea(Address)!=-1 && 
                AddressGetWordLen(Address)!=-1 &&
                AddressGetAmount(Address)!=-1 &&
                AddressGetStart(Address)!=-1;
            }

            static std::string DisplayTS7DataItem(const PS7DataItem& item, Operation op = Operation::READ)
            {
                const std::string opStr = op == Operation::READ ? "READ" : "WRITE";
                std::stringstream ss;
                if (!item->pdata){
                    ss << "-->" << opStr << " : pdata is null";
                    return ss.str();
                }
                switch(item->WordLen){
                    case S7WLByte:
                        if(item->Amount>1){
                            std::string strVal(reinterpret_cast<const char*>(item->pdata), reinterpret_cast<const char*>(item->pdata) + item->Amount);
                            ss << "--> " << opStr << " value as string :'" << strVal << "'";
                        }
                        else{
                            uint8_t byteVal;
                            std::memcpy(&byteVal, item->pdata  , sizeof(uint8_t));
                            ss << "--> " << opStr << " value as byte : " << static_cast<int>(byteVal) << "";
                        }
                        break;
                    case S7WLWord:
                    {
                        uint16_t wordVal = Common::Utils::CopyNSwapBytes<uint16_t>(item->pdata);
                        ss << "--> " << opStr << " value as word : " << wordVal << "";
                        break;
                    }
                    case S7WLReal:
                    {
                        float realVal = Common::Utils::CopyNSwapBytes<float>(item->pdata);
                        ss << "--> " << opStr << " value as real : " << std::fixed << std::setprecision(3) << realVal << "";
                        break;
                    }
                    case S7WLBit:
                    {
                        uint8_t bitVal;
                        std::memcpy(&bitVal, item->pdata  , sizeof(uint8_t));
                        ss << "--> " << opStr << " value as bit : " << static_cast<int>(bitVal) << "";
                        break;
                    }
                    default:
                        ss << "--> " << opStr << " : unknown WordLen";
                        break;
                }
                
                return ss.str();
            }

            static TS7DataItem TS7DataItemFromAddress(const std::string& Address, bool allocateMemory = false){
                TS7DataItem item;
                item.Area     = AddressGetArea(Address);
                item.WordLen  = AddressGetWordLen(Address);
                item.DBNumber = 1;
                item.Start    = item.WordLen == S7WLBit ? (AddressGetStart(Address)*8)+AddressGetBit(Address) : AddressGetStart(Address);
                item.Amount   = AddressGetAmount(Address);
                item.Result   = -1;
                if(allocateMemory) 
                {
                    TS7AllocateDataItemForAddress(item);
                }
                else
                {
                    item.pdata    = nullptr;
                }
                return item;
            }

            static TS7DataItem TS7DataItemShallowClone(const TS7DataItem& item){
                TS7DataItem newItem;
                newItem.Area     = item.Area;
                newItem.WordLen  = item.WordLen;
                newItem.DBNumber = item.DBNumber;
                newItem.Start    = item.Start;
                newItem.Amount   = item.Amount;
                newItem.pdata    = nullptr;
                newItem.Result   = -1;
                TS7AllocateDataItemForAddress(newItem);
                return newItem;
            }

            static void TS7AllocateDataItemForAddress(TS7DataItem& item){
                if(item.pdata){
                    delete[] static_cast<char*>(item.pdata);
                }
                item.pdata    =  new char[DataSizeByte(item.WordLen )*item.Amount];
                std::memset(item.pdata, 0, DataSizeByte(item.WordLen )*item.Amount);
            }

            static int GetByteSizeFromAddress(const std::string& Address)
            {
                TS7DataItem item = TS7DataItemFromAddress(Address);
                return (DataSizeByte(item.WordLen )*item.Amount);
            }
    }; //class S7Utils
} //namespace Common