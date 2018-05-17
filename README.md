# NifXsdValidate

### !!!This is work in progress, api will change in the next few days !!! ###

An Elixir package for Xml Validation, based on libxml2.

## Installation

Although it would be probably possible to make this work it won't currently compile for windows.

Prerequisites for building:
* curl
* pkg-config

The package can be installed
by adding `nif_xsd_validate` to your list of dependencies in `mix.exs`:

```elixir
def deps do
  [
    {:nif_xsd_validate, git: "https://github.com/terminalstatic/nif_xsd_validate"}
  ]
end
```

## Usage
Init the Storage Agent in the application config or manually:
```elixir
Schema.Store.start_link(%{schema: "url://to/schema"})
ValidateXsd.validate(:schema, "<xml></xml>")
```
