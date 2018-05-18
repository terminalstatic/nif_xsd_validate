defmodule NifXsd.Schema do  
    def start_link(init_map) do  
        Agent.start_link(fn -> 
                init_map
                |> Enum.map(fn {k, v} -> 
                    {:ok, schema} = NifXsd.load_schema(v)
                    {k,schema}
                end)
                |> Enum.into(%{})
            end, name: __MODULE__)  
    end
    
    def put(key, value) do  
        Agent.update(__MODULE__, &Map.put(&1, key, value))  
    end
    
    def get(key) do  
        Agent.get(__MODULE__, &Map.get(&1, key))  
    end  
end  