#pragma once
#include <map>
#include <vector>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "Types.hpp"
#include "Module.hpp"
#include "Collector.hpp"

namespace SkyBlue
{
	template<class type> class Device 
		: public Collector<type>
	{
	private:
		using collector = Collector<type>;
		using collector::tx;
		using collector::rx;
	public:
		std::map<ID, Module*> modules;
		bool islistening = false;
		TaskHandle_t main;
		
		void Report() {
			printf("Report asked, %u ids: ", modules.size());
			std::vector<ID> ids;
			for (auto mod : modules)
			{
				printf("%s(%u) ", typeToString(mod.first.type), mod.first.number);
				ids.push_back(mod.first);
			}
			printf("\n IDs size - %u\n", ids.size());

			collector::tx.id = 0;
			collector::tx.command = command_t::report;
			collector::transmit(&ids[0], sizeof(ID) * ids.size());
		}

		void Execute() {
			printf("Searching %u:%u\n", collector::rx.id.number, (int)collector::rx.id.type);
			auto res = modules.find(collector::rx.id);
			if (res == modules.end())
				return;

			printf("Found module\n");
			const auto& mod = *res->second;
			if (collector::rx.command == command_t::read) {
				mod.execread(collector::rx.id, collector::rx.data, collector::rx.length);
				printf("  Execute read\n");
			}else {
				mod.execwrite(collector::rx.id, collector::rx.data, collector::rx.length);
				printf("  Execute write\n");
			}
		}

		bool IsCommandReport(){
			return collector::rx.command == command_t::report;
		}

		Device() {
			xTaskCreate([](void* arg)
			{
				while (true)
				{
					auto dev = (Device<type>*)arg;
					if(dev->receive() < 0)
					{
						dev->disconnect();
						printf("  Client disconnected...\n");
						dev->listen();
				        printf("  Client connected!\n");
					}

					printf("Received message\n");
					if (dev->IsCommandReport())
						dev->Report();
					else
						dev->Execute();
				}
			}, "MainThread", 5120, this, 0, &main);
			deaf();
		}
		
		// Process receive
		void listen() {
			islistening = true;
			vTaskResume(main);
		}

		void deaf() {
			islistening = false;
			vTaskSuspend(main);
		}
		bool doeslisten() const { return islistening; }

		// Process transmit
		std::vector<ID> report() {
			static std::vector<ID> idvector;
			if (islistening)
				return idvector;

			collector::tx.id = 0;
			collector::transmit(nullptr, 0);
			collector::receive();

			idvector.clear();
			ID* ids = (ID*)collector::rx.data;
			collector::rx.length /= sizeof(ID);
			while (collector::rx.length--)
				idvector.push_back(*ids++);

			return idvector;
		}
		template<class sendtype>
		void write(ID id, const sendtype& data){
			write(id, &data, sizeof(sendtype));
		}
		void write(ID id, const void* data, unsigned int length) {
			collector::tx.id = id;
			collector::tx.command = command_t::write;
			collector::transmit(data, length);
		}
		void read(ID id, const void* request, unsigned int length) {
			collector::tx.id = id;
			collector::tx.command = command_t::read;
			collector::transmit(request, length);
		}

		// process system of modules
		Module* get(ID id) {
			auto res = modules.find(id);
			return res == modules.end() ? nullptr : res->second;
		}
		virtual void add(ID id, Module* mod) {
			modules.insert({ id, mod });
		}
		void remove(ID id) { 
			modules.erase(id); 
		}
		virtual void clear() {
			modules.clear();
		}
	};

	using TCPclientDevice = Device<TCP::client>;
	using TCPserverDevice = Device<TCP::server>;
}
