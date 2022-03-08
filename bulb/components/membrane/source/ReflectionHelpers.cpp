#include "Membrane/ReflectionHelpers.hpp"

#include "Membrane/MembraneLog.hpp"

#include <Clove/ReflectionAttributes.hpp>
#include <msclr/marshal_cppstd.h>

using namespace clove;

namespace membrane {
    System::Collections::Generic::List<AvailableTypeInfo ^> ^ReflectionHelper::getEditorVisibleComponents() {
        System::Collections::Generic::List<AvailableTypeInfo ^> ^list { gcnew System::Collections::Generic::List<AvailableTypeInfo ^>{} };

        std::vector<reflection::TypeInfo const *> editorTypes{ reflection::getTypesWithAttribute<EditorVisibleComponent>() };
        for(auto const *typeInfo : editorTypes) {
            auto const visibleAttribute{ typeInfo->attributes.get<EditorVisibleComponent>().value() };

            AvailableTypeInfo ^availableTypeInfo { gcnew AvailableTypeInfo{} };
            availableTypeInfo->displayName = gcnew System::String{ visibleAttribute.name.value_or(typeInfo->name).c_str() };
            availableTypeInfo->typeName    = gcnew System::String{ typeInfo->name.c_str() };
            list->Add(availableTypeInfo);
        }

        return list;
    }

    System::Collections::Generic::List<AvailableTypeInfo ^> ^ ReflectionHelper::getEditorVisibleSubSystems() {
        System::Collections::Generic::List<AvailableTypeInfo ^> ^ list { gcnew System::Collections::Generic::List<AvailableTypeInfo ^>{} };

        std::vector<reflection::TypeInfo const *> editorTypes{ reflection::getTypesWithAttribute<EditorVisibleSubSystem>() };
        for(auto const *typeInfo : editorTypes) {
            auto const visibleAttribute{ typeInfo->attributes.get<EditorVisibleSubSystem>().value() };

            AvailableTypeInfo ^ availableTypeInfo { gcnew AvailableTypeInfo{} };
            availableTypeInfo->displayName = gcnew System::String{ visibleAttribute.name.value_or(typeInfo->name).c_str() };
            availableTypeInfo->typeName    = gcnew System::String{ typeInfo->name.c_str() };
            list->Add(availableTypeInfo);
        }

        return list;
    }

    System::Collections::Generic::List<EditorTypeInfo ^> ^ constructMembers(std::vector<reflection::MemberInfo> const &members, void const *const memory, size_t offsetIntoParent) {
        auto editorVisibleMembers{ gcnew System::Collections::Generic::List<EditorTypeInfo ^>{} };

        for(auto const &member : members) {
            size_t const totalMemberOffset{ offsetIntoParent + member.offset };

            if(std::optional<EditorEditableMember> attribute{ member.attributes.get<EditorEditableMember>() }) {
                auto memberInfo{ gcnew EditorTypeInfo{} };
                memberInfo->typeName    = gcnew System::String{ member.name.c_str() };
                memberInfo->displayName = gcnew System::String{ attribute->name.value_or(member.name).c_str() };
                memberInfo->offset      = totalMemberOffset;
                if(reflection::TypeInfo const *memberTypeInfo{ reflection::getTypeInfo(member.id) }) {
                    memberInfo->type     = EditorTypeType::Parent;
                    memberInfo->typeData = constructMembers(memberTypeInfo->members, memory, totalMemberOffset);
                } else {
                    memberInfo->type = EditorTypeType::Value;

                    if(attribute->onEditorGetValue != nullptr) {
                        std::string value{ attribute->onEditorGetValue(reinterpret_cast<uint8_t const *const>(memory), totalMemberOffset, member.size) };
                        memberInfo->typeData = gcnew System::String{ value.c_str() };
                    } else {
                        CLOVE_LOG(Membrane, clove::LogLevel::Error, "EditorEditableMember {0} does not provide an implementation for onEditorGetValue", member.name);
                        memberInfo->typeData = gcnew System::String{ "" };
                    }
                }

                editorVisibleMembers->Add(memberInfo);
            } else if(std::optional<EditorEditableDropdown> attribute{ member.attributes.get<EditorEditableDropdown>() }) {
                auto memberInfo{ gcnew EditorTypeInfo{} };
                memberInfo->typeName    = gcnew System::String{ member.name.c_str() };
                memberInfo->displayName = gcnew System::String{ attribute->name.value_or(member.name).c_str() };
                memberInfo->offset      = totalMemberOffset;
                memberInfo->type        = EditorTypeType::Dropdown;

                EditorTypeDropdown ^ dropdownData { gcnew EditorTypeDropdown };
                dropdownData->currentSelection = attribute->getSelectedIndex(reinterpret_cast<uint8_t const *const>(memory), totalMemberOffset, member.size);
                dropdownData->dropdownItems    = gcnew System::Collections::Generic::List<System::String ^>{};
                for(auto item : attribute->getDropdownMembers()) {
                    dropdownData->dropdownItems->Add(gcnew System::String{ item.c_str() });
                }
                if(attribute->getTypeInfoForMember != nullptr) {
                    std::vector<reflection::MemberInfo> dropdownMemberInfos{};
                    for(auto item : attribute->getDropdownMembers()) {
                        //Construct member infos in order to call this function again. This is a bit hacky but allows us to reuse a lot of code and handle all cases
                        reflection::TypeInfo const *itemTypeInfo{ attribute->getTypeInfoForMember(item) };

                        reflection::MemberInfo info{
                            itemTypeInfo->name,
                            itemTypeInfo->id,
                            0,
                            itemTypeInfo->size
                        };
                        info.attributes.add(EditorEditableMember{ item });

                        dropdownMemberInfos.push_back(std::move(info));
                    }

                    dropdownData->dropdownTypeInfos = constructMembers(dropdownMemberInfos, memory, totalMemberOffset);
                }

                memberInfo->typeData = dropdownData;

                editorVisibleMembers->Add(memberInfo);
            }
        }

        return editorVisibleMembers;
    }

