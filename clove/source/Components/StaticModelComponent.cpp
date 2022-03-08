#include "Clove/Components/StaticModelComponent.hpp"

#include "Clove/Application.hpp"
#include "Clove/FileSystem/AssetManager.hpp"
#include "Clove/ReflectionAttributes.hpp"

CLOVE_REFLECT_BEGIN(clove::StaticModelComponent, clove::EditorVisibleComponent{
                                                     .name                     = "Static Model Component",
                                                     .onEditorCreateComponent  = &createComponentHelper<clove::StaticModelComponent>,
                                                     .onEditorGetComponent     = &getComponentHelper<clove::StaticModelComponent>,
                                                     .onEditorDestroyComponent = &destroyComponentHelper<clove::StaticModelComponent>,
                                                 })
CLOVE_REFLECT_MEMBER(model, clove::EditorEditableMember{
                                .onEditorGetValue = [](uint8_t const *const memory, size_t offset, size_t size) -> std::string {
                                    CLOVE_ASSERT(size == sizeof(AssetPtr<StaticModel>));

                                    auto const *const value{ reinterpret_cast<AssetPtr<StaticModel> const *const>(memory + offset) };
                                    return value->getPath().string();
                                },
                                .onEditorSetValue = [](uint8_t *const memory, size_t offset, size_t size, std::string_view value) { 
                                    CLOVE_ASSERT(size == sizeof(AssetPtr<StaticModel>));

                                    auto *const assetPtr{ reinterpret_cast<AssetPtr<StaticModel> *const>(memory + offset) };
                                    *assetPtr = clove::Application::get().getAssetManager()->getStaticModel(value); },
                            })
CLOVE_REFLECT_MEMBER(material, clove::EditorEditableMember{})
CLOVE_REFLECT_END