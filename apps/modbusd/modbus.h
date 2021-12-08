
#ifndef _MODBUS_H_
#define _MODBUS_H_

#define MODBUS_TCP_PORT                      502
#define MODBUS_TCP_HEADER_SIZE               7

#define MODBUS_TCP_ADDR_IDX                  6
#define MODBUS_TCP_FUNC_IDX                  7
#define MODBUS_TCP_DATA_IDX                  8

#define MODBUS_RTU_ADDR_IDX                  0
#define MODBUS_RTU_FUNC_IDX                  1
#define MODBUS_RTU_DATA_IDX                  2

#define MODBUS_FUNC_READ_COILS               0x01
#define MODBUS_READ_DISCRETE_INPUTS          0x02
#define MODBUS_FUNC_WRITE_COIL               0x05 
#define MODBUS_FUNC_WRITE_SINGLE_REGISTER    0x06


int modbus_rtu_write_request(int sd, int addr, uint8_t func, uint16_t regaddr, uint16_t regdata);
int modbus_rtu_read_response(int sd, int rspsize, uint8_t *buf, int bufsize);

int modbus_rtu_read_coils_state_fix(int sd, int addr, int start_coil, int num_coils, uint16_t *state);
int modbus_rtu_read_coils_state(int sd, int addr, int start_coil, int num_coils, uint16_t *state);
int modbus_rtu_write_coil(int sd, int addr, int coil, int state);
int modbus_rtu_read_inputs(int sout, int addr, int start_input, int count, uint16_t *state);
int modbus_rtu_write_sigle_register(int sd, int addr, int regaddr, int value);


#endif /* _MODBUS_RTU_H_ */
