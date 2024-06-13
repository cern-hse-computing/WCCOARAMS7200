/**
 * Library of functions used to address RAMS7200 Driver DPE
 * @file   rams7200_dpe_addressing.ctl
 * @author Alexandru Savulescu
 * @date   10/06/2024
 * @modifications:
 *  -[author] [date] [object]
*/



const unsigned RAMS7200_MODE_OUT = 1;
const unsigned RAMS7200_MODE_IN = 2;
const unsigned RAMS7200_MODE_INOUT = 6;

const unsigned RAMS7200_DATA_TYPE_BOOL = 1000;
const unsigned RAMS7200_DATA_TYPE_CHAR = 1001;
const unsigned RAMS7200_DATA_TYPE_INT16 = 1002;
const unsigned RAMS7200_DATA_TYPE_INT32 = 1003;
const unsigned RAMS7200_DATA_TYPE_FLOAT = 1004;
const unsigned RAMS7200_DATA_TYPE_STRING = 1005;

private const unsigned RAMS7200_TYPE 			= 1;
private const unsigned RAMS7200_DRIVER_NUMBER 	= 32;
private const unsigned RAMS7200_REFERENCE		= 3;
private const unsigned RAMS7200_DIRECTION		= 4;
private const unsigned RAMS7200_DATATYPE			= 5;
private const unsigned RAMS7200_ACTIVE 			= 6;
private const unsigned RAMS7200_SUBINDEX = 7;


/**
 * Called when a DPE connected to this driver is adressed (used to set the proper periph. address config)
 * @param dpe             path to dpe to address 
 * @param dataType        Data Type (RAMS7200_DATA_TYPE_*)
 * @param mode            adressing mode ( RAMS7200_MODE_IN or RAMS7200_MODE_OUT or RAMS7200_MODE_INOUT)
 * @param driverNum       driver manager number
 * @param plc_ip          S7200 PLC IP
 * @param address         S7 address
 * @param polling_interval 
 * @return 1 if OK, 0 if not
*/

public int RAMS7200_addressDPE(string dpe, unsigned dataType, unsigned mode, unsigned driverNum, string plc_ip, string address, unsigned polling_interval)
{
  dyn_anytype params;
  try
  {
    params[RAMS7200_DRIVER_NUMBER] = driverNum;
    params[RAMS7200_DIRECTION]= mode;
    params[RAMS7200_ACTIVE] = true;
    params[RAMS7200_SUBINDEX] = 0;
    params[RAMS7200_DATATYPE] = dataType;
    params[RAMS7200_REFERENCE] = plc_ip + "$" + address + "$" + polling_interval;
    RAMS7200_setPeriphAddress(dpe, params);
  }
  catch
  {
    DebugN("Error: Uncaught exception in RAMS7200_addressDPE: " + getLastException());
    return 0;
  }
  return 1;
}


/**
 * Called when a DPE connected to this driver is adressed (used to set the proper periph. address config)
 * @param dpe             path to dpe to address 
 * @param dataType        Data Type (RAMS7200_DATA_TYPE_*)
 * @param mode            adressing mode ( RAMS7200_MODE_IN or RAMS7200_MODE_OUT or RAMS7200_MODE_INOUT)
 * @param driverNum       driver manager number
 * @param plc_ip          S7200 PLC IP
 * @param configName      name of the config (e.g. _DEBUGLVL)
 * @return 1 if OK, 0 if not
*/

public int RAMS7200_addressConfigDPE(string dpe, unsigned dataType, unsigned mode, unsigned driverNum, string configName)
{
  dyn_anytype params;
  try
  {
    params[kafkaAddress_DRIVER_NUMBER] = driverNum;
    params[kafkaAddress_DIRECTION]= mode;
    params[kafkaAddress_ACTIVE] = true;
    params[kafkaAddress_SUBINDEX] = 0;
    params[kafkaAddress_DATATYPE] = dataType;
    params[kafkaAddress_REFERENCE] = configName;
    kafkaAddress_setPeriphAddress(dpe, params);
  }
  catch
  {
    DebugN("Error: Uncaught exception in kafkaAddress_addressConfigDPE: " + getLastException());
    return 0;
  }
  return 1;
}



/**
  * Method setting addressing for RAMS7200 datapoints elements
  * @param datapoint element for which address will be set
  * @param configuration parameteres
  */
private void RAMS7200_setPeriphAddress(string dpe, dyn_anytype configParameters){

  int i = 1;
  dyn_string names;
  dyn_anytype values;

	dpSetWait(dpe + ":_distrib.._type", DPCONFIG_DISTRIBUTION_INFO,
						dpe + ":_distrib.._driver", configParameters[RAMS7200_DRIVER_NUMBER] );
	dyn_string errors = getLastError();
  if(dynlen(errors) > 0){
		throwError(errors);
		DebugN("Error: Could not create the distrib config");
		return;
	}

  names[i] = dpe + ":_address.._type";
  values[i++] = DPCONFIG_PERIPH_ADDR_MAIN;
  names[i] = dpe + ":_address.._drv_ident";
  values[i++] = "RAMS7200";
  names[i] = dpe + ":_address.._reference";
  values[i++] = configParameters[RAMS7200_REFERENCE];
  names[i] = dpe + ":_address.._mode";
  values[i++] = configParameters[RAMS7200_DIRECTION];
  names[i] = dpe + ":_address.._datatype";
  values[i++] = configParameters[RAMS7200_DATATYPE];
  names[i] = dpe + ":_address.._subindex";
  values[i++] = configParameters[RAMS7200_SUBINDEX];

  dpSetWait(names, values);
}
