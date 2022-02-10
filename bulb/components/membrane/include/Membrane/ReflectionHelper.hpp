#pragma once

namespace membrane {
    public enum class EditorTypeType{
        Value,
        Parent,
        Dropdown,
    };

    public ref class EditorTypeDropdown {
        //VARIABLES
    public:
        System::Int32 currentSelection{};
        System::Collections::Generic::List<System::String ^> ^ dropdownItems { nullptr };

        System::Collections::Generic::List<ref class EditorTypeInfo ^> ^ dropdownTypeInfos { nullptr };
    };

    public ref class EditorTypeInfo {
        //VARIABLES
    public:
        System::String ^typeName { nullptr };
        System::String ^displayName { nullptr };

        System::UInt32 offset{ 0 }; /**< Offset into the parent of this type. */

        EditorTypeType type{};
        System::Object ^typeData{}; //Data dependant on the type. If it is a value then it is a string, if it is a parent then it is an array of members etc.
    };

    public ref class AvailableTypeInfo {
        //VARIABLES
    public:
        System::String ^typeName { nullptr };
        System::String ^displayName { nullptr };
    };

    /**
     * @brief Allows the editor to easily access reflected types.
     */
    public ref class ReflectionHelper {
        //FUNCTIONS
    public:
        static System::Collections::Generic::List<AvailableTypeInfo ^> ^getAvailableTypes();
    };
}