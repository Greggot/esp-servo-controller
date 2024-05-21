#pragma once
#include "Types.hpp"
#include <functional>

namespace SkyBlue
{
	/**
	* @brief Robot's part communication protocol 
	*	without connection to lower lever transport protocol (e.g. TCP, UDP, USB...)
	* 
	* @member read/write are callbacks, because there are several Modules at one end
	*	Thus, it's 'server' job to select appropriate module to retransmit data to
	*/
	class Module
	{
	public:
		using requestHandler = std::function<void(const ID&, const void*, unsigned int)>;
		Module() = default;

		// Application polymorphism
		void setRead(requestHandler call) { read = call; }
		void setWrite(requestHandler call) { write = call; }

        void execread(const ID& id, const void* data, unsigned int size) const { if(read) read(id, data, size); }
        void execwrite(const ID& id, const void* data, unsigned int size) const { if(write) write(id, data, size); }
	private:
		requestHandler read, write;
	};
}


// using callback = std::function<void(const void*, size_t)>;
// namespace SkyBlue
// {
//     class Module
//     {
//     protected:
//         const ID identifier;
//         callback RX, TX;
//     public:
//         Module(ID id) : identifier(id) {}

//         virtual void write(const void*, size_t) = 0;
        
//         const ID& getID() { return identifier; }

//         void setTX(callback call) { TX = call; }
//         void setRX(callback call) { RX = call; }

//         void callTX(const void* data, size_t size) { if(TX) TX(data, size); }
//         void callRX(const void* data, size_t size) { if(RX) RX(data, size); }
//     };
// }

// #include <TCP/client.hpp>
// namespace SkyBlue
// {
//     class TCPclientModule : public Module
//     {
//     private:
//         TCP::client& client;
//     public:
//         TCPclientModule(ID&& id, TCP::client& client) 
//             : Module(id), client(client) {}
        
//         void write(const void* data, size_t size) override {
//             client.Send(data, size);
//         }
//     };
// }

// #include <TCP/server.hpp>
// namespace SkyBlue
// {
//      class TCPserverModule : public Module
//     {
//     private:
//         TCP::server& server;
//     public:
//         TCPserverModule(ID&& id, TCP::server& server) 
//             : Module(id), server(server) {}
        
//         void write(const void* data, size_t size) override {
//             server.Send(0, data, size);
//         }
//     };
// }