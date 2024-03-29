#include "Membrane/MessageHandler.hpp"

#include "Membrane/MembraneLog.hpp"

#include <msclr/lock.h>
#include <msclr/marshal_cppstd.h>

namespace membrane {
    // clang-format off
    generic<class MessageType> 
    void MessageHandler::bindToMessage(MessageSentHandler<MessageType> ^ handler) {
        MessageDelegateWrapper<MessageType> ^wrapper{ gcnew MessageDelegateWrapper<MessageType> };
        wrapper->target = gcnew System::WeakReference{ handler->Target };
        wrapper->method = handler->Method;

        if (!isSendingMessages){
            delegates->Add(wrapper);
        } else{
            pendingDelegates->Add(wrapper);
        }
    }

    generic<class MessageType>
    void MessageHandler::sendMessage(MessageType message){
        System::Type ^const baseType{ message->GetType()->BaseType };        

        if (baseType == EditorMessage::typeid) {
            msclr::lock scopedLock{ editorLock };
            editorMessages->Add((EditorMessage^)message);
        } else if (baseType == EngineMessage::typeid) {
            engineMessages->Add((EngineMessage^)message);
        } else {
            CLOVE_LOG(Membrane, clove::LogLevel::Warning, "{0} called with neither an Editor or Engine message! Base type is: {1}", CLOVE_FUNCTION_NAME_PRETTY, msclr::interop::marshal_as<std::string>(baseType->Name));
        }
    }

    void MessageHandler::flushEditor() {
        isSendingMessages = true;

        //All messages are dispatched from the engine thread so we need to lock the editor thread incase it sends a message while we're flushing
        msclr::lock scopedLock{ editorLock };

        System::Collections::Generic::List<IMessageDelegateWrapper ^> ^invalidDelegates{ gcnew System::Collections::Generic::List<IMessageDelegateWrapper^> };

        for each(EditorMessage ^message in editorMessages) {
            for each(IMessageDelegateWrapper^ wrapper in delegates){
                if (wrapper->isAlive()){
                    wrapper->tryInvoke(message);
                } else{
                    invalidDelegates->Add(wrapper);
                }
            }
        }

        for each(IMessageDelegateWrapper^ wrapper in invalidDelegates){
            delegates->Remove(wrapper);
        }

        editorMessages->Clear();

        delegates->AddRange(pendingDelegates);
        pendingDelegates->Clear();

        isSendingMessages = false;
    }

    void MessageHandler::flushEngine(System::Windows::Threading::Dispatcher ^editorDispatcher) {
        isSendingMessages = true;

        System::Collections::Generic::List<IMessageDelegateWrapper ^> ^invalidDelegates{ gcnew System::Collections::Generic::List<IMessageDelegateWrapper^> };

        for each(EngineMessage ^message in engineMessages) {            
            for each(IMessageDelegateWrapper^ wrapper in delegates){
                if (wrapper->isAlive()){
                    wrapper->tryInvoke(message, editorDispatcher);
                } else{
                    invalidDelegates->Add(wrapper);
                }
            }
        }

        for each(IMessageDelegateWrapper^ wrapper in invalidDelegates){
            delegates->Remove(wrapper);
        }

        engineMessages->Clear();

        delegates->AddRange(pendingDelegates);
        pendingDelegates->Clear();

        isSendingMessages = false;
    }
    // clang-format on
}