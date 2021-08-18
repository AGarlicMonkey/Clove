#pragma once

#include "Clove/Layer.hpp"

namespace clove {
    class EntityManager;
}

namespace clove {
    /**
	 * @brief Plays poisitional audio
	 */
    class AudioLayer : public Layer{
        //VARIABLES
    private:
        EntityManager *entityManager{ nullptr };
        
        //FUNCTIONS
    public:
		AudioLayer() = delete;
        AudioLayer(EntityManager *entityManager);

        AudioLayer(AudioLayer const &other) = delete;
        AudioLayer(AudioLayer &&other) noexcept;

        AudioLayer &operator=(AudioLayer const &other) = delete;
        AudioLayer &operator=(AudioLayer &&other) noexcept;

        ~AudioLayer();

        void onUpdate(DeltaTime const deltaTime) override;
    };
}
