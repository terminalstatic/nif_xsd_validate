# NifXsdValidate

An Elixir package for Xml Validation, based on libxml2.

## Installation

Although it would be probably possible to make this work it won't currently compile for windows.

Prerequisites for building:
* curl
* build tools (gcc, make, ...)

The package can be installed by adding `nif_xsd_validate` to your list of dependencies in `mix.exs`:

```elixir
def deps do
  [
    {:nif_xsd_validate, git: "https://github.com/terminalstatic/nif_xsd_validate"}
  ]
end
```

## Usage
Init the Storage Agent in the application config or manually.
The validition is done against the schema identified by the key.

```elixir
NifXsd.Schema.start_link(%{someSchemaKey1: "url://to/schema1"})
NifXsd.validate(NifXsd.Schema.get(:someSchemaKey1), "<xml></xml>")

NifXsd.Schema.start_link(%{someSchemaKey2: "url://to/schema2"})
NifXsd.validate(NifXsd.Schema.get(:someSchemaKey2), "<xml></xml>")
```



