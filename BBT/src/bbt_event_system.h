#ifndef __BBT_EVENT_SYSTEM_H__
#define __BBT_EVENT_SYSTEM_H__ 1

#include <functional>

namespace bbt
{

	struct eventParams
	{
		void* userData	{nullptr};
	};

	static constexpr int QuitEvent  	= -1;
	static constexpr int InputEvent 	= -2;

	class eventSystem
	{
	public:

		// Type definition for event functions "syntax sugar"
		using eventFunc = std::function<void(const eventParams& params)>;
		// Esto "typedef void(*eventFunc)(eventParams&)" significa lo de arriba ;

		static void Subscribe(int eventId, eventFunc inFunc, const char* metaName);

		template<typename obj_t>
		static void Subscribe(int eventId, const obj_t* eventInst, void (obj_t::* instMethod)(const eventParams&), const char* metaName)
		{
			// Funcion lambda construida donde invovamos el metodo de clase
			// de esta forma reutilizamos nuestro sistema de bindear funciones
			auto CallbackHandle
			= [eventInst, instMethod](const eventParams& inParams)
			{
				(eventInst->*instMethod)(inParams);
			};

			Subscribe(eventId, CallbackHandle, metaName);
		}

		static void Broadcast(int eventId, const eventParams& eventParams);

		//template<typename obj_t>
		//static void Subscribe(int eventId, obj_t* instance, void obj_t::*(eventParams& params) objFunc, const char* metaName);

		// QUIT
	};

#define BBT_EVENT_SUBSCRIBE( _EventID, _ToBind ) \
	::bbt::eventSystem::Subscribe( _EventID, _ToBind, #_ToBind )

#define BBT_EVENT_BROADCAST_EMPTY( _EventID ) \
	::bbt::eventSystem::Broadcast( _EventID, {} )

}
#endif