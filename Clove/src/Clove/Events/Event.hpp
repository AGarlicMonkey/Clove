#pragma once

#include <queue>

namespace clv{
	enum EventType{
		ET_None = 0,
		WindowClose, WindowResize, WindowFocus, WindowLostFocus, WindowMoved,
		AppTick, AppUpdate, AppRender,
		KeyPressed, KeyReleased, KeyTyped,
		MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled
	};

	enum EventCategory{
		EC_None = 0,
		EventCategoryApplication	= BIT(0),
		EventCategoryInput			= BIT(1),
		EventCategoryKeyboard		= BIT(2),
		EventCategoryMouse			= BIT(3),
		EventCategoryMouseButton	= BIT(4)
	};


	//Macro containing overrides that deal with the event type
	#if CLV_PLATFORM_WINDOWS
		#define EVENT_CLASS_TYPE(type)	static EventType getStaticType() { return EventType::##type; }\
										virtual EventType getEventType() const override { return getStaticType(); }\
										virtual const char* getName() const override { return #type; }
	#else
		#define EVENT_CLASS_TYPE(type)	static EventType getStaticType() { return EventType::type; }\
										virtual EventType getEventType() const override { return getStaticType(); }\
										virtual const char* getName() const override { return #type; }
	#endif
	
	//Macro containing overrides that deal with the event category
	#define EVENT_CLASS_CATEGORY(category) virtual int getCategoryFlags() const override { return category; }

	namespace evt{
		//I Want:
		//A queue of events that happen
		//certain point in game loop this queue will get processed
		//Any listener can, at any point, bind to these events
		//This listener has to return that it has handle the event
		//no more listeners will be notified once the event is handled
		//ideally I'd like it all to be type deductable (no casts) but am willing to compromise

		//So what would I need?
		//A disptacher class which would:
		//	collect all of the listeners
		//	receive events and either fire these immediately or queue them
		//A listener class that wraps a delegate which binds to a function on the actual listener
		//I don't think I'll need an event class

		enum class DispatchType{
			immediate,
			deferred,
		};

		enum class HandledType{
			handled_continue,
			handled_stop,
			unhandled

			/*continue,
			stop*/
		};

		//Do I need this or just take the std::function?
		template<typename EventType>
		class Listener{
			//VARIABLES
		/*private:*/
		public:
			SingleCastDelegate<HandledType(EventType&)> del;
		};

		class InternalEventDispatcherBase{
		public:
			virtual void processEventQueue() = 0;
		};

		template<typename EventType>
		class InteralEventDispatcher : public InternalEventDispatcherBase{
			//VARIABLES
		private:
			std::queue<EventType> eventQueue;
			std::vector<Listener<EventType>> listeners;

			static bool initialised;
			static InteralEventDispatcher<EventType>* instance;

			//FUNCTIONS
		public:
			InteralEventDispatcher(){
				initialised = true;
				instance = this;
			}

			~InteralEventDispatcher(){
				//Doesn't look like this is ever called

				CLV_DEBUG_BREAK;

			}

			static bool isInitialised(){
				return initialised;
			}

			static InteralEventDispatcher<EventType>* get(){
				return instance;
			}

			template<typename Function, typename Object>
			void bind(Function&& function, Object* object){
				//bind lambdas?

				Listener<EventType> listener{};
				listener.del.bind(std::forward<Function>(function), object);
				listeners.emplace_back(std::move(listener));
			}

			//unbind?

			void dispatch(EventType&& event, DispatchType dispatchType){
				if(dispatchType == DispatchType::deferred){
					eventQueue.push(event);
				} else{
					doDispatch(std::forward<EventType>(event));
				}
			}

			virtual void processEventQueue() override{
				while(!eventQueue.empty()){
					doDispatch(eventQueue.front());
					eventQueue.pop();
				}
			}

		private:
			template<typename EventType>
			void doDispatch(EventType&& event){
				for(auto& listener : listeners){
					const HandledType handledState = listener.del.broadcast(event);
					if(handledState == HandledType::handled_stop){
						break;
					}
				}
			}
		};

		template<typename EventType> bool InteralEventDispatcher<EventType>::initialised = false;
		template<typename EventType> InteralEventDispatcher<EventType>* InteralEventDispatcher<EventType>::instance = nullptr;


		class EventDispatcher{
			//VARIABLES
		private:
			static std::vector<std::unique_ptr<InternalEventDispatcherBase>> dispatchers;

			//FUNCTIONS
		public:
			template<typename EventType, typename Function, typename Object>
			static void bind(Function&& function, Object* object){
				if(!InteralEventDispatcher<EventType>::isInitialised()){
					dispatchers.emplace_back(std::make_unique<InteralEventDispatcher<EventType>>());
				}
				InteralEventDispatcher<EventType>::get()->bind(std::forward<Function>(function), object);
			}

			//unbind?

			template<typename EventType>
			static void dispatch(EventType&& event, DispatchType dispatchType){
				if(InteralEventDispatcher<EventType>::isInitialised()){
					InteralEventDispatcher<EventType>::get()->dispatch(std::forward<EventType>(event), dispatchType);
				}
			}

			static void processEventQueue(){
				for(auto& dispatcher : dispatchers){
					dispatcher->processEventQueue();
				}
			}
		};
	}

	//OLD--------------------------------------------------
	class Event{
		friend class EventDispatcher;

		//VARIABLES
	protected:
		bool handled = false;

		//FUNCTIONS
	public:
		virtual EventType getEventType() const = 0;
		virtual const char* getName() const = 0;
		virtual int getCategoryFlags() const = 0;

		bool isHandled() const;

		virtual std::string toString() const;

		bool isInCategory(EventCategory category);
	};

	class EventDispatcher{
		template<typename EventType>
		using EventReceiverFn = std::function<bool(EventType&)>;

		//VARIABLES
	private:
		Event& event;

		//FUNCTIONS
	public:
		EventDispatcher(Event& inEvent);

		template<typename EventType>
		bool dispatch(EventReceiverFn<EventType> func);
	};

	std::ostream& operator<<(std::ostream& os, const Event& e);
}

#include "Event.inl"