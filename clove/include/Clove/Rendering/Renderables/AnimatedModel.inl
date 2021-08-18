namespace clove {
    std::array<mat4f, MAX_JOINTS> AnimatedModel::update(DeltaTime const deltaTime) {
        return animator.update(deltaTime);
    }
}