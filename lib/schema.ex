defmodule NifXsd.Schema do
  @typedoc "The opaque resource type that wraps an xmlSchemaPtr."
  @type resource :: any()

  @doc """
  Starts the agent for the schema map with an empty map.
  """
  def start_link() do
    Agent.start_link(fn -> %{} end, name: __MODULE__)
  end

  @doc """
  Starts the agent for the schema map with init_map.
  ## Example
      NifXsd.Schema.start_link(%{someSchemaKey1: "url://to/schema1", someSchemaKey2: "url://to/schema2"})
  In an application file:
      supervisor(NifXsd.Schema,[%{someSchemaKey1: "url://to/schema1", someSchemaKey2: "url://to/schema2"}])
  """
  def start_link(init_map) do
    Agent.start_link(
      fn ->
        init_map
        |> Enum.map(fn {k, v} ->
          {:ok, schema} = NifXsd.load_schema(v)
          {k, schema}
        end)
        |> Enum.into(%{})
      end,
      name: __MODULE__
    )
  end

  @doc """
  Puts an entry into the schema map.
  ## Example
      NifXsd.Schema.put(:newSchemaKey, "url://to/newSchema")
  """
  @spec put(atom(), String.t()) :: :ok | {:error, String.t()}
  def put(key, value) do
    case NifXsd.load_schema(value) do
      {:ok, schema} ->
        Agent.update(__MODULE__, &Map.put(&1, key, schema))
        :ok

      {:error, reason} ->
        {:error, reason}
    end
  end

  @doc """
  Returns a resource(xmlSchemaPtr) by its key, see `NifXsd.validate/2`.
  """
  @spec get(atom()) :: resource
  def get(key) do
    Agent.get(__MODULE__, &Map.get(&1, key))
  end

  @doc """
  Deletes an entry from the schema map.
  ## Example
      NifXsd.Schema.delete(:schemaKey)
  """
  @spec delete(atom()) :: :ok
  def delete(key) do
    Agent.update(__MODULE__, &Map.delete(&1, key))
  end
end
