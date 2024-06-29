template <typename ModuleClass>
concept Module = requires (ModuleClass m){
    {ModuleClass::get_name()} -> std::same_as<std::string>;
};