#pragma once

#include "net_headers.h"
#include "net_tsqueue.h"
#include "net_message.h"

namespace olc
{
	namespace net
	{
		template<typename T>
		class server_interface;

		template<typename T>
		class connection : public std::enable_shared_from_this<connection<T>>
		{
		public:
			enum class owner
			{
				server,
				client
			};

		public:
			connection(owner parent, boost::asio::io_context& asioContext, boost::asio::ip::tcp::socket socket, tsqueue<owned_message<T>>& qIn)
				: m_asioContext(asioContext), m_socket(std::move(socket)), m_qMessagesIn(qIn)
			{
				m_nOwnerType = parent;

				// Construir los datos para validar. 
				if (m_nOwnerType == owner::server)
				{
					// Conexión Servidor --> Cliente
					// Cogemos del timpo del sistema en ese momento un valor.
					m_nHandshakeOut = uint64_t(std::chrono::system_clock::now().time_since_epoch().count());

					// Precalculamos el resultado que nos tiene que enviar el cliente para darlo como una conexión valida.
					m_nHandshakeCheck = scramble(m_nHandshakeOut);
				}
				else
				{
					// Conexión Cliente --> Servidor
					m_nHandshakeIn = 0;
					m_nHandshakeOut = 0;
				}
			}

			virtual ~connection()
			{}

			uint32_t GetID() const
			{
				return id;
			}

		public:
			void ConnectToClient(olc::net::server_interface<T>* server, uint32_t uid = 0)
			{
				if (m_nOwnerType == owner::server)
				{
					if (m_socket.is_open())
					{
						id = uid;

						WriteValidation();

						ReadValidation(server);
					}
				}
			}

			void ConnectToServer(const boost::asio::ip::tcp::resolver::results_type& endpoints)
			{
				// Solo si eres un cliente puedes conectarte  al servidor.
				if (m_nOwnerType == owner::client)
				{
					// ASio intenta conectarse a un endpoint.
					boost::asio::async_connect(m_socket, endpoints,
						[this](std::error_code ec, boost::asio::ip::tcp::endpoint endpoint)
						{
							if (!ec)
							{
								// Lo primero que haremos sera mandar desde el servidor un paquete para validar.
								ReadValidation();


							}
						});
				}
			}
			
			void Disconnect()
			{
				if (IsConnected())
					boost::asio::post(m_asioContext, [this]() { m_socket.close(); });
			}

			bool IsConnected() const
			{
				return m_socket.is_open();
			}

			void StartListening()
			{

			}

		public:
			void Send(const message<T>& msg)
			{
				boost::asio::post(m_asioContext,
					[this, msg]()
					{
						bool bWritingMessage = !m_qMessagesOut.empty();
						m_qMessagesOut.push_back(msg);

						if (!bWritingMessage)
						{
							WriteHeader();
						}
						
					});
			}

		private:
			// ASYNC Contexto listo para leer el header de un mensaje.
			void ReadHeader()
			{
				boost::asio::async_read(m_socket, boost::asio::buffer(&m_msgTemporaryIn.header, sizeof(message_header<T>)),
					[this](std::error_code ec, std::size_t length)
					{
						if (!ec)
						{
							if (m_msgTemporaryIn.header.size > 0)
							{
								m_msgTemporaryIn.body.resize(m_msgTemporaryIn.header.size);
								ReadBody();
							}
							else
							{
								AddToIncomingMessageQueue();

							}
						}
						else
						{
							std::cout << "[" << id << "] Read Header Fail.\n";
							m_socket.close();
						}
					});
			}

			// ASYNC Contexto listo para leer el cuerpo de un mensaje.
			void ReadBody()
			{
				boost::asio::async_read(m_socket, boost::asio::buffer(m_msgTemporaryIn.body.data(), m_msgTemporaryIn.body.size()),
					[this](std::error_code ec, std::size_t length)
					{
						if (!ec)
						{
							AddToIncomingMessageQueue();
						}
						else
						{
							// Igual que en el header
							std::cout << "[" << id << "] Read Body Fail.\n";
							m_socket.close();
						}
					});
			}

