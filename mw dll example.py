
from ctypes import (WinDLL, Structure, c_bool, c_char, c_wchar, c_byte, c_ubyte, c_short, c_ushort, c_int, c_uint, c_long, c_ulong, c_longlong, c_ulonglong, c_float, c_double, c_char_p, c_wchar_p, c_void_p, POINTER, byref)

#   reference to ctypes dll for LMX2820 registers
LMX2820_dll = WinDLL("./LMX2820_DLL/LMX2820.dll")




''' generic helper functions    '''


#   declare max register size for 123x3 2D array: R0<-->R122
class registersMatrix(Structure):
    _fields_ = [("numberRegisters", c_ubyte), ("dataBytes", c_ubyte * (123 * 3))]

#   convert ctypes struct to python list
def _reassembleList(dll2Darray:registersMatrix) -> list:
    '''
    converts ctypes struct object into python list of registers data
    '''
    registersList = []
    for i in range(dll2Darray.numberRegisters):
        registersList.append([dll2Darray.dataBytes[i * 3], dll2Darray.dataBytes[i * 3 + 1], dll2Darray.dataBytes[i * 3 + 2]])
    if (dll2Darray.numberRegisters) != len(registersList):
        raise ValueError("Incorrect array size generated.")
    return registersList

#   read LMX2820 register
def _read_spi_Register(address:int) -> list:
    '''
    LMX2820 dll doesn't have SPI MISO access;
    therefore read register and pass arrays to dll for calculations,
    then write functions and bitwise OR to avoid over-write other bits in same register
    '''
    if not address in range(0, 123):
        raise ValueError("Invalid LMX2820 register number in readRegister() function")
    readBuffer = [(address|0x80) , 0, 0]  # mask 0x80 for R/W bit set to 1
    readBuffer = list(FT232H.write_read_spi(LMX2820_spi, readBuffer))
    readBuffer[0] = address
    return readBuffer

#   set FCAL_EN (R0[4]) bit=1 for VCO calibration
def _program_FCAL_EN():
    FT232H.write_spi(LMX2820_spi, _get_R0register('FCAL_EN', 1))



''' General Registers R0, R1, R7 (R/W)   '''
# need to read R0 from SPI before inputting list (arry) to dll
# R0: POWERDOWN, RESET, FCAL_EN, DBLR_CAL_EN, INSTCAL_SKIP_ACAL
def _get_R0register(R0_field:str, value:int)->list:
    # dictionary of R0 fields to dll enum integers
    R0_fields = {'POWERDOWN':0,
                    'RESET':1,
                    'FCAL_EN':2,
                    'DBLR_CAL_EN':3,
                    'INSTCAL_SKIP_ACAL':4}
    match R0_field:
        case 'POWERDOWN':
            if not 0 <= value <= 1:
                raise ValueError("Invalid POWERDOWN value in _get_R0register function")
        case 'RESET':
            if not 0 <= value <= 1:
                raise ValueError("Invalid RESET value in _get_R0register function")
        case 'FCAL_EN':
            if not 0 <= value <= 1:
                raise ValueError("Invalid FCAL_EN value in _get_R0register function")
        case 'DBLR_CAL_EN':
            if not 0 <= value <= 1:
                raise ValueError("Invalid DBLR_CAL_EN value in _get_R0register function")
        case 'INSTCAL_SKIP_ACAL':
            if not 0 <= value <= 1:
                raise ValueError("Invalid INSTCAL_SKIP_ACAL value in _get_R0register function")
    # convert python to dll unsigned char types
    LMX2820_dll.get_R0_register.argtypes = [POINTER(c_ubyte * 3), c_int, c_ubyte]
    # must read from R0 and mask out values
    R0_register = _read_spi_Register(0)
    R0_array = (c_ubyte * 3)(*R0_register)
    #print([x for x in R0_array])
    # R0_array modified via pointers in dll
    LMX2820_dll.get_R0_register(R0_array, R0_fields[R0_field], value)
    # convert ctypes array to python list
    return list(bytearray(R0_array))
def _calc_R0register():
    POWERDOWN = c_int(-1)
    RESET = c_int(-1)
    FCAL_EN = c_int(-1)
    FCAL_LPFD_ADJ = c_int(-1)
    FCAL_HPFD_ADJ = c_int(-1)
    DBLR_CAL_EN = c_int(-1)
    INSTCAL_SKIP_ACAL = c_int(-1)
    # must read from R0 and mask out values
    R0_register = _read_spi_Register(0)
    R0_array = (c_ubyte * 3)(*R0_register)
    LMX2820_dll.calc_R0_register.argtypes = [POINTER(c_ubyte * 3),
                                                POINTER(c_int),
                                                POINTER(c_int),
                                                POINTER(c_int),
                                                POINTER(c_int),
                                                POINTER(c_int),
                                                POINTER(c_int),
                                                POINTER(c_int)]
    LMX2820_dll.calc_R0_register(R0_array,
                                 byref(POWERDOWN),
                                 byref(RESET),
                                 byref(FCAL_EN),
                                 byref(FCAL_LPFD_ADJ),
                                 byref(FCAL_HPFD_ADJ),
                                 byref(DBLR_CAL_EN),
                                 byref(INSTCAL_SKIP_ACAL))
    print(R0_register)
    print("POWERDOWN ", (POWERDOWN.value))
    print("RESET ", (RESET.value))
    print("FCAL_EN ", (FCAL_EN.value))
    print("FCAL_LPFD_ADJ ", (FCAL_LPFD_ADJ.value))
    print("FCAL_HPFD_ADJ ", (FCAL_HPFD_ADJ.value))
    print("DBLR_CAL_EN ", (DBLR_CAL_EN.value))
    print("INSTCAL_SKIP_ACAL ", (INSTCAL_SKIP_ACAL.value))




##############################################################################################################
if __name__ == "__main__":
    ###     dll example interface function "add"    ###
    LMX2820_dll.add.argtypes = [c_int, c_int]
    LMX2820_dll.add.restype = c_int
    result = LMX2820_dll.add(2, 3)
    print(f"add = {result}")
    ###     get LMX2820.dll version     ###
    LMX2820_dll.get_DLL_Version.restype = c_char_p
    version_info = LMX2820_dll.get_DLL_Version().decode('utf-8')
    print(f"Version from DLL: {version_info}")
    print(type(version_info))
    print("\n~~~~~~~~~~~~~~~~~~~\n\n")