    EditorTypeInfo ^ constructComponentEditorTypeInfo(reflection::TypeInfo const *typeInfo, void const *const memory) {
              EditorVisibleComponent const visibleAttribute{ typeInfo->attributes.get<EditorVisibleComponent>().value() };

              auto editorTypeInfo{ gcnew EditorTypeInfo{} };
              editorTypeInfo->typeName    = gcnew System::String{ typeInfo->name.c_str() };
              editorTypeInfo->displayName = gcnew System::String{ visibleAttribute.name.value_or(typeInfo->name).c_str() };
              editorTypeInfo->type        = EditorTypeType::Parent;
              editorTypeInfo->typeData    = constructMembers(typeInfo->members, memory, 0);

              return editorTypeInfo;
          }

    void modifyComponentMember(uint8_t *const memory, reflection::TypeInfo const *typeInfo, std::string_view value, size_t const requiredOffset, size_t currentOffset) {
        for(auto const &member : typeInfo->members) {
            size_t const totalMemberOffset{ currentOffset + member.offset };
            reflection::TypeInfo const *memberTypeInfo{ reflection::getTypeInfo(member.id) };

            if(memberTypeInfo != nullptr) {
                modifyComponentMember(memory, memberTypeInfo, value, requiredOffset, totalMemberOffset);
            } else if(totalMemberOffset == requiredOffset) {
                if(std::optional<EditorEditableMember> attribute{ member.attributes.get<EditorEditableMember>() }) {
                    attribute->onEditorSetValue(memory, totalMemberOffset, member.size, value);
                } else if(std::optional<EditorEditableDropdown> attribute{ member.attributes.get<EditorEditableDropdown>() }) {
                    attribute->setSelectedItem(memory, totalMemberOffset, member.size, value);
                } else {
                    CLOVE_LOG(Membrane, LogLevel::Error, "{0}: Reached required member but it does not have an editable attribute.", CLOVE_FUNCTION_NAME);
                }
                return;
            }
        }
    }

    serialiser::Node serialiseComponent(reflection::TypeInfo const *const componentTypeInfo, uint8_t const *const componentMemory, size_t currentOffset) {
        if(componentTypeInfo == nullptr) {
            return {};
        }

        serialiser::Node members{};

        for(auto const &member : componentTypeInfo->members) {
            size_t const totalMemberOffset{ currentOffset + member.offset };

            if(reflection::TypeInfo const *memberType{ reflection::getTypeInfo(member.id) }) {
                members[member.name] = serialiseComponent(memberType, componentMemory, totalMemberOffset);
            } else {
                if(std::optional<EditorEditableMember> attribute{ member.attributes.get<EditorEditableMember>() }) {
                    members[member.name] = attribute->onEditorGetValue(componentMemory, totalMemberOffset, member.size);
                } else if(std::optional<EditorEditableDropdown> attribute{ member.attributes.get<EditorEditableDropdown>() }) {
                    size_t const index{ attribute->getSelectedIndex(componentMemory, totalMemberOffset, member.size) };
                    if(attribute->getTypeInfoForMember != nullptr) {
                        reflection::TypeInfo const *const selectedTypeInfo{ attribute->getTypeInfoForMember(attribute->getDropdownMembers()[index]) };

                        members[member.name]["selection"] = index;
                        members[member.name]["value"]     = serialiseComponent(selectedTypeInfo, componentMemory, totalMemberOffset);
                    } else {
                        members[member.name] = index;
                    }
                }
            }
        }

        return members;
    }

    void deserialiseComponent(reflection::TypeInfo const *const componentTypeInfo, uint8_t *const componentMemory, serialiser::Node const &componentNode, size_t currentOffset) {
        for(auto const &member : componentTypeInfo->members) {
            size_t const totalMemberOffset{ currentOffset + member.offset };

            if(reflection::TypeInfo const *memberType{ reflection::getTypeInfo(member.id) }) {
                deserialiseComponent(memberType, componentMemory, componentNode[member.name], totalMemberOffset);
            } else {
                if(std::optional<EditorEditableMember> attribute{ member.attributes.get<EditorEditableMember>() }) {
                    attribute->onEditorSetValue(componentMemory, totalMemberOffset, member.size, componentNode[member.name].as<std::string>());
                } else if(std::optional<EditorEditableDropdown> attribute{ member.attributes.get<EditorEditableDropdown>() }) {
                    std::vector<std::string> const dropdownMembers{ attribute->getDropdownMembers() };
                    serialiser::Node const &dropdownNode{ componentNode[member.name] };

                    bool const isOnlyIndex{ dropdownNode.getType() == serialiser::Node::Type::Scalar };
                    if(isOnlyIndex) {
                        size_t const index{ dropdownNode.as<size_t>() };
                        attribute->setSelectedItem(componentMemory, totalMemberOffset, member.size, dropdownMembers[index]);
                    } else {
                        size_t const index{ dropdownNode["selection"].as<size_t>() };
                        attribute->setSelectedItem(componentMemory, totalMemberOffset, member.size, dropdownMembers[index]);

                        reflection::TypeInfo const *const selectedTypeInfo{ attribute->getTypeInfoForMember(dropdownMembers[index]) };
                        deserialiseComponent(selectedTypeInfo, componentMemory, dropdownNode["value"], totalMemberOffset);
                    }
                }
            }
        }
    }
}