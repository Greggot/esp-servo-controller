#pragma once
#include "Types.hpp"
#include "InterfaceTraits.hpp"

namespace SkyBlue
{
	// Invariant - size is not limited by buffer size
	// TODO: Add templates for buffer and client
	template<class interface>
    class Collector : public InterfaceTraits<interface>
	{
	protected:
		buffer rx, tx;
        using intrfc = InterfaceTraits<interface>;
	public:
		int receive() {
            return intrfc::receive(&rx, sizeof(rx));
        }
		void transmit(const void* data, size_t size){
            tx.length = size;
            memcpy(tx.data, data, tx.length);
            intrfc::send(&tx, tx.length + BUFFER_HEADER_SiZE());
        }
	};
    
    using TCPinterface = Collector<TCP::server>;
}
