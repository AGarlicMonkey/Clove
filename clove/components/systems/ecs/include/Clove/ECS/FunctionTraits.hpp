#pragma once

namespace clove {
    template<typename FunctionType>
    struct FunctionTraits;

    template<typename RetType, typename... ParameterTypes>
    struct FunctionTraits<RetType(ParameterTypes...)> {
        using ReturnType               = RetType;
        using ParameterTypesTuple      = std::tuple<ParameterTypes...>;
        using DecayParameterTypesTuple = std::tuple<std::decay_t<ParameterTypes>...>;

        static inline size_t constexpr arity{ sizeof...(ParameterTypes) };
    };

    //Free function specialisation
    template<typename RetType, typename... ParameterTypes>
    struct FunctionTraits<RetType (*)(ParameterTypes...)> : public FunctionTraits<RetType(ParameterTypes...)> {};

    //Member function specialisation
    template<typename RetType, typename ClassType, typename... ParameterTypes>
    struct FunctionTraits<RetType (ClassType::*)(ParameterTypes...)> : public FunctionTraits<RetType(ParameterTypes...)> {};

    //Const member function specialisation
    template<typename RetType, typename ClassType, typename... ParameterTypes>
    struct FunctionTraits<RetType (ClassType::*)(ParameterTypes...) const> : public FunctionTraits<RetType(ParameterTypes...)> {};

    //Callable object specialisation
    template<typename CallableType>
    struct FunctionTraits : public FunctionTraits<decltype(&CallableType::operator())> {};
}