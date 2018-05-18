# NifXsdValidate

An elixir package for validating xml content against xsd, based on libxml2.
It is basically meant to preload xsd schemata from given urls and use those throughout the lifetime of an application.
This is quite an early version, right now the validation returns the status and a list of strings, this will most likely change to a more verbose structure in future versions.

## Installation

Although it would be probably possible to make this work it won't currently compile for windows.

Prerequisites for building:
* curl
* build tools (gcc, make, ...)

The package can be installed by adding `nif_xsd_validate` to the list of dependencies in `mix.exs`:

```elixir
def deps do
  [
    {:nif_xsd_validate, git: "https://github.com/terminalstatic/nif_xsd_validate"}
  ]
end
```

## Usage

Initialize the schema agent in the application module or manually.
Schemata (Libxml2 xmlSchemaPtrs) are stored in a map and identified by a key.

```elixir
NifXsd.Schema.start_link(%{someSchemaKey1: "url://to/schema1"})
NifXsd.validate(NifXsd.Schema.get(:someSchemaKey1), "<xml></xml>")
```
Supervisor configuration example:
```elixir
children = [
supervisor(NifXsd.Schema,[%{someSchemaKey1: "url://to/schema1", someSchemaKey2: "url://to/schema2"])
]
```
Validation works as expected:
```elixir
NifXsd.validate(NifXsd.Schema.get(:someSchemaKey1), "<xml></xml>")
NifXsd.validate(NifXsd.Schema.get(:someSchemaKey2), "<xml></xml>")
```
## Plug example
It comes in quite handy when used in combination with [Plug](https://github.com/elixir-plug/plug), a quick example could look like this:

```elixir
defmodule ExamplePlug.XsdValidate do
    import Plug.Conn
    def init(default), do: default
  
    def call(conn, _) do
        {:ok, body, _} = Plug.Conn.read_body(conn)    
        
        case NifXsd.validate(NifXsd.Schema.get(:schema), body) do
            {:ok, _} -> assign(conn, :xml_body, body)
            {:error, reason} -> 
            reason = 
            "<Errors>" 
            <> (reason
            |>Enum.map(fn(x) -> "<Error>" <> x <> "</Error>" end)
            |>Enum.join)
            <>"</Errors>"
            conn
            |> put_resp_content_type("text/xml")
            |> send_resp(400, reason)
            |> halt
        end
    end
end
```
