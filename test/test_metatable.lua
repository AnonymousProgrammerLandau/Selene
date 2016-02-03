function call_method()
   instance = get_instance()
   return instance:baz()
end

function access_member()
   instance = get_instance()
   return instance:qux()
end

function return_both_instances()
   return get_instance(), get_instance2()
end