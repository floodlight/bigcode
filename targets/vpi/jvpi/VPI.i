%module VPI
%{
#include <VPI/vpi.h>
%}

%include "inttypes.i"
%include "arrays_java.i"
%include "cpointer.i"
%include various.i
%include "enums.swg"

%apply char[] {unsigned char*}
%apply long long {vpi_t}
//%apply int {vpiProtocolOpcode_t}
//
// These two functions are redeclared with the special BYTE
// indicator so they will be treated as INOUT buffers
//
int vpi_recv(vpi_t vpi, char *BYTE, int len, int block);
int vpi_send(vpi_t vpi, char* BYTE, int len);

%ignore vpi_log_flag_desc_map;
%ignore vpi_protocol_opcode_map;
%ignore vpi_protocol_opcode_desc_map;
%ignore vpi_log_flag_map;


%include "../../../modules/VPI/module/inc/VPI/vpi.h"

