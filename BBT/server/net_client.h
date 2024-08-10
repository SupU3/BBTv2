#pragma once

#include "net_headers.h"
#include "net_tsqueue.h"
#include "net_message.h"
#include "net_connection.h"

namespace olc
{
	namespace net
	{
		template <typename T>
		class client_interface
		{
		public:
			client_interface()
			{}

			virtual ~client_interface()
			{
				// Si el cliente es destruido, siempre intentar desconectarlo del servidor.
				Disconnect();
			}

		public:
			// Conecta con el servidor usando el hostname/dirección ip y puerto
			bool Connect(const std::string& host, const uint16_t port)
			{
				try
				{
					// Resuelve un hostname/dirección ip en una dirección física real.
					boost::asio::ip::tcp::resolver resolver(m_context);
					boost::asio::ip::tcp::resolver::results_type endpoints = resolver.resolve(host, std::to_string(port));

					// Crear la conexión.
					m_connection = std::make_unique<connection<T>>(connection<T>::owner::client, m_context, boost::asio::ip::tcp::socket(m_context), m_qMessagesIn);

					// Decirle al objeto m_connection que se conecte al servidor.
					m_connection->ConnectToServer(endpoints);

					// Crea el hilo donde se ejecutará Asio
					thrContext = std::thread([this]() { m_context.run(); });

				}
				catch (std::exception& e)
				{
					std::cout << "Client Exception: " << e.what() << "\n";
					return false;
				}

			}

			// Desconecta al cliente del servidor.
			void Disconnect()
			{
				// Si la conexión existe y esta conectado entonces...
				if (IsConnected())
				{
					// ... cerramos la conexión.
					m_connection->Disconnect();
				}

				// Si no paramos el hilo del contexto de asio. 
				m_context.stop();
				if (thrContext.joinable())
					thrContext.join();

				// Destruimos el objeto de la conexión que está caro.
				m_connection.release();
			}

			// Comprueba si el cliente está conectado al servidor.
			bool IsConnected()
			{
				if (m_connection)
					return m_connection->IsConnected();
				else
					return false;
			}

		public:
			// Manda mensaje al servidor
			void Send(const message<T>& msg)
			{
				if (IsConnected())
					m_connection->Send(msg);
			}

			// Recupera la cola de mensajes del servidor.
			tsqueue<owned_message<T>>& Incoming()
			{
				return m_qMessagesIn;
			}

		protected:
			boost::asio::io_context m_context;
			std::thread thrContext;

			std::unique_ptr<connection<T>> m_connection;
		private:
			// Esta es la cola thread safe que manda el servidor.
			tsqueue<owned_message<T>> m_qMessagesIn;
		};
	}
}