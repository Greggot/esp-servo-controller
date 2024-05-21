#pragma once
#include <TCP/server.hpp>
#include <TCP/client.hpp>
 
// std::char_traits<char>;
// std::string;

template<class interface>
class ITraitsPrototype
{ };

template<class interface>
class InterfaceTraits : public ITraitsPrototype<interface>
{ };

template<>
class InterfaceTraits<TCP::client>
{
private:
    TCP::client client;
public:
    void connect(TCP::Address addr) {
        client.Connect(addr);
    }
    void disconnect() {
        client.Disconnect();
    }
    int receive(void* destination, const size_t maxsize) {
        return client.Receive(destination, maxsize);
    }
    int send(const void* source, const size_t size) {
        return client.Send(source, size);
    }

};

template<>
class InterfaceTraits<TCP::server>
{
private:
    TCP::server server;
    size_t client;
public:
    void connect(TCP::Address addr) {
        server.Start(addr);
        client = server.Accept();
        printf("Client: %u\n", client);
    }
    TCP::Address otherside() { 
        const auto& addr = server.ClientAddress(client);
        return TCP::Address{addr.sin_addr.s_addr, addr.sin_port};
    }
    void disconnect() {
        server.Close(client);
    }
    int receive(void* destination, const size_t maxsize) {
        int len =  server.Receive(client, destination, maxsize);
        return len;
    }
    int send(const void* source, const size_t size) {
        return server.Send(client, source, size);
    }
};