defmodule NifXsd do
  require Logger
  @on_load :load_nifs
  
    @doc false
    def load_nifs do
      case :erlang.load_nif(Path.join(:code.priv_dir(:nif_xsd_validate), "nif_xsd_validate"), 0) do
        {:error, {_, error_text}} -> Logger.error "NIF load_nif failed: #{error_text}"
        :ok -> Logger.info "NIF load_nif successful"
      end
    end

    @doc """
    Takes resource(xmlSchemaPtr) to validate the xml string.
    Use `NifXsd.Schema` to manage resources.
    ## Example
        NifXsd.validate(NifXsd.Schema.get(:someSchemaKey1), "<xml></xml>")
    """ 
    @spec validate(any(), String.t) :: :ok|{:error, [String.t]}
    def validate(_resource, _xml) do
      raise "NIF validate/2 not implemented, load_nif probably failed"
    end

    @doc false
    def load_schema(_path) do
        raise "NIF load_schema/1 not implemented, load_nif probably failed"
      end  
  end