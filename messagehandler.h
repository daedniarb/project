#ifndef MESSAGEHANDLER_H
#define MESSAGEHANDLER_H

/** Message handler is the low-level message handler for messages
    to/from the server. It handles
    transmission/reception of low-level message units.
*/

#include "connectionclosedexception.h"
#include "connection.h"
#include "protocol.h"
#include <string>
#include <vector>

namespace client_server {

    typedef std::vector<int> Logger;
    using protocol::Protocol;

    struct ProtocolViolationException{};

    class MessageHandler {
    public:
	/** Create a message handler.
	    @param c The connection to use messages
	*/
	MessageHandler(Connection* c);

	/** Basic destructor
	 */
	~MessageHandler() {}

	/** Set the log window to use.
	    @param logWindow The log window
	*/
//	void setLogWindow(const Logger &log);

	/** Transmit a code (a constant from the Protocol class).
	    @param code The code to transmit.
	    @throws ConnectionClosedException If the server died
	*/
	void sendCode(int code) const throw(ConnectionClosedException);

	/** Transmit an int value, according to the protocol.
	    @param value The value to transmit
	    @throws ConnectionClosedException If the server died
	*/
	void sendInt(int value) const throw(ConnectionClosedException);

	/** Transmit an int parameter, according to the protocol.
	    @param value The parameter to transmit
	    @throws ConnectionClosedException If the server died
	*/
	void sendIntParameter(int param) const throw(ConnectionClosedException);

	/** Transmit a string parameter, according to the protocol.
	    @param value The parameter to transmit
	    @throws ConnectionClosedException If the server died
	*/
	void sendStringParameter(std::string param) const throw(ConnectionClosedException);

	/** Receive a command code or an error code from the server.
	    @return The code
	    @throws ConnectionClosedException If the server died
	*/
	int recvCode() const throw(ConnectionClosedException);

	/** Receive an int value from the server.
	    @return The int value
	    @throws ConnectionClosedException If the server died
	*/
	int recvInt() const throw(ConnectionClosedException);

	/** Receive an int parameter from the server.
	    @return The parameter value
	    @throws ConnectionClosedException If the server died
	*/
	int recvIntParameter() const throw(ConnectionClosedException, ProtocolViolationException);

	/** Receive a string parameter from the server.
	    @return The parameter value
	    @throws ConnectionClosedException If the server died
	*/
	std::string recvStringParameter() const throw(ConnectionClosedException, ProtocolViolationException);

    private:

	Connection* conn;    	// the connection
//	Logger &logWindow; 			// the log window

	void sendByte(int code) const throw(ConnectionClosedException);
	int recvByte() const throw(ConnectionClosedException);
    };

} // End namespace

#endif
