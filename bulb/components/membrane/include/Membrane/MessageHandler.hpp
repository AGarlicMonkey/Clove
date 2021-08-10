#pragma once

#include "Membrane/MessageBase.hpp"

namespace garlic::membrane {
    // clang-format off
    //Delegates
    generic<class MessageType> 
    public delegate void MessageSentHandler(MessageType message);

    //Helper types
    private interface class IMessageDelegateWrapper {
    public:
        virtual void tryInvoke(EditorMessage ^message) = 0;
        virtual void tryInvoke(EngineMessage ^message, System::Windows::Threading::Dispatcher ^editorDispatcher) = 0;

        virtual bool isAlive() = 0;
    };

    generic<class MessageType> 
    private ref class MessageDelegateWrapper : public IMessageDelegateWrapper {
        //VARIABLES
    public:
        System::WeakReference ^target;
        System::Reflection::MethodInfo ^method;

        //FUNCTIONS
    public:
        virtual void tryInvoke(EditorMessage ^message) {
            if (message->GetType() == MessageType::typeid) {
                System::Object ^invokeTarget{ target->Target }; //Storing the target here prevents a race condition (instead of doing WeakReference::IsAlive then accessing)
                array<System::Object ^> ^parameters = gcnew array<System::Object ^>{1};

                parameters[0] = (MessageType)message;

                if (invokeTarget != nullptr){
                    method->Invoke(invokeTarget, parameters);
                }
            }
        }

        virtual void tryInvoke(EngineMessage ^message, System::Windows::Threading::Dispatcher ^editorDispatcher) {
            if (message->GetType() == MessageType::typeid) {
                //When sending events to WPF we need to use the dispatcher on the editor thread as WPF does not allow controls to be updated outside of the thread they are created in.
                editorDispatcher->BeginInvoke(gcnew MessageSentHandler<EngineMessage ^>(this, &MessageDelegateWrapper<MessageType>::invokeEditorMessage), message);
            }
        }

        virtual bool isAlive() {
            return target->IsAlive;
        }

    private:
        void invokeEditorMessage(EngineMessage ^message) {
            System::Object ^invokeTarget{ target->Target }; //Storing the target here prevents a race condition (instead of doing WeakReference::IsAlive then accessing)
            array<System::Object ^> ^parameters = gcnew array<System::Object ^>{1};

            parameters[0] = (MessageType)message;

            if (invokeTarget != nullptr){
                method->Invoke(invokeTarget, parameters);
            }
        }
    };

    public ref class MessageHandler {
    private:
        static System::Object^ editorLock{ gcnew System::Object() };

        static System::Collections::Generic::List<IMessageDelegateWrapper ^> ^delegates{ gcnew System::Collections::Generic::List<IMessageDelegateWrapper^> };

        static System::Collections::Generic::List<EditorMessage ^> ^editorMessages{ gcnew System::Collections::Generic::List<EditorMessage ^> };
        static System::Collections::Generic::List<EngineMessage ^> ^engineMessages{ gcnew System::Collections::Generic::List<EngineMessage ^> };
    public:
        generic<class MessageType> 
        static void bindToMessage(MessageSentHandler<MessageType> ^handler);

        generic<class MessageType>
        static void sendMessage(MessageType message);

        //Flushes the message queue for Editor -> Engine messages
        static void flushEditor();
        //Flushes the message queue for Engine -> Editor messages
        static void flushEngine(System::Windows::Threading::Dispatcher ^editorDispatcher);
    };
    // clang-format on
}