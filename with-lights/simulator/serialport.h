//
// Created by sabinka on 22. 2. 2022.
//

#ifndef SIMULATOR_SERIALPORT_H
#define SIMULATOR_SERIALPORT_H


using namespace std;

#include <string>
#include <termios.h>

/** Class to interface serialport under Linux and macOS */
class SerialPort
{
public:
    /** SerialPort
     *  @param[in] port Path to the serialport.
     *  @param[in] baudRate Serial baud rate configuration.
     *  @param[in] stopBits Number of stop bits (1 or 2).
     */
    SerialPort(const char *port, unsigned baudRate = 57600, unsigned stopBits = 1,
               bool canonical_mode = false);
    ~SerialPort();

    /** Write a buffer.
     * @param[in] buf Output buffer.
     * @param[in] nBytes Number of bytes to be written.
     * @return Number of bytes read, negative on error.
     */
    int write(const char *buf, size_t nBytes);

    /** Wrire a std::string.
     * @param[in] string String to be written.
     * @return Number of bytes written, negative on error.
     */
    int write(const std::string& string);

    /** Read nBytes into a buffer.
     * @param[out] buf Buffer read.
     * @return Number of bytes read, negative on error.
     */
    int read(char *buf, size_t nBytes);

    /** Read a line terminated by a newline character (CR or LF).
     * @param[out] line Line read, NULL-terminated.
     * @param[in] nmax Capacity of line buffer inclusive NULL-termination.
     * @return Number of bytes read (without NULL-termination), negative on error.
     */
    int readLine(char *line, size_t nmax);

    /** Read a line terminated by a newline character (CR or LF).
     * @param[out] line Line read as a std::string.
     * @return Number of bytes read, negative on error.
     */
    int readLine(std::string &line);


protected:
    int mFileDesc; /**< File descriptor */

private:
    struct termios mOriginalTTYAttrs; /**< Original termios options */
};


#endif //SIMULATOR_SERIALPORT_H
