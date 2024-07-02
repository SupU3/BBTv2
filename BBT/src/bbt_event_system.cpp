#include "bbt_event_system.h"
#include "bbt_log.h"
#include <mutex>
#include <unordered_map>
#include <vector>

// Estructura que empaqueta un puntero a funcion o llamable
// cons u meta nombre. para debug.
struct eventSystemObserver
{
    // Puntero a funcion callable.
    bbt::eventSystem::eventFunc functor;
    // Meta name del callable.
    const char*                 functorName;
};

// Nuestras variables globales.

// para sincronizacion de acceso a la hora de suscribir y broadcastear.
static std::mutex gMut;
// Nuestra tabla asociativa de Eventos -> Observadores
static std::unordered_map<int, std::vector<eventSystemObserver>> gObserversMap;


void bbt::eventSystem::Subscribe(int eventId, eventFunc inFunc, const char* metaName)
{
    // thead safe... hace lo mismo que mutex.lock()... pero se asegura .unlock() al salir del scope.
    std::lock_guard<std::mutex> scopedLock{gMut};

    gObserversMap[eventId].push_back({ inFunc, metaName });

    BBT_LOG( INFO, "Subscribed: %s -> Event ID: %d", metaName, eventId);

}

void bbt::eventSystem::Broadcast(int eventId, const eventParams& eventParams)
{
    std::lock_guard<std::mutex> scopedLock{gMut};

    if(!gObserversMap.contains(eventId))
        return;

    BBT_LOG( INFO, "Broadcast even with ID: %d ", eventId);

    for(auto&& observer : gObserversMap[eventId])
    {
        observer.functor(eventParams);
        BBT_LOG( INFO, "\tCalled: %s", observer.functorName);
    }

}
