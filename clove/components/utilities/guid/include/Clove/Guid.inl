namespace clove{
    Guid::Guid(Guid &&other) noexcept = default;

    Guid &Guid::operator=(Guid &&other) noexcept = default;

    Guid::~Guid() = default;

    uint64_t Guid::getId() const {
        return guid;
    }

    Guid::operator uint64_t() const {
        return getId();
    }
}