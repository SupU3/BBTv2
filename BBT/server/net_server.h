#pragma once

#include "net_headers.h"
#include "net_tsqueue.h"
#include "net_message.h"
#include "net_connection.h"

namespace olc
{
	namespace net
	{
		template<typename T>
		class server_interface
		{
		public:
			server_interface(uint32_t port) : m_asioAcceptor(m_asioContext, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port))
			{

			}

			virtual ~server_interface()
			{
				Stop();
			}

			bool Start()
			{
				try
				{
					WaitForClientConnection();

					m_threadContext = std::thread([this]() { m_asioContext.run(); });


				}
				catch(std::exception& e)
				{
					std::cout << "[SERVER] Exception: " << e.what() << "\n";
					return false;
				}

				std::cout << "[SERVER] Started :3\n";
				return true;
			}

			void Stop()
			{
				// Cerramos el contexto.
				m_asioContext.stop();

				if (m_threadContext.joinable()) m_threadContext.join();

				std::cout << "[SERVER] Stoped >:(\n";
			}

			// ASINC - Le decimos a asio que espere a una conexión.
			void WaitForClientConnection()
			{
				m_asioAcceptor.async_accept(
					[this](std::error_code ec, boost::asio::ip::tcp::socket socket)
					{ 
						if (!ec)
						{
							std::cout << "[SERVER] New Connection:" << socket.remote_endpoint() << "\n";

							std::shared_ptr<connection<T>> newconn = std::make_shared<connection<T>>(connection<T>::owner::server, m_asioContext, std::move(socket), m_qMessagesIn);

							//Podemos denegar alguna conexión si queremos.
							if (OnClientConnect(newconn))
							{
								// Si no la denegamos, la añadimos a la cola de conexiones.
								m_deqConnections.push_back(std::move(newconn));

								m_deqConnections.back()->ConnectToClient(nIDCounter++);

								std::cout << "[" << m_deqConnections.back()->GetID() << "] Connection Approved\n";

							}
							else
							{
								std::cout << "[------] Connection Denied\n";
							}

						}
						else
						{
							// Ha ocurrido un error durante la valildación.
							std::cout << "[SERVER] New Connection Error:" << ec.message() << "\n";
						}

						WaitForClientConnection();
					});
			}

			// Manda un mensaje a un cliente en específico.
			void MessageClient(std::shared_ptr<connection<T>> client, const message<T>& msg)
			{
				if (client && client->IsConnected())
				{
					client->Send(msg);
				}
				else
				{
					OnClientDisconnect(client);
					client.reset();
					m_deqConnections.erase(std::remove(m_deqConnections.begin(), m_deqConnections.end(), client), m_deqConnections.end());

				}
			}

			// Manda un mensaje a TODOS los clientes conectados.
			void MessageAllClients(const message<T>& msg, std::shared_ptr<connection<T>> pIgnoreClient = nullptr)
			{
				bool bInvalidClientExists = false;

				for (auto& client : m_deqConnections)
				{
					if (client && client->IsConnected())
					{
						if (client != pIgnoreClient)
							client->Send(msg);
					}
					else
					{
						OnClientDisconnect(client);
						client.reset();
						bInvalidClientExists = true;
					}
				}
				
				if(bInvalidClientExists)
					m_deqConnections.erase(std::remove(m_deqConnections.begin(), m_deqConnections.end(), nullptr), m_deqConnections.end());
			}

			void Update(size_t nMaxMessages = -1, bool bWait = false)
			{
				if (bWait) m_qMessagesIn.wait();

				size_t nMessageCount = 0;
				while (nMessageCount < nMaxMessages && !m_qMessagesIn.empty())
				{
					// Coge el primer mensaje en la cola.
					auto msg = m_qMessagesIn.pop_front();

					// Se lo pasamos al manejador de mensajes.
					OnMessage(msg.remote, msg.msg);

					// 
					nMessageCount++;
				}
			}

		protected:
			// Se llama al conectarseun cliente, podemos vetar la conexión si devolvemos false. 
			virtual bool OnClientConnect(std::shared_ptr<connection<T>> client)
			{
				return false;
			}

			// Se llama cuando un cliente se ha desconectado. 
			virtual void OnClientDisconnect(std::shared_ptr<connection<T>> client)
			{
				
			}

			// Se llama cuando recibimos un mensaje. 
			virtual void OnMessage(std::shared_ptr<connection<T>> client, message<T>& msg)
			{
				
			}

		protected:
			// Cola thread safe para los mensajes entrantes.
			tsqueue<owned_message<T>> m_qMessagesIn;

			// Contenedor de conexiones validas.
			std::deque<std::shared_ptr<connection<T>>> m_deqConnections;

			boost::asio::io_context m_asioContext;
			std::thread m_threadContext;

			// Esto necesita un contexto de asio.
			boost::asio::ip::tcp::acceptor m_asioAcceptor;

			// Los clientes tendrán un identificador único dentro del sistema, así evitamos usar su ip y mandarla a otros clientes. 
			uint32_t nIDCounter = 10000;



		};
	}
}