#include <stdio.h>
#include <iostream>

#ifdef _WIN32
#define _WIN32_WINNT 0x0A00
#endif

#define ASIO_STANDALONE

#include <boost/asio.hpp>
#include <boost/asio/ts/buffer.hpp>
#include <boost/asio/ts/internet.hpp>

std::vector<char> vBuffer(20 * 1024);

void GrabData(boost::asio::ip::tcp::socket& socket)
{
	socket.async_read_some(boost::asio::buffer(vBuffer.data(), vBuffer.size()),
		[&](std::error_code ec, std::size_t length)
		{
			if (!ec)
			{
				std::cout << "\n\nRead " << length << " bytes\n\n";

				for (int i = 0; i < length; i++)
					std::cout << vBuffer[i];

				GrabData(socket);
			}
		}
	);
}

int main()
{
	boost::system::error_code ec;

	//Crea un "Contexto" 
	boost::asio::io_context context;

	boost::asio::io_context::work idleWork(context);

	std::thread thrContext = std::thread([&]() { context.run(); });

	//Obtener la dirección del lugar dodne queremos conectarnos
	boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::make_address("51.38.81.49", ec), 80);

	//Creamos un socket y le pasamos el contexto
	boost::asio::ip::tcp::socket socket(context);

	//Intentamos conectar
	socket.connect(endpoint, ec);

	if (!ec)
	{
		printf("Conectadoooooooo\n");
	}
	else
	{
		printf("Ñooo :( %s\n", ec.message().c_str());
	}

	if (socket.is_open())
	{
		GrabData(socket);

		std::string sRequest =
			"GET /index.html HTTP/1.1\r\n"
			"Host: example.com\r\n"
			"Connection: close\r\n\r\n";

		socket.write_some(boost::asio::buffer(sRequest.data(), sRequest.size()), ec);

		using namespace std::chrono_literals;
		std::this_thread::sleep_for(20000ms);

		context.stop();
		if (thrContext.joinable()) thrContext.join();

	}

	return 0;
}