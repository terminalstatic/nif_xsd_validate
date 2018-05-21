defmodule NifXsd do
  require Logger
  
  @typedoc "The opaque resource type that wraps an xmlSchemaPtr."
  @opaque resource :: any()

  @on_load :load_nifs
  
    @doc false
    def load_nifs do
      case :erlang.load_nif(String.to_charlist(Path.join(:code.priv_dir(:nif_xsd_validate), "nif_xsd_validate")), 0) do
        :ok -> Logger.info "NIF load successful"
        {:error, {_, error_text}} -> Logger.error "NIF load failed: #{error_text}"
      end
    end

    @doc """
    Takes resource(xmlSchemaPtr) to validate the xml string.
    Use `NifXsd.Schema` to manage resources.
    ## Example
        NifXsd.validate(NifXsd.Schema.get(:someSchemaKey1), "<xml></xml>")
    """ 
    @spec validate(resource, String.t) :: :ok|{:error, [String.t]}
    def validate(_resource, _xml) do
      raise "NIF validate/2 not implemented, load_nif probably failed"
    end

    @doc false
    def load_schema(_path) do
        raise "NIF load_schema/1 not implemented, load_nif probably failed"
      end  
  end