			// ASYNC Contexto listo para escibir el header de un mensaje.
			void WriteHeader()
			{
				boost::asio::async_write(m_socket, boost::asio::buffer(&m_qMessagesOut.front().header, sizeof(message_header<T>)),
					[this](std::error_code ec, std::size_t length)
					{
						if (!ec)
						{
							if (m_qMessagesOut.front().body.size() > 0)
							{
								WriteBody();
							}
							else
							{
								m_qMessagesOut.pop_front();

								if (!m_qMessagesOut.empty())
								{
									WriteHeader();
								}
							}
						}
						else
						{
							// Igual que en el header
							std::cout << "[" << id << "] Write Header Fail.\n";
							m_socket.close();
						}
					});
			}

			// ASYNC Contexto listo para escribir el cuerpo de un mensaje.
			void WriteBody()
			{
				boost::asio::async_write(m_socket, boost::asio::buffer(m_qMessagesOut.front().body.data(), m_qMessagesOut.front().body.size()),
					[this](std::error_code ec, std::size_t length)
					{
						if (!ec)
						{
							m_qMessagesOut.pop_front();

							if (!m_qMessagesOut.empty())
							{
								WriteHeader();
							}
						}
						else
						{
							std::cout << "[" << id << "] Write Body Fail.\n";
							m_socket.close();
						}
					});
			}

			void AddToIncomingMessageQueue()
			{
				if (m_nOwnerType == owner::server)
					m_qMessagesIn.push_back({ this->shared_from_this(), m_msgTemporaryIn });
				else
					m_qMessagesIn.push_back({ nullptr, m_msgTemporaryIn });

				ReadHeader();
			}

			// "Encripta" los datos.
			uint64_t scramble(uint64_t nInput)
			{
				uint64_t out = nInput ^ 0xAEBCDEB013267DF0;
				out = (out & 0xF0F0F0F0F0F0F0) >> 4 | (out & 0x0F0F0F0F0F0F0F) << 4;
				return out ^ 0xDACBCCFEA0000001; // Al actualizar una nueva versión del cliente hay que cambiar este número para que clientes anteriores no sean compatibles.
			}

			// ASYNC La usaremos para escribir desde el servidor o cliente para escribir los mensajes de validación.
			void WriteValidation()
			{
				boost::asio::async_write(m_socket, boost::asio::buffer(&m_nHandshakeOut, sizeof(uint64_t)),
					[this](std::error_code ec, std::size_t length)
					{
						if (!ec)
						{
							if (m_nOwnerType == owner::client)
								ReadHeader();
						}
						else
						{
							m_socket.close();
						}
					});
			}

			void ReadValidation(olc::net::server_interface<T>* server = nullptr)
			{
				boost::asio::async_read(m_socket, boost::asio::buffer(&m_nHandshakeIn, sizeof(uint64_t)),
					[this, server](std::error_code ec, std::size_t length)
					{
						if (!ec)
						{
							if (m_nOwnerType == owner::server)
							{
								if (m_nHandshakeIn == m_nHandshakeCheck)
								{
									// Si coincide el reto recibido desde el usuario con el que hemos calculado nosotros se valida correctamente.
									std::cout << "Client Validated" << std::endl;
									server->OnClientValidated(this->shared_from_this());

									// Una vez validado espera a recibir algo que escribir.
									ReadHeader();
								}
								else
								{
									// El cliente no ha resuelto el reto correctamente (no es compatible o no es un cliente que queramos).
									std::cout << "Client Disconnected (Fail Validation)" << std::endl;
									m_socket.close();
								}
							}
							else
							{
								// La conexión es de un cliente, por lo que ha de resolver un reto para validarse.
								m_nHandshakeOut = scramble(m_nHandshakeIn);

								// Escribimos el resultado.
								WriteValidation();
							}
						}
						else
						{
							// Igual que en el header
							std::cout << "Client Disconnected (ReadValidation)" << std::endl;
							m_socket.close();
						}
					});
			}

		protected:
			// Cada conexión esta conectada a un único socket.
			boost::asio::ip::tcp::socket m_socket;

			// Este contexto esta compartido con toda la intancia de asio.
			boost::asio::io_context& m_asioContext;

			// Esta cola contiene todos los mensajes que se van a enviar por el socket.
			tsqueue<message<T>> m_qMessagesOut;

			// Esta cola guarda todos los mensajes recibidos desde el socket. 
			tsqueue<owned_message<T>>& m_qMessagesIn;
			message<T> m_msgTemporaryIn;

			owner m_nOwnerType = owner::server;
			uint32_t id = 0;

			// Validación del Handshake.
			uint64_t m_nHandshakeOut = 0;
			uint64_t m_nHandshakeIn = 0;
			uint64_t m_nHandshakeCheck = 0;
		};
	}
}
