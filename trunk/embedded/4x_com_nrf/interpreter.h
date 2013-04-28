/* interpreter.h
 * 2012-9-13 16:23:49
 * js200300953
 */

 #ifndef __FILE_INTERPRETER_H__
 #define __FILE_INTERPRETER_H__


void inpertreter_checkFrame(void);
void interpreter_rf2serial(const void * rfData,unsigned char len);
void serial_received(unsigned char data);

 #endif // __FILE_INTERPRETER_H__
