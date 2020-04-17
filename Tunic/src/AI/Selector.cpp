#include "Tunic/AI/Selector.hpp"

namespace tnc::ai {
	Selector::Selector() = default;

	Selector::Selector(Selector&& other) noexcept = default;

	Selector& Selector::operator=(Selector&& other) noexcept = default;

	Selector::~Selector() = default;

	Task::Status Selector::activate(clv::utl::DeltaTime deltaTime) {
		for(const auto& child : children) {
			const Status status = child->activate(deltaTime);
			if(status != Status::failure) {
				return status;
			}
		}
		return Status::failure;
	}
}