namespace clove {
    Guid::Guid(uint64_t guid)
        : guid{ guid } {
    }

    Guid::Guid(Guid const &other) = default;

    Guid::Guid(Guid &&other) noexcept = default;

    Guid &Guid::operator=(Guid const &other) = default;

    Guid &Guid::operator=(Guid &&other) noexcept = default;

    Guid::~Guid() = default;

    uint64_t Guid::getId() const {
        return guid;
    }

    Guid::operator uint64_t() const {
        return getId();
    }
}