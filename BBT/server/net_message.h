#pragma once
#include "net_headers.h"

namespace olc
{
	namespace net
	{
		template <typename T>
		struct message_header
		{
			// El message_header se manda al inicio de todos los mensajes para indicar de que tipo son estos.
			// Utilizando el template podemos usar un "enum class" para asegurarnos de que el mensaje en valido en tiempo de compilación.
			T id{};
			uint32_t size = 0;
		};

		template <typename T>
		struct message
		{
			message_header<T> header{};
			std::vector<uint8_t> body;
			
			// Devuelve el tamaño de todo el paquete en bytes.
			size_t size() const
			{
				return body.size();
			}

			// Override del std::cout - muestra una descripción del mensaje.
			friend std::ostream& operator << (std::ostream& os, const message<T>& msg)
			{
				os << "ID:" << int(msg.header.id) << " SIZE:" << msg.header.size;
				return os;
			}

			// Pushea cualquier tipo de dato dentro del buffer del mensaje.
			template<typename DataType>
			friend message<T>& operator << (message<T>& msg, const DataType& data)
			{
				// Comprobamos el tipo de datos que vamos a pushear.
				static_assert(std::is_standard_layout<DataType>::value, "Data is too complet to be pushed into vector");

				// Valor en cache del tamaño del vector, aquí introduciremos la información.
				size_t i = msg.body.size();

				// Camiamos el tamaño del vector para que se adapte al del mensaje que vamos a enviar.
				msg.body.resize(msg.body.size() + sizeof(DataType));

				// Copiamos físicamente la información dentro del nuevo espacio donde se encuentra del vector. 
				std::memcpy(msg.body.data() + i, &data, sizeof(DataType));

				// Recalculamos el tamaño del mensaje.
				msg.header.size = msg.size();

				// Devuelve el mensaje para poder ser encadenado.
				return msg;
			}

			template<typename DataType>
			friend message<T>& operator >> (message<T>& msg, DataType& data)
			{
				// Comprobamos el tipo de datos que vamos a pushear.
				static_assert(std::is_standard_layout<DataType>::value, "Data is too complet to be pushed into vector");

				// Obtenemos la posición del finla del vector donde empieza el mensaje.
				size_t i = msg.body.size() - sizeof(DataType);

				// Copiamos físicamente los datos desde el vector dentro de la variable del usuario.
				std::memcpy(&data, msg.body.data() + i, sizeof(DataType));

				// Reduce el vector para eliminar los bytes de lectura y resetear la posición.
				msg.body.resize(i);

				// Recalculamos el tamaño del mensaje.
				msg.header.size = msg.size();

				// Devuelve el mensaje para poder ser encadenado.
				return msg;
			}
		};

		// Declaración de la conexión
		template <typename T>
		class connection;

		template <typename T>
		struct owned_message
		{
			std::shared_ptr<connection<T>> remote = nullptr;
			message<T> msg;

			friend std::ostream& operator << (std::ostream& os, const owned_message<T>& msg)
			{
				os << msg.msg;
				return os;
			}
		};
	}
}