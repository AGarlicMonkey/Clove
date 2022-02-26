namespace clove {
    Guid::Guid(Type guid)
        : guid{ guid } {
    }

    Guid::Guid(Guid const &other) = default;

    Guid::Guid(Guid &&other) noexcept = default;

    Guid &Guid::operator=(Guid const &other) = default;

    Guid &Guid::operator=(Guid &&other) noexcept = default;

    Guid::~Guid() = default;

    Guid::Type Guid::getId() const {
        return guid;
    }

    Guid::operator Guid::Type() const {
        return getId();
    }
}