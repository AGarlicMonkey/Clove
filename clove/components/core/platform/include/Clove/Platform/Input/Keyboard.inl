namespace clove {
    template<typename T>
    void Keyboard::Dispatcher::trimBuffer(std::queue<T> &buffer) {
        while(buffer.size() > bufferSize) {
            buffer.pop();
        }
    }
}