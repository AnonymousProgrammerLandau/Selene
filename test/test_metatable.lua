function call_method()
   instance = get_instance()
   return instance:baz()
end

function access_member()
   instance = get_instance()
   return instance:qux()
end

function types_stay_the_same()
   instance = get_instance()
   type_of_instance = getmetatable(instance)
   get_instance2()
   return rawequal(type_of_instance, getmetatable(instance))
end

function have_different_metatables(a, b)
   return not rawequal(getmetatable(a), getmetatable(b))
end