#pragma once
#include "net_headers.h"

namespace olc
{
	namespace net
	{
		template<typename T> 
		class tsqueue
		{
		public:
			tsqueue() = default;
			tsqueue(const tsqueue<T>&) = delete;
			virtual ~tsqueue() { clear(); }

		public:
			// Devuelve y mantiene el item enfrente de la cola.
			const T& front()
			{
				std::scoped_lock lock(muxQueue);
				return deqQueue.front();
			}

			// Devuelve y mantiene el item al final de la cola.
			const T& back()
			{
				std::scoped_lock lock(muxQueue);
				return deqQueue.back();
			}

			// Añade un item al frente de la cola.
			void push_front(const T& item)
			{
				std::scoped_lock lock(muxQueue);
				deqQueue.emplace_front(std::move(item));

				std::unique_lock<std::mutex> ul(muxBlocking);
				cvBlocking.notify_one();
			}

			// Añade un item al final de la cola.
			void push_back(const T& item)
			{
				std::scoped_lock lock(muxQueue);
				deqQueue.emplace_back(std::move(item));

				std::unique_lock<std::mutex> ul(muxBlocking);
				cvBlocking.notify_one();
			}

			// Devuelve true si la cola esta vacía.
			bool empty()
			{
				std::scoped_lock lock(muxQueue);
				return deqQueue.empty();
			}

			// Devuelve número de elementos en la cola.
			size_t count()
			{
				std::scoped_lock lock(muxQueue);
				return deqQueue.size();
			}

			// Limpia la cola.
			void clear()
			{
				std::scoped_lock lock(muxQueue);
				return deqQueue.clear();
			}

			// Elimina y devuelve el elemento primero de la cola.
			T pop_front()
			{
				std::scoped_lock lock(muxQueue);
				auto t = std::move(deqQueue.front());
				deqQueue.pop_front();
				return t;
			}

			void wait()
			{
				while (empty())
				{
					std::unique_lock<std::mutex> ul(muxBlocking);
					cvBlocking.wait(ul);
				}
			}
			
		protected:
			std::mutex muxQueue;
			std::deque<T> deqQueue;
			std::condition_variable cvBlocking;
			std::mutex muxBlocking;

		};

	}
}