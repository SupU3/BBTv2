AppCore: 

	static bool 		running;
	static const char* 	Name;
	static bbtRenderer 	renderer;
	static serviceProvider  clientProvider;

	static clientProfile	myself;

	myself.username = "Mordi";
	myself.ip       = ....;
	myself.uid      = hash(username + ip);


	request_connection(myself);

	hilo aparte:
	
	futuro respuesta = ;
	{
		...
		...
		...
		...

		todo bien!!!
		serviceProvider::init();

		{
			serviceProvider::poll_messages();

			// Cola de mensajes...
			// thread safe.

			// Message queue...
			// Mensajes los chuplamos.
		}

		todo mal!!!
		return true : false;
	}


	// LOGIN:
	// username: ...str;
	// password: ...str;

	class serviceProvider
	{
		static void init();
	}

	class renderer
	{

		static void process_input()
		{
			SDL_PollEvents

			if(event.type == QUIT)
				BBT_EVENT_BROADCAST_EMPTY(QuitEvent);
			if(event.type == INPUT)
				bbt::eventSystem::Broadcast(QuitEvent, {&event.key});
			if(event.type == LOWMEMORY)
				BBT_EVENT_BROADCAST_EMPTY(LowMemEvent);
		}
		static void present()
		{
			clear();
			render_ui()
			{
				if(!provider::connected())
				{
					render_connection_window();
				} else
				{
					render_chat(); // chat...
				}
			}
			swapBuffers();
		}
	}

	void EntryPoint_Quit_Observer(eventParams hola)
	{
		running = false;
	}

	void OnLowMemory_Observer(eventParams hola)
	{

	}

	BBT_EVENT_SUBSCRIBE(QuitEvent, EntryPoint_Quit_Observer);
	BBT_EVENT_SUBSCRIBE(LowMemEvent, OnLowMemory_Observer);

	// No podemos bloquear.


	// Procesa eventos.
	WhileLoop( running )
	{
		renderer::process_input();
		renderer::present();
	